#include "C2MMaterialInstance.h"

#include "AnimationEditorUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"

UMaterialInterface* C2MMaterialInstance::CreateMixMaterialInstance(UC2MImportOptions* ImportOptions, TArray<C2Material*> CoDMaterials, FString& GameVersion)
{
	UMaterialInterface* UnrealMaterialFinal = nullptr;
	// Set full package path
	FString FullMaterialName = CoDMaterials[0]->Header->MaterialName;
	for (int i = 1; i < CoDMaterials.Num(); i++)
	{
		auto CoDMat = CoDMaterials[i];
		FullMaterialName += "__" + CoDMat->Header->MaterialName;
	}
	FString PathPackage = FPaths::Combine(TEXT("/Game"),
		TEXT("C2M"),
		GameVersion,
		TEXT("Materials_new"),
		*FullMaterialName);

	// Check if material already exists in browser
	// UMaterialInstanceConstant* ExistingMaterial = LoadObject<UMaterialInstanceConstant>(nullptr, *PathPackage);
	// if (IsValid(ExistingMaterial))
	// {
	// 	UnrealMaterialFinal = ExistingMaterial;
	// 	return UnrealMaterialFinal;
	// }
	// Create new material instance asset
	UPackage* Package = CreatePackage(*PathPackage);
	// Package->FullyLoad();
	// Package->Modify();
	auto MaterialInstanceFactory = NewObject<UMaterialInstanceConstantFactoryNew>();
	MaterialInstanceFactory->InitialParent = ImportOptions->uBaseMaterialLit;
	UMaterialInstanceConstant* MaterialInstance = Cast<UMaterialInstanceConstant>(MaterialInstanceFactory->FactoryCreateNew(UMaterialInstanceConstant::StaticClass(), Package, *FullMaterialName, RF_Standalone | RF_Public, NULL, GWarn));
	// Notify the asset registry
	FAssetRegistryModule::AssetCreated(MaterialInstance);
	// Set the dirty flag so this package will get saved later
	Package->SetDirtyFlag(true);

	// Get static parameters
	FStaticParameterSet StaticParameters;
	MaterialInstance->GetStaticParameterValues(StaticParameters);
	const bool IsMixMaterial = CoDMaterials.Num() > 1 ? true : false;
	// Set layers for mix material
	if (CoDMaterials.Num() > 1)
	{
		// Create layers for material instance
		SetLayerFunctions(MaterialInstance, CoDMaterials, ImportOptions, StaticParameters);
		// Get updated parameters (this is not smart, will change it)
		FStaticParameterSet StaticParameters2;
		MaterialInstance->GetStaticParameterValues(StaticParameters2);
		for (int i = 0; i < CoDMaterials.Num(); i++)
		{
			// Get current COD Material
			auto CODMat = CoDMaterials[i];
			// Set Texture Parameters
			SetMaterialTextures(CODMat, StaticParameters2, MaterialInstance, IsMixMaterial, i, GameVersion);
			SetMaterialConstants(CODMat, MaterialInstance, IsMixMaterial, i, GameVersion); // index -1 is used for global parameters
			// Set UV Channel for layer
			FMaterialParameterInfo Param_UVSet(TEXT("UVSet"), LayerParameter, i);
			MaterialInstance->SetScalarParameterValueEditorOnly(Param_UVSet, i);
			// Set Blend parameters
			if (i > 0)
				SetBlendParameters(MaterialInstance, i - 1); // First layer has no blend, so subtract by 1
		}
		MaterialInstance->UpdateStaticPermutation(StaticParameters2);
	}
	// Set single material
	else
	{
		const auto CODMat = CoDMaterials[0];
		SetMaterialTextures(CODMat, StaticParameters, MaterialInstance, IsMixMaterial, -1, GameVersion); // index -1 is used for global parameters
		SetMaterialConstants(CODMat, MaterialInstance, IsMixMaterial, -1, GameVersion); // index -1 is used for global parameters
		MaterialInstance->UpdateStaticPermutation(StaticParameters);
	}

	// let the material update itself if necessary
	UnrealMaterialFinal = MaterialInstance;
	UnrealMaterialFinal->PreEditChange(NULL);
	UnrealMaterialFinal->PostEditChange();

	return UnrealMaterialFinal;
}

void C2MMaterialInstance::SetLayerFunctions(UMaterialInstanceConstant* MaterialAsset,
	TArray<C2Material*> CoDMaterials,
	UC2MImportOptions* ImportOptions,
	FStaticParameterSet& StaticParameters)
{
	// Get static parameters
	MaterialAsset->GetStaticParameterValues(StaticParameters);
	for (auto& Param : StaticParameters.StaticSwitchParameters)
		if (Param.ParameterInfo.Name == "Use Layers")
		{
			Param.bOverride = true;
			Param.Value = true;
		}
	// Get Layer node parameter
	auto LayerParameter = StaticParameters.MaterialLayersParameters[0];
	// Create & Set Background layer
	auto layerFunctions = FMaterialLayersFunctions();
	// Add layers
	for (int i = 0; i < CoDMaterials.Num(); i++)
	{
		auto& CODMaterial = CoDMaterials[i];
		if (i > 0)
		{
			layerFunctions.AppendBlendedLayer();
			switch (CODMaterial->Header->Blending)
			{
			case BLEND_VERTEX_SIMPLE:
				layerFunctions.Blends[i - 1] = ImportOptions->uBaseVertexBlend;
				break;
			case BLEND_VERTEX_COMPLEX:
				layerFunctions.Blends[i - 1] = ImportOptions->uBaseVertexRevealBlend;
				break;
			case BLEND_MULTIPLY:
				layerFunctions.Blends[i - 1] = ImportOptions->uBaseMultiplyBlend;
				break;
			}

		}
		layerFunctions.Layers[i] = ImportOptions->uBaseMaterialLayer;
		layerFunctions.LayerNames[i] = FText::FromString(CODMaterial->Header->MaterialName);
	}
	StaticParameters.MaterialLayersParameters[0].Value = layerFunctions;
	// Update parameters
	MaterialAsset->UpdateStaticPermutation(StaticParameters);
}

