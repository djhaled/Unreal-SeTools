#pragma once
#include "Structures/C2Material.h"
#include "Materials/MaterialInstanceConstant.h"

class C2MMaterialInstance
{
public:
	static UMaterialInterface* CreateMixMaterialInstance(TArray<C2Material*> CoDMaterials, UObject* ParentPackage);
	static void SetMaterialTextures(C2Material* CODMat, FStaticParameterSet& StaticParameters, UMaterialInstanceConstant*& MaterialAsset, bool IsMixMaterial, int MaterialIndex);
	static void SetMaterialConstants(C2Material* CODMat, UMaterialInstanceConstant*& MaterialAsset, bool IsMixMaterial, int MaterialIndex);
	static void SetBlendParameters(UMaterialInstanceConstant*& MaterialAsset, int32_t Index);
};