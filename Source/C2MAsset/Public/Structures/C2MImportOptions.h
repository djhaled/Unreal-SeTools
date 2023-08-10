#pragma once

#include "CoreMinimal.h"
#include "C2MapHeader.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
//#include "Factories/ImportSettings.h"
#include "Materials/MaterialFunctionMaterialLayer.h"
#include "Materials/MaterialFunctionMaterialLayerBlend.h"
#include "C2MImportOptions.generated.h"


UCLASS(BlueprintType, config=EditorPerProjectUserSettings, AutoExpandCategories=(FTransform), HideCategories=Object)
class C2MASSET_API UC2MImportOptions : public UObject
{
	GENERATED_BODY()
public:
	UC2MImportOptions(){}
	UPROPERTY(EditAnywhere, Category = "IMPORT",	DisplayName = "Import Models")
	bool bImportModels = true;
	UPROPERTY(EditAnywhere, Category = "IMPORT",	DisplayName = "Import Materials")
	bool bImportMaterials = true;
	// UPROPERTY(EditAnywhere, Category = "IMPORT",	DisplayName = "Import Textures")
	// bool bImportTextures = true;
	// UPROPERTY(EditAnywhere, Category = "Models",	DisplayName = "Nanite",						meta = (EditCondition = "bImportModels",					    EditConditionHides, Description = "Apply Nanite to imported mesh"))
	// bool bModelNanite = false;
	// UPROPERTY(EditAnywhere, Category = "Models",	DisplayName = "Animations",					meta = (EditCondition = "bImportModels",					    EditConditionHides, Description = "Import model animations"))
	// bool bImportModelAnimations = false;
	UPROPERTY(EditAnywhere, Category = "Materials", DisplayName = "Base Material (Lit)",		meta = (EditCondition = "bImportMaterials",					    EditConditionHides, Description = "Master Material for Lit objects"))
	UMaterial* uBaseMaterialLit;
	// UPROPERTY(EditAnywhere, Category = "Materials", DisplayName = "Base Material (Emissive)",	meta = (EditCondition = "bImportMaterials",					    EditConditionHides, Description = "Master Material for Emissive objects"))
	// UMaterial* uBaseMaterialEmissive;
	// UPROPERTY(EditAnywhere, Category = "Materials", DisplayName = "Base Material (Glass)",		meta = (EditCondition = "bImportMaterials",					    EditConditionHides, Description = "Master Material for Glass objects"))
	// UMaterial* uBaseMaterialGlass;
	// UPROPERTY(EditAnywhere, Category = "Materials", DisplayName = "Base Material (Blend)",		meta = (EditCondition = "bImportMaterials && !bHasMixMaterial", EditConditionHides, Description = "Master Material for Blend objects"))
	// UMaterial* uBaseMaterialBlend;
	
	/* Mix Material properties
	 * ONLY USED IN GAMES THAT WORK WITH MIX MATERIALS/LAYERS */
	UPROPERTY(EditAnywhere, Category = "Materials", DisplayName = "Base Layer",					meta = (EditCondition = "bImportMaterials && bHasMixMaterial",  EditConditionHides, Description = "Master Layer for Lit objects"))
	UMaterialFunctionMaterialLayer* uBaseMaterialLayer;
	UPROPERTY(EditAnywhere, Category = "Materials", DisplayName = "Vertex Reveal Blend",		meta = (EditCondition = "bImportMaterials && bHasMixMaterial && bHasComplexBlend",
																										EditConditionHides,
																										Description = "Complex Vertex Blend (Game specific)"))
	UMaterialFunctionMaterialLayerBlend* uBaseVertexRevealBlend;
	UPROPERTY(EditAnywhere, Category = "Materials", DisplayName = "Vertex Blend",				meta = (EditCondition = "bImportMaterials && bHasMixMaterial",  EditConditionHides, Description = "Simple Vertex Blend"))
	UMaterialFunctionMaterialLayerBlend* uBaseVertexBlend;
	UPROPERTY(EditAnywhere, Category = "Materials", DisplayName = "Multiply Blend",				meta = (EditCondition = "bImportMaterials && bHasMixMaterial",  EditConditionHides, Description = "Decal Multiply Blend"))
	UMaterialFunctionMaterialLayerBlend* uBaseMultiplyBlend;
	
	bool bShouldImport = false;
	UPROPERTY()
	bool bHasMixMaterial = true;
	UPROPERTY()
	bool bHasComplexBlend = true;

	void ParseGameSpecificOptions(UC2MapHeader* MapHeader);
private:
};
