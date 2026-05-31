// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SAnimOptions.generated.h"
/**
 *
 */
UCLASS(config = Engine, defaultconfig, transient)
class C2MODEL_API USAnimOptions : public UObject
{
public:
	GENERATED_BODY()
	USAnimOptions();
	UPROPERTY(EditAnywhere, Category = "Import Settings")
	TSoftObjectPtr<USkeleton> Skeleton;

	UPROPERTY(EditAnywhere, Category = "Anim Settings", meta = (ToolTip = "Override the animation type if enabled"))
	bool bOverrideAnimType;
	
	UPROPERTY(EditAnywhere, Category = "Anim Settings", meta = (ToolTip = "Specify the type of animation if override is enabled", EditCondition = "bOverrideAnimType"))
	ESEAnimAnimationType AnimType;

	UPROPERTY(EditAnywhere, Category = "Anim Settings", meta = (ToolTip = "Use Cure To Save Animtion"))
	bool bUseCurveeSave=false;

	
	UPROPERTY(EditAnywhere, Category = "Anim Settings", meta = (ToolTip = "Override the refpose if enabled"))
	bool bOverrideRefpose;
	
	UPROPERTY(EditAnywhere, Category = "Anim Settings", meta = (ToolTip = "Refpose Sequence",EditCondition = "bOverrideRefpose"))
	TObjectPtr<UAnimSequence>  RefposeSequence;
	
	UPROPERTY(EditAnywhere, Category = "Anim Settings", meta = (ToolTip = "Refpose time",EditCondition = "bOverrideRefpose"))
	int PoseTime=0;

	bool bInitialized;



};
