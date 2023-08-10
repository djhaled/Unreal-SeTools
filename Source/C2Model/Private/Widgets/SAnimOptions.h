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

	bool bInitialized;



};