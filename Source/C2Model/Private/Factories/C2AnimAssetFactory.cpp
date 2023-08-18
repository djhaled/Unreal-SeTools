#include "Factories/C2AnimAssetFactory.h"
#include "Containers/UnrealString.h"
#include "Structures/C2Anim.h"
#include "ToolMenus.h"
#include "Engine.h"
#include "Misc/FileHelper.h"
#include "Animation/AnimData/IAnimationDataController.h"
#include "AssetToolsModule.h"
#include "ComponentReregisterContext.h"
#include "Widgets/Animations/SAnimOptions.h"
#include "Structures/C2Material.h"
#include "Widgets/Animations/SAnimImportOption.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/ScopedSlowTask.h"
#include "Commandlets/ImportAssetsCommandlet.h"
#include "FileHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(C2AnimAssetFactory)
#define LOCTEXT_NAMESPACE "C2AnimAssetFactory"


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
            .Title(FText::FromString(TEXT("SEAnim Import Options")))
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
    constexpr bool bShouldTransact = false;
    USkeleton* Skeleton = SettingsImporter->Skeleton.LoadSynchronous();
    AnimSequence->SetSkeleton(Skeleton);

    IAnimationDataController& Controller = AnimSequence->GetController();
    Controller.OpenBracket(LOCTEXT("ImportAnimation_Bracket", "Importing Animation"));
    Controller.InitializeModel();

    // Delta
    FString DeltaRootBone = "tag_origin";
    //
    AnimSequence->ResetAnimation();
    auto MeshBones = Skeleton->GetReferenceSkeleton().GetRawRefBoneInfo();
    TArray64<uint8> FileDataOld;
    if (FFileHelper::LoadFileToArray(FileDataOld, *Filename))
    {
        FLargeMemoryReader Reader(FileDataOld.GetData(), FileDataOld.Num());
        C2Anim* Anim = new C2Anim();
        Anim->ParseAnim(Reader);
        Controller.SetFrameRate(FFrameRate(Anim->Header.FrameRate, 1),bShouldTransact);
        int test = Anim->Header.FrameCountBuffer;
        Controller.SetNumberOfFrames(FFrameNumber(test), bShouldTransact);
        UE_LOG(LogTemp, Warning, TEXT("This animation '%s' is of type %s"), *Filename, *UEnum::GetValueAsString(Anim->Header.AnimType));
        for (int32 BoneTreeIndex = 0; BoneTreeIndex < MeshBones.Num(); BoneTreeIndex++)
        {
            const FName BoneTreeName = Skeleton->GetReferenceSkeleton().GetBoneName(BoneTreeIndex);
            Controller.AddBoneCurve(BoneTreeName, bShouldTransact);
        }
        
        for (int32 BoneIndex = 0; BoneIndex < Anim->BonesInfos.Num(); BoneIndex++)
        {
            const BoneInfo& BoneInfo = Anim->BonesInfos[BoneIndex];
            if (!hasBone(MeshBones, BoneInfo.Name)) { continue; }
            if (BoneInfo.Name == "tag_camera") { continue; }
            FRawAnimSequenceTrack RawAnimSequenceTrack;
            //Anim->Header.FrameCountBuffer = 1;
            for (uint32_t FrameIndex = 0; FrameIndex < Anim->Header.FrameCountBuffer; FrameIndex++)
            {
                FVector3f Position = BoneInfo.GetPositionAtFrame(FrameIndex);
        
                FQuat4f Rotation = BoneInfo.GetRotationAtFrame(FrameIndex);
        
                FVector3f Scale = BoneInfo.GetScaleAtFrame(FrameIndex);
        
                if (Position != FVector3f(-1, -1, -1))
                {
                    Position.Y *= -1.0;
                    RawAnimSequenceTrack.PosKeys.Add(Position);
                }
        
                if (Rotation != FQuat4f(-1, -1, -1, -1))
                {
                    FRotator3f LocalRotator = Rotation.Rotator();
                    LocalRotator.Yaw *= -1.0f;
                    LocalRotator.Roll *= -1.0f;
                    Rotation = LocalRotator.Quaternion();
        
                    RawAnimSequenceTrack.RotKeys.Add(FQuat4f(Rotation.X, Rotation.Y, Rotation.Z, Rotation.W));
                }
        
                if (Scale != FVector3f(-1, -1, -1))
                {
                    RawAnimSequenceTrack.ScaleKeys.Add(Scale);
                }
                else
                {
                    RawAnimSequenceTrack.ScaleKeys.Add(FVector3f::OneVector);
                }
            }
            int frc = Anim->Header.FrameCountBuffer;
            for (size_t i = 0; i < Anim->Header.FrameCountBuffer; i++)
            {
                if (!RawAnimSequenceTrack.PosKeys.IsEmpty()&&  frc > RawAnimSequenceTrack.PosKeys.Num() )
                {
                    auto Copy = RawAnimSequenceTrack.PosKeys.Last();
                    RawAnimSequenceTrack.PosKeys.Add(Copy);
                }
                if (!RawAnimSequenceTrack.RotKeys.IsEmpty() && frc > RawAnimSequenceTrack.RotKeys.Num())
                {
                    auto Copy = RawAnimSequenceTrack.RotKeys.Last();
                    RawAnimSequenceTrack.RotKeys.Add(Copy);
                }
            }
            FC2Anims AnimL;
            AnimL.Name = FName(BoneInfo.Name);
            AnimL.PosKeys = RawAnimSequenceTrack.PosKeys;
            AnimL.RotKeys = RawAnimSequenceTrack.RotKeys;
            AnimL.ScaleKeys = RawAnimSequenceTrack.ScaleKeys;
            TracksAll.Add(AnimL);
            Controller.SetBoneTrackKeys(FName(BoneInfo.Name), RawAnimSequenceTrack.PosKeys, RawAnimSequenceTrack.RotKeys, RawAnimSequenceTrack.ScaleKeys,bShouldTransact);
        }

        Controller.NotifyPopulated();
        Controller.CloseBracket();
        AnimSequence->Modify(true);
        AnimSequence->PostEditChange();
        FAssetRegistryModule::AssetCreated(AnimSequence);
        AnimSequence->MarkPackageDirty();
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
