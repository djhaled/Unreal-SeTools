#pragma once
#include "C2Material.h"
#include "C2MImportOptions.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Enums.h"

class C2MMaterialInstance
{
public:
	static UMaterialInterface* CreateMixMaterialInstance(UC2MImportOptions* ImportOptions, TArray<C2Material*> CoDMaterials, FString& GameVersion);
	static void SetLayerFunctions(UMaterialInstanceConstant* Material, TArray<C2Material*> CoDMaterials, UC2MImportOptions* ImportOptions, FStaticParameterSet& StaticParameters);
	static void SetMaterialTextures(C2Material* CODMat, FStaticParameterSet& StaticParameters, UMaterialInstanceConstant*& MaterialAsset, bool IsMixMaterial, int MaterialIndex, FString GameVersion);
	static void SetMaterialConstants(C2Material* CODMat, UMaterialInstanceConstant*& MaterialAsset, bool IsMixMaterial, int MaterialIndex, FString GameVersion);
	static void SetBlendParameters(UMaterialInstanceConstant*& MaterialAsset, int32_t Index);
};