void C2MMaterialInstance::SetMaterialTextures(
	C2Material* CODMat,
	FStaticParameterSet& StaticParameters,
	UMaterialInstanceConstant*& MaterialAsset,
	bool IsMixMaterial,
	int32_t MaterialIndex,
	FString GameVersion)
{
	for (auto Texture : CODMat->Textures)
	{
		FString TexturePath = FPaths::Combine(TEXT("/Game"),
			TEXT("C2M"),
			GameVersion,
			TEXT("Textures"),
			*C2MTexture::NoIllegalSigns(Texture.TextureName));
		UTexture* TextureAsset = LoadObject<UTexture>(nullptr, *TexturePath);
		FString& TextureType = Texture.TextureType;
		if (TextureAsset)
		{
			// Check if colorMap's alpha is gloss
			if (Texture.TextureType == "colorGloss")
			{
				// Enable ColorGloss in material & change opacity to float constant
				for (auto& SwitchParam : StaticParameters.StaticSwitchParameters)
				{
					if (SwitchParam.ParameterInfo.Index == MaterialIndex && (SwitchParam.ParameterInfo.Name == "ColorGlossPacked" || SwitchParam.ParameterInfo.Name == "Override Texture Opacity"))
					{
						SwitchParam.bOverride = true;
						SwitchParam.Value = true;
					}
				}

				TextureType = "colorMap";
			}
			// Check if colorMap's alpha is opacity
			else if (Texture.TextureType == "colorOpacity")
			{
				// Check if single material (there's no need to do this for layers)
				if (MaterialIndex == -1)
				{
					// Override material blending mode
					MaterialAsset->BasePropertyOverrides.bOverride_BlendMode = true;
					MaterialAsset->BasePropertyOverrides.BlendMode = CODMat->Header->Blending == BLEND_TRANSLUCENT ? BLEND_Translucent : BLEND_Masked;
					MaterialAsset->UpdateOverridableBaseProperties();
				}

				TextureType = "colorMap";
			}
			// Get the right texture parameter
			FMaterialParameterInfo TextureParameterInfo(*TextureType, IsMixMaterial ? LayerParameter : GlobalParameter, MaterialIndex);
			// Set texture
			MaterialAsset->SetTextureParameterValueEditorOnly(TextureParameterInfo, TextureAsset);
		}
	}
}

void C2MMaterialInstance::SetMaterialConstants(
	C2Material* CODMat,
	UMaterialInstanceConstant*& MaterialAsset,
	bool IsMixMaterial,
	int32_t MaterialIndex,
	FString GameVersion)
{
	for (auto Constant : CODMat->Constants)
	{
		auto ConstantType = ConstantTypes[Constant.Hash];
		EMaterialParameterAssociation ConstantAssociation = IsMixMaterial ? LayerParameter : GlobalParameter;
		switch (ConstantType)
		{
		case alphaRevealParams:
			ConstantAssociation = BlendParameter;
			FMaterialParameterInfo alphaRevealSoftEdgeParameterInfo("alphaRevealSoftEdge", ConstantAssociation, ConstantAssociation != BlendParameter ? MaterialIndex : MaterialIndex - 1); // if it's blend parameter, we subtract
			FMaterialParameterInfo alphaRevealRampParameterInfo("alphaRevealRamp", ConstantAssociation, ConstantAssociation != BlendParameter ? MaterialIndex : MaterialIndex - 1);
			MaterialAsset->SetScalarParameterValueEditorOnly(alphaRevealSoftEdgeParameterInfo, Constant.Value.X);
			MaterialAsset->SetScalarParameterValueEditorOnly(alphaRevealRampParameterInfo, Constant.Value.Y);
			break;
		}
	}
}
void C2MMaterialInstance::SetBlendParameters(UMaterialInstanceConstant*& MaterialAsset, int32_t Index)
{
	// Get the right vertex color channel for current layer blending
	FLinearColor VertexChannel = FLinearColor(0.0, 0.0, 0.0, 0.0);
	switch (Index)
	{
	default:
	case 0:
		VertexChannel.G = 1.0;
		break;
	case 1:
		VertexChannel.B = 1.0;
		break;
	case 2:
		VertexChannel.A = 1.0;
		break;
	}
	// Set Vertex Color Channel for layer
	FMaterialParameterInfo Param_VertexChannel(TEXT("Vertex Color Channel"), BlendParameter, Index);
	MaterialAsset->SetVectorParameterValueEditorOnly(Param_VertexChannel, VertexChannel);
}