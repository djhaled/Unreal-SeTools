// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Internationalization/Text.h"
#include "UObject/Object.h"
#include "C2MapHeader.h"
#include "C2Mesh.h"
#include "C2MImportOptions.h"
#include "C2MCreateOptions.h"
#include "C2MAsset.generated.h"

UCLASS(BlueprintType, hidecategories=(Object))
class C2MASSET_API UC2MAsset
	: public UObject
{
	GENERATED_BODY()

	public:

	/** Holds the stored text. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="C2MAsset")
	FString Path;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="C2MAsset")
	UC2MapHeader* Header;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="C2MAsset")
	UC2MImportOptions* ImportOptions;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="C2MAsset")
	UC2MCreateOptions* CreateOptions;
};