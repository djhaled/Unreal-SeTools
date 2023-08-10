// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
// Based on TextAsset example - https://github.com/ue4plugins/TextAsset
#include "C2MAssetFactoryNew.h"
#include "C2MAsset.h"


/* UC2MAssetFactoryNew structors
*****************************************************************************/

UC2MAssetFactoryNew::UC2MAssetFactoryNew(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UC2MAsset::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}


/* UFactory overrides
*****************************************************************************/

UObject* UC2MAssetFactoryNew::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UC2MAsset>(InParent, InClass, InName, Flags);
}


bool UC2MAssetFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}