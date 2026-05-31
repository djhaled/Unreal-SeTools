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
#include "AnimationBlueprintLibrary.h"

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
    Bones = Skeleton->GetReferenceSkeleton().GetRawRefBoneInfo();
    BonePoses = Skeleton->GetReferenceSkeleton().GetRawRefBonePose();
    TArray64<uint8> FileDataOld;

	
    if (FFileHelper::LoadFileToArray(FileDataOld, *Filename))
    {
        FLargeMemoryReader Reader(FileDataOld.GetData(), FileDataOld.Num());
        C2Anim* Anim = new C2Anim();
        Anim->ParseAnim(Reader);
        if (SettingsImporter->bOverrideAnimType)
        {
            Anim->Header.AnimType = SettingsImporter->AnimType;
        }
        Controller.SetFrameRate(FFrameRate(Anim->Header.FrameRate, 1),bShouldTransact);
        Controller.SetNumberOfFrames(FFrameNumber(int(Anim->Header.FrameCountBuffer)), bShouldTransact);
        UE_LOG(LogTemp, Warning, TEXT("This animation '%s' is of type %s"), *Filename, *UEnum::GetValueAsString(Anim->Header.AnimType));


	    
        bool UseCurveSave=SettingsImporter->bUseCurveeSave;
        for (int32 BoneTreeIndex = 0; BoneTreeIndex < Bones.Num(); BoneTreeIndex++)
        {
            const FName BoneTreeName = Skeleton->GetReferenceSkeleton().GetBoneName(BoneTreeIndex); 

           FTransform RePoseTransform;                                                                              
           if(SettingsImporter->RefposeSequence&&SettingsImporter->RefposeSequence->GetSkeleton()==SettingsImporter->Skeleton)  
           {
               FAnimPoseEvaluationOptions EvaluationOptions;
               FAnimPose Pose;
               FName BoneName=BoneTreeName;
               UAnimPoseExtensions::GetAnimPoseAtFrame( SettingsImporter->RefposeSequence, SettingsImporter->PoseTime,EvaluationOptions,Pose);
               RePoseTransform =UAnimPoseExtensions::GetBonePose(Pose,BoneName,EAnimPoseSpaces::Local);
           }
            else
            {
                RePoseTransform=BonePoses[BoneTreeIndex];
            }


            Controller.AddBoneCurve(BoneTreeName, bShouldTransact);
            if(UseCurveSave)
            {
                FAnimationCurveIdentifier TransformCurveId(BoneTreeName, ERawCurveTrackTypes::RCT_Transform);           
                AnimSequence->GetController().AddCurve(TransformCurveId, AACF_DriveTrack | AACF_Editable);
                if (!(Anim->Header.AnimType == ESEAnimAnimationType::SEANIM_ABSOLUTE ))
                    AnimSequence->GetController().SetTransformCurveKey(TransformCurveId,0,RePoseTransform);
            }
            if(!(Anim->Header.AnimType == ESEAnimAnimationType::SEANIM_ABSOLUTE )&&!UseCurveSave)
            {

                TArray<FVector3f> PositionalKeys;
                TArray<FQuat4f> RotationalKeys;
                TArray<FVector3f> ScalingKeys;
                int32 longer=AnimSequence->GetNumberOfSampledKeys();
                for(int i=0;i<=longer;i++)
                {
                    PositionalKeys.Add(FVector3f(RePoseTransform.GetLocation()));
                    RotationalKeys.Add(FQuat4f(RePoseTransform.GetRotation()));
                    ScalingKeys.Add(FVector3f(1,1,1));

                }
                Controller.SetBoneTrackKeys(BoneTreeName, PositionalKeys, RotationalKeys, ScalingKeys);
               
            }
        }

        
       
        for (int32 BoneIndex = 0; BoneIndex < Anim->BonesInfos.Num(); BoneIndex++)
        {
            const BoneInfo& KeyFrameBone = Anim->BonesInfos[BoneIndex];   
            auto BoneMesh = GetBone(KeyFrameBone.Name);
            if (BoneMesh.Name == "None") { continue; }
            
            // Add Bone Transform curve, and add base pose transform to start

         
            FName NewCurveName(KeyFrameBone.Name);
            TArray<FVector3f> PositionalKeys;
            TArray<FQuat4f> RotationalKeys;
            TArray<FVector3f> ScalingKeys;
            uint32 CurrentFrame = 0;

           
            
          FTransform BonePoseTransform;
            if(SettingsImporter->RefposeSequence)
            {
                FAnimPoseEvaluationOptions EvaluationOptions;
                FAnimPose Pose;
                FName BoneName=FName(KeyFrameBone.Name);
                UAnimPoseExtensions::GetAnimPoseAtFrame( SettingsImporter->RefposeSequence, SettingsImporter->PoseTime,EvaluationOptions,Pose);
                BonePoseTransform =UAnimPoseExtensions::GetBonePose(Pose,BoneName,EAnimPoseSpaces::Local);
            }
            else
            {
                BonePoseTransform = BonePoses[GetBoneIndex(KeyFrameBone.Name)]; 
            }
            
            
            /*                        UE4                                     */
            //FSmartName NewCurveName;
          // Skeleton->AddSmartNameAndModify(USkeleton::AnimTrackCurveMappingName, FName(KeyFrameBone.Name), NewCurveName); 。
          //  Skeleton->AddCurveMetaData(FName(KeyFrameBone.Name));
          //  FAnimationCurveIdentifier TransformCurveId(NewCurveName, ERawCurveTrackTypes::RCT_Transform);

            

            /*                      ***  UE5 ***                                                        *///
        

            FAnimationCurveIdentifier TransformCurveId(FName(KeyFrameBone.Name), ERawCurveTrackTypes::RCT_Transform);  
            if(UseCurveSave)
            {
                Skeleton->AddCurveMetaData(FName(KeyFrameBone.Name));
                AnimSequence->GetController().AddCurve(TransformCurveId, AACF_DriveTrack | AACF_Editable);
            }
               
            /*                                                                                    *///
            

            
        // Abandoned, unable to restore complete proposal! Function to be placed above...
          // AnimSequence->GetController().SetTransformCurveKey(TransformCurveId,0,BonePoseTransform); 
            
            // Loop thru actual bone positions   
            if (KeyFrameBone.BonePositions.Num() > 0)
            {
                TArray<TArray<FRichCurveKey>> PosCurveKeys;
                TArray<FRichCurveKey> PositionKeysZ;
                TArray<FRichCurveKey> PositionKeysY;
                TArray<FRichCurveKey> PositionKeysX;
                
                FVector relative_transform= FVector(0,0,0); 
             
                if (Anim->Header.AnimType == ESEAnimAnimationType::SEANIM_ABSOLUTE )
                {
                    relative_transform = FVector(0,0,0);
                }
                else
                {
                    relative_transform = BonePoseTransform.GetLocation(); ///????
                }

                if(UseCurveSave)
                {
                    for (size_t i = 0; i < KeyFrameBone.BonePositions.Num(); i++)  
                    {
                        FVector boneFrameVector;
                        auto BonePosAnimFrame = KeyFrameBone.BonePositions[i];    
                        BonePosAnimFrame.Value[1] *= -1;
                        auto TimeInSeconds = static_cast<float>(BonePosAnimFrame.Frame) / Anim->Header.FrameRate;
                    
                        // Relative_transform should be 0.0.0 if absolute anim.. its needed for relative/delta/additive  
                        boneFrameVector.X = BonePosAnimFrame.Value[0] + relative_transform[0];
                        boneFrameVector.Y = BonePosAnimFrame.Value[1] + relative_transform[1];
                        boneFrameVector.Z = BonePosAnimFrame.Value[2] + relative_transform[2];

                  
                    
                        PositionKeysX.Add(FRichCurveKey(TimeInSeconds, boneFrameVector.X));
                        PositionKeysY.Add(FRichCurveKey(TimeInSeconds, boneFrameVector.Y));
                        PositionKeysZ.Add(FRichCurveKey(TimeInSeconds, boneFrameVector.Z));
                    
                    }
                    PosCurveKeys.Add(PositionKeysX);
                    PosCurveKeys.Add(PositionKeysY);
                    PosCurveKeys.Add(PositionKeysZ);
                
                    for (int i = 0; i < 3; ++i)
                    {
                        const EVectorCurveChannel Axis = static_cast<EVectorCurveChannel>(i);
                        UAnimationCurveIdentifierExtensions::GetTransformChildCurveIdentifier(TransformCurveId, ETransformCurveChannel::Position, Axis);
                        AnimSequence->GetController().SetCurveKeys(TransformCurveId, PosCurveKeys[i], bShouldTransact);
                    }
                }
                else
                {
                    CurrentFrame = 0;
                    for (int32 i = 0; i < KeyFrameBone.BonePositions.Num(); ++i,  ++CurrentFrame)  
                    {
                        FVector3f boneFrameVector,LastBoneFrameVector;
                        auto BonePosAnimFrame = KeyFrameBone.BonePositions[i];  
                        BonePosAnimFrame.Value[1] *= -1;
                    
                       
                        boneFrameVector.X = BonePosAnimFrame.Value[0] + relative_transform[0];//
                        boneFrameVector.Y = BonePosAnimFrame.Value[1] + relative_transform[1];
                        boneFrameVector.Z = BonePosAnimFrame.Value[2] + relative_transform[2];
                    
                        // 手动插值
                        if (i == 0 && BonePosAnimFrame.Frame > 0)
                        {
                           //开始设置下一针
                            while (CurrentFrame < BonePosAnimFrame.Frame)
                            {
                                PositionalKeys.Add(boneFrameVector);
                                ++CurrentFrame;
                            }
                        }
                        else
                        {
                            while (CurrentFrame < BonePosAnimFrame.Frame) 
                            {
                                WraithAnimFrame<FVector3f> LastBonePosAnimFrame = KeyFrameBone.BonePositions[i - 1];
                                PositionalKeys.Add(
                                    FMath::Lerp(LastBoneFrameVector,  boneFrameVector,
                                                static_cast<float>(CurrentFrame - LastBonePosAnimFrame.Frame) /
                                                static_cast<float>(BonePosAnimFrame.Frame - LastBonePosAnimFrame.Frame)));
                                ++CurrentFrame;
                            }
                        }
                        LastBoneFrameVector=boneFrameVector; 
                        PositionalKeys.Add(FVector3f(boneFrameVector));
                    }
                }
            }
                
          
            if (KeyFrameBone.BoneRotations.Num() > 0)
            {
                TArray<TArray<FRichCurveKey>> RotCurveKeys;
                TArray<FRichCurveKey> RotationKeysZ;
                TArray<FRichCurveKey> RotationKeysY;
                TArray<FRichCurveKey> RotationKeysX;

                
                FQuat Rel_Rotation;
                if (Anim->Header.AnimType == ESEAnimAnimationType::SEANIM_ADDITIVE)
                {
                    Rel_Rotation =  BonePoseTransform.GetRotation();
                }
                else
                {
                    Rel_Rotation = FQuat(0, 0, 0, 1);
                }
            
               if(UseCurveSave)
               {
                   for (size_t i = 0; i < KeyFrameBone.BoneRotations.Num(); i++)
                   {
                       auto BoneRotationKeyFrame = KeyFrameBone.BoneRotations[i];
                       BoneRotationKeyFrame.Value *=  FQuat4f(Rel_Rotation);
                       // Unreal uses other axis type than COD engine 
                       FRotator3f LocalRotator = BoneRotationKeyFrame.Value.Rotator(); 
                       LocalRotator.Yaw *= -1.0f;
                       LocalRotator.Roll *= -1.0f;
              
                       BoneRotationKeyFrame.Value = LocalRotator.Quaternion();
                       auto TimeInSeconds = static_cast<float>(BoneRotationKeyFrame.Frame) / Anim->Header.FrameRate;
                       RotationKeysX.Add(FRichCurveKey(TimeInSeconds, LocalRotator.Pitch));
                       RotationKeysY.Add(FRichCurveKey(TimeInSeconds, LocalRotator.Yaw));
                       RotationKeysZ.Add(FRichCurveKey(TimeInSeconds, LocalRotator.Roll));
                    
                   }
                   RotCurveKeys.Add(RotationKeysZ);
                   RotCurveKeys.Add(RotationKeysX);
                   RotCurveKeys.Add(RotationKeysY);
                   for (int i = 0; i < 3; ++i)
                   {
                       const EVectorCurveChannel Axis = static_cast<EVectorCurveChannel>(i);
                       UAnimationCurveIdentifierExtensions::GetTransformChildCurveIdentifier(TransformCurveId, ETransformCurveChannel::Rotation, Axis);
                       AnimSequence->GetController().SetCurveKeys(TransformCurveId, RotCurveKeys[i], bShouldTransact);
                   }
               }
        
               else
               {
                   FQuat4f LastRotator;
                   CurrentFrame = 0;
                   for (int32 i = 0; i < KeyFrameBone.BoneRotations.Num(); ++i, ++CurrentFrame)
                   {
                       WraithAnimFrame<FQuat4f> BoneRotationKeyFrame = KeyFrameBone.BoneRotations[i];
                       BoneRotationKeyFrame.Value *=  FQuat4f(Rel_Rotation);
                       // Unreal uses other axis type than COD engine
                       FRotator3f LocalRotator = BoneRotationKeyFrame.Value.Rotator();
                       LocalRotator.Yaw *= -1.0f;
                       LocalRotator.Roll *= -1.0f;
                       FQuat4f NewRotator = LocalRotator.Quaternion();
                    


                    
                       // 手动插值
                       if (i == 0 && BoneRotationKeyFrame.Frame > 0)
                       {
                           while (CurrentFrame < BoneRotationKeyFrame.Frame)
                           {
                               RotationalKeys.Add(NewRotator);
                               ++CurrentFrame;
                           }
                       }
                       else
                       {
                           while (CurrentFrame < BoneRotationKeyFrame.Frame)
                           {
                               uint32 LastFrame = KeyFrameBone.BoneRotations[i - 1].Frame;
                              
                               RotationalKeys.Add(
                                   FMath::Lerp(LastRotator, NewRotator,
                                               static_cast<float>(CurrentFrame - LastFrame) /
                                               static_cast<float>(BoneRotationKeyFrame.Frame - LastFrame)));
                               
                               ++CurrentFrame;
                           }
                       }
                       LastRotator = NewRotator;
                       RotationalKeys.Add(NewRotator);
                   }
               }

                CurrentFrame = 0;
                for (int32 i = 0; i < KeyFrameBone.BoneScale.Num(); ++i, ++CurrentFrame)
                {
                    WraithAnimFrame<FVector3f> BoneScaleKeyFrame = KeyFrameBone.BoneScale[i];

                    // 手动插值
                    if (i == 0 && BoneScaleKeyFrame.Frame > 0)
                    {
                        while (CurrentFrame < BoneScaleKeyFrame.Frame)
                        {
                            ScalingKeys.Add(BoneScaleKeyFrame.Value);
                            ++CurrentFrame;
                        }
                    }
                    else
                    {
                        while (CurrentFrame < BoneScaleKeyFrame.Frame)
                        {
                            WraithAnimFrame<FVector3f> LastScaleFrame = KeyFrameBone.BoneScale[i - 1];
                            ScalingKeys.Add(
                                FMath::Lerp(LastScaleFrame.Value, BoneScaleKeyFrame.Value,
                                            static_cast<float>(CurrentFrame - LastScaleFrame.Frame) /
                                            static_cast<float>(BoneScaleKeyFrame.Frame - LastScaleFrame.Frame)));
                            ++CurrentFrame;
                        }
                    }
                    ScalingKeys.Add(BoneScaleKeyFrame.Value);
                }
        
                
                int32 ArrLen = FMath::Max3(PositionalKeys.Num(), RotationalKeys.Num(), ScalingKeys.Num());
                while (PositionalKeys.Num() < ArrLen)
                {
                    if (PositionalKeys.IsEmpty())
                    {
                        bool IsAb=Anim->Header.AnimType == ESEAnimAnimationType::SEANIM_ABSOLUTE ;
                        PositionalKeys.Add(IsAb  ?  FVector3f::ZeroVector   :   FVector3f(BonePoseTransform.GetLocation()));
                    }
                    else
                    {
                        FVector3f LastItem = PositionalKeys.Last();
                        PositionalKeys.Add(LastItem);
                    }
                }
                while (RotationalKeys.Num() < ArrLen)
                {
                    if (RotationalKeys.IsEmpty())
                    {
                        RotationalKeys.Add(FQuat4f(BonePoseTransform.GetRotation()));
                    }
                    else
                    {
                        FQuat4f LastItem = RotationalKeys.Last();
                        RotationalKeys.Add(LastItem);
                    }
                }
                while (ScalingKeys.Num() < ArrLen)
                {
                    if (ScalingKeys.IsEmpty())
                    {
                        ScalingKeys.Add(FVector3f::OneVector);
                    }
                    else
                    {
                        FVector3f LastItem = ScalingKeys.Last();
                        ScalingKeys.Add(LastItem);
                    }
                }
                Controller.SetBoneTrackKeys(NewCurveName, PositionalKeys, RotationalKeys, ScalingKeys);
            }
        }
        
        
        Controller.NotifyPopulated();
        Controller.CloseBracket();
        AnimSequence->Modify(true);
        AnimSequence->PostEditChange();
        FAssetRegistryModule::AssetCreated(AnimSequence);
        bool bDirty = AnimSequence->MarkPackageDirty();
    }
    if (AnimSequence)
    {
        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        AssetEditorSubsystem->OpenEditorForAsset(AnimSequence);
    }
    return AnimSequence;
}




FMeshBoneInfo UC2AnimAssetFactory::GetBone(const FString& AnimBoneName)
{
    for (const auto& Bone : Bones)
    {
        if (Bone.Name.ToString() == AnimBoneName)
        {
            return Bone;
        }
    }
    return FMeshBoneInfo();
}

int UC2AnimAssetFactory::GetBoneIndex(const FString& AnimBoneName)
{
    for (int32 Index = 0; Index < Bones.Num(); ++Index)
    {
        if (Bones[Index].Name.ToString() == AnimBoneName)
        {
            return Index;
        }
    }
    return -69;
}
