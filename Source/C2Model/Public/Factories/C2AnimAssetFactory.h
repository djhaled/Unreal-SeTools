// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"
#include "Widgets/SAnimOptions.h"
#include "C2AnimAssetFactory.generated.h"



#define LOCTEXT_NAMESPACE "SeAnimPlugin"
/**
 * Implements a factory for UC2MAsset objects.
 */
UCLASS(hidecategories=Object)
class UC2AnimAssetFactory
	: public UFactory
{
	GENERATED_UCLASS_BODY()

public:

	//~ UFactory Interface
	FString BaseDestPath = "/Game/C2M/";
	UPROPERTY()
		USAnimOptions* SettingsImporter;

	bool bImport;
	bool bImportAll;
	bool hasBone(TArray<FMeshBoneInfo> Bones, const FString& AnimBoneName);
//	virtual UObject* FactoryCreateBinary(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn) override;
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
};
