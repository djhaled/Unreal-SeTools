#include "Factories/C2AnimAssetFactory.h"
#include "Containers/UnrealString.h"
#include "Structures/C2Anim.h"
#include "ToolMenus.h"
#include "Engine.h"
#include "Misc/FileHelper.h"
#include "AssetToolsModule.h"
#include "Widgets/SAnimOptions.h"
#include "Structures/C2Material.h"
#include "Widgets/SAnimImportOption.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/ScopedSlowTask.h"
#include "Commandlets/ImportAssetsCommandlet.h"
#include "FileHelpers.h"

/* UTextAssetFactory structors
 *****************************************************************************/

UC2AnimAssetFactory::UC2AnimAssetFactory( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	Formats.Add(TEXT("seanim;seanim Animation File"));
	SupportedClass = UAnimSequence::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
    SettingsImporter = CreateDefaultSubobject<USAnimOptions>(TEXT("Anim Options"));
}


/* UFactory overrides
 *****************************************************************************/


UObject* UC2AnimAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FScopedSlowTask SlowTask(5, NSLOCTEXT("C2MFactory", "BeginReadC2MFile", "Opening C2Anim file."), true);
	if (Warn->GetScopeStack().Num() == 0)
	{
		// We only display this slow task if there is no parent slowtask, because otherwise it is redundant and doesn't display any relevant information on the progress.
		// It is primarly used to regroup all the smaller import sub-tasks for a smoother progression.
		SlowTask.MakeDialog(true);
	}
	SlowTask.EnterProgressFrame(0);

    // picker
    if (SettingsImporter->bInitialized == false)
    {
        TSharedPtr<SAnimImportOption> ImportOptionsWindow;
        TSharedPtr<SWindow> ParentWindow;
        if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
        {
            IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
            ParentWindow = MainFrame.GetParentWindow();
        }

        TSharedRef<SWindow> Window = SNew(SWindow)
            .Title(LOCTEXT("DatasmithImportSettingsTitle", "SEAnim Import Options"))
            .SizingRule(ESizingRule::Autosized);
        Window->SetContent
        (
            SAssignNew(ImportOptionsWindow, SAnimImportOption)
            .WidgetWindow(Window)
        );
        SettingsImporter = ImportOptionsWindow.Get()->Stun;
        FSlateApplication::Get().AddModalWindow(Window, ParentWindow, false);
        bImport = ImportOptionsWindow.Get()->ShouldImport();
        bImportAll = ImportOptionsWindow.Get()->ShouldImportAll();
        SettingsImporter->bInitialized = true;
    }
    //
	UAnimSequence* AnimSequence = NewObject<UAnimSequence>(InParent, UAnimSequence::StaticClass(), InName, Flags);
    USkeleton* Skeleton = SettingsImporter->Skeleton.Get();
	AnimSequence->SetSkeleton(Skeleton);
    USkeletalMesh* SkeletalMesh = Skeleton->GetPreviewMesh();
	AnimSequence->CreateAnimation(SkeletalMesh);
	auto MeshBones = Skeleton->GetReferenceSkeleton().GetRawRefBoneInfo();
	FRawAnimSequenceTrack RawTrack;
	TArray64<uint8> FileDataOld;
    if (FFileHelper::LoadFileToArray(FileDataOld, *Filename))
    {
        FLargeMemoryReader Reader(FileDataOld.GetData(), FileDataOld.Num());
        C2Anim* Anim = new C2Anim();
        Anim->ParseAnim(Reader);
        AnimSequence->SetRawNumberOfFrame(Anim->Header.FrameCountBuffer);
        AnimSequence->SequenceLength = Anim->Header.FrameCountBuffer / Anim->Header.FrameRate;
        UE_LOG(LogTemp, Warning, TEXT("This animation '%s' is of type %s"), *Filename,*UEnum::GetValueAsString(Anim->Header.AnimType));
        for (int32 BoneIndex = 0; BoneIndex < Anim->BonesInfos.Num(); BoneIndex++)
        {
            const BoneInfo& BoneInfo = Anim->BonesInfos[BoneIndex];
            if (!hasBone(MeshBones, BoneInfo.Name)){continue;}
            FRawAnimSequenceTrack RawAnimSequenceTrack;
            for (uint32_t FrameIndex = 0; FrameIndex < Anim->Header.FrameCountBuffer; FrameIndex++)
            {
                FVector Position = BoneInfo.GetPositionAtFrame(FrameIndex);
                
                FQuat Rotation = BoneInfo.GetRotationAtFrame(FrameIndex);

                FVector Scale = BoneInfo.GetScaleAtFrame(FrameIndex);

                if (Position != FVector(-1, -1, -1))
                {
                    Position.Y *= -1.0;
                    const int32 TrackIndex = AnimSequence->GetAnimationTrackNames().Find(FName(BoneInfo.Name));
                    const FVector& InitialPosition = AnimSequence->GetRawAnimationTrack(TrackIndex).PosKeys[0];

                    if (Anim->Header.AnimType == ESEAnimAnimationType::SEANIM_ABSOLUTE)
                    {
                        Anim->FixPositionAbsolute(Position, InitialPosition);
                    }
                    RawAnimSequenceTrack.PosKeys.Add(Position);
                }

                if (Rotation != FQuat(-1, -1, -1, -1))
                {
                    FRotator LocalRotator = Rotation.Rotator();
                    LocalRotator.Yaw *= -1.0f;
                    LocalRotator.Roll *= -1.0f;
                    Rotation = LocalRotator.Quaternion();
                    const int32 TrackIndex = AnimSequence->GetAnimationTrackNames().Find(FName(BoneInfo.Name));
                    const FQuat& InitialRotation = AnimSequence->GetRawAnimationTrack(TrackIndex).RotKeys[0];

                    if (Anim->Header.AnimType == ESEAnimAnimationType::SEANIM_ABSOLUTE)
                    {
                        Anim->FixRotationAbsolute(Rotation, InitialRotation);
                    }
                    RawAnimSequenceTrack.RotKeys.Add(FQuat(Rotation.X, Rotation.Y, Rotation.Z, Rotation.W));
                }

                if (Scale != FVector(-1, -1, -1))
                {
                    RawAnimSequenceTrack.ScaleKeys.Add(Scale);
                }
                else
                {
                    RawAnimSequenceTrack.ScaleKeys.Add(FVector::OneVector);
                }
            }
            // fill out track 
            for (uint32_t i = RawAnimSequenceTrack.PosKeys.Num(); i < Anim->Header.FrameCountBuffer; i++)
            {
                if (RawAnimSequenceTrack.PosKeys.Num() != 1 && RawAnimSequenceTrack.PosKeys.Num() != 0)
                {
                    FVector LastV = RawAnimSequenceTrack.PosKeys.Last();
                    RawAnimSequenceTrack.PosKeys.Add(LastV);
                }
            }
            for (uint32_t i = RawAnimSequenceTrack.RotKeys.Num(); i < Anim->Header.FrameCountBuffer; i++)
            {
                if (RawAnimSequenceTrack.RotKeys.Num() != 1 && RawAnimSequenceTrack.RotKeys.Num() != 0)
                {
                    FQuat Last = RawAnimSequenceTrack.RotKeys.Last();
                    RawAnimSequenceTrack.RotKeys.Add(Last);
                }
            }
            AnimSequence->AddNewRawTrack(FName(BoneInfo.Name), &RawAnimSequenceTrack);
        }
        AnimSequence->MarkRawDataAsModified();


    }
    if (AnimSequence)
    {
        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        AssetEditorSubsystem->OpenEditorForAsset(AnimSequence);
    }
	return AnimSequence;
}
bool UC2AnimAssetFactory::hasBone(TArray<FMeshBoneInfo> Bones, const FString& AnimBoneName)
{

    for (const auto& Bone : Bones)
    {
        if (Bone.Name.ToString() == AnimBoneName)
        {
            return true;
        }
    }

    return false;
}

