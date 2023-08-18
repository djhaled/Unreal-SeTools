// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UserMeshOptions.generated.h"
/**
 *
 */
UENUM(BlueprintType)
enum class EMeshType : uint8
{
	/** Represents a Static Mesh option */
	StaticMesh UMETA(DisplayName = "Static Mesh"),
	/** Represents a Skeletal Mesh option */
    SkeletalMesh UMETA(DisplayName = "Skeletal Mesh")
};

UCLASS(config = Engine, defaultconfig, transient)
class C2MODEL_API UUserMeshOptions : public UObject
{
public:
	GENERATED_BODY()
	UUserMeshOptions();
	UPROPERTY(EditAnywhere, Category = "Import Settings", meta = (DisplayName = "Mesh Type"))
	EMeshType MeshType;
	
	UPROPERTY(EditAnywhere, Category = "Import Settings")
	bool bImportMaterials = true;

	UPROPERTY(EditAnywhere, Category = "Import Settings", meta = (EditCondition = "bImportMaterials == true", DisplayName = "Override Master Material", 
		Tooltip = "Override the current master material by providing a custom master material. If you want to use a custom material for this mesh, set your created master material here."))
	TSoftObjectPtr<UMaterial> OverrideMasterMaterial;

	
	UPROPERTY(EditAnywhere, Category = "Import Settings", meta = (EditCondition = "MeshType == EMeshType::SkeletalMesh"))
	TSoftObjectPtr<USkeleton> OverrideSkeleton;


	bool bInitialized;



};