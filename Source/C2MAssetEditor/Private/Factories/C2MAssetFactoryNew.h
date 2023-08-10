// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
// Based on TextAsset example - https://github.com/ue4plugins/TextAsset
#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"
#include "C2MAssetFactoryNew.generated.h"


/**
* Implements a factory for UC2MAsset objects.
*/
UCLASS(hidecategories=Object)
class UC2MAssetFactoryNew
	: public UFactory
{
	GENERATED_UCLASS_BODY()

public:

	//~ UFactory Interface

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};