// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"
#include "C2ModelAssetFactory.generated.h"


/**
 * Implements a factory for UC2MAsset objects.
 */

class UUserMeshOptions;
UCLASS(hidecategories=Object)
class UC2ModelAssetFactory
	: public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY()
	UUserMeshOptions* UserSettings;
	bool bImport;
	bool bImportAll;
	//~ UFactory Interface
	FString BaseDestPath = "/Game/C2M/";
//	virtual UObject* FactoryCreateBinary(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn) override;
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	static UObject* ImportTexture(FString FilePath, UObject* InParent);
};
