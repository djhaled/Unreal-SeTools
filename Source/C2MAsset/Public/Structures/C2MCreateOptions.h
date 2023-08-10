#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Materials/MaterialFunctionMaterialLayer.h"
#include "Materials/MaterialFunctionMaterialLayerBlend.h"
#include "C2MCreateOptions.generated.h"

// Struct for import options when importing a C2M file
UCLASS(BlueprintType, config=EditorPerProjectUserSettings, AutoExpandCategories=(FTransform), HideCategories=Object)
class C2MASSET_API UC2MCreateOptions : public UObject
{
	GENERATED_BODY()
public:
	UC2MCreateOptions(){}
	// UPROPERTY(EditAnywhere, Category = "Create",	DisplayName = "Create Materials")
	// bool bCreateMaterials;
	UPROPERTY(EditAnywhere, Category = "Create",	DisplayName = "Create Instances")
	bool bCreateInstances;
	UPROPERTY(EditAnywhere, Category = "Create",	DisplayName = "Create Lights")
	bool bCreateLights;
	// UPROPERTY(EditAnywhere, Category = "Materials", DisplayName = "Base Layer",				meta = (EditCondition = "bCreateMaterials", EditConditionHides))
	// UMaterialFunctionMaterialLayer* uBaseMaterialLayer;
	// UPROPERTY(EditAnywhere, Category = "Materials", DisplayName = "Vertex Reveal Blend",	meta = (EditCondition = "bCreateMaterials", EditConditionHides))
	// UMaterialFunctionMaterialLayerBlend* uBaseVertexRevealBlend;
	// UPROPERTY(EditAnywhere, Category = "Materials", DisplayName = "Vertex Blend",			meta = (EditCondition = "bCreateMaterials", EditConditionHides))
	// UMaterialFunctionMaterialLayerBlend* uBaseVertexBlend;
	// UPROPERTY(EditAnywhere, Category = "Materials", DisplayName = "Multiply Blend",			meta = (EditCondition = "bCreateMaterials", EditConditionHides))
	// UMaterialFunctionMaterialLayerBlend* uBaseMultiplyBlend;

	// Transient Map Info (We got this info stored in C2MAsset Header, so no point in saving it here aswel)
	UPROPERTY(VisibleAnywhere, Transient, Category = "Map Information",		DisplayName = "File")
	FString MapPath;
	UPROPERTY(VisibleAnywhere, Transient, Category = "Map Information",		DisplayName = "Game")
	FString GameVersion;
	UPROPERTY(VisibleAnywhere, Transient, Category = "Map Information",		DisplayName = "Map")
	FString MapName;
	UPROPERTY(VisibleAnywhere, Transient, Category = "Map Information",		DisplayName = "Materials")
	FString MaterialCount;
	UPROPERTY(VisibleAnywhere, Transient, Category = "Map Information",		DisplayName = "Models")
	FString ModelCount;
	UPROPERTY(VisibleAnywhere, Transient, Category = "Map Information",		DisplayName = "Instances")
	FString InstanceCount;
	UPROPERTY(VisibleAnywhere, Transient, Category = "Map Information",		DisplayName = "Lights")
	FString LightCount;

private:
};
