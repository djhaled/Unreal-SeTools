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
	// This is a property that determines whether to automatically decide the mesh type.
	// If set to true, the mesh type will be automatically decided.
	// If set to false, you can manually choose the mesh type using the 'MeshType' property.
	// If enabled, the mesh type will be determined automatically based on the bone count.
	// If the bone count is greater than 0, the mesh will be imported as a Skeletal Mesh.
	// If the bone count is 0, the mesh will be imported as a Static Mesh.
	// This property determines whether the mesh type should be automatically decided based on bone count.
	// If enabled, the mesh type will be determined automatically as Skeletal Mesh if bones are present, or Static Mesh if not.
	UPROPERTY(EditAnywhere, Category = "Mesh Settings", meta = (DisplayName = "Automatically Decide Mesh Type", Tooltip = "If enabled, the mesh type will be determined automatically based on bone count."))
	bool bAutomaticallyDecideMeshType;

	// This property defines the type of mesh to import.
	// It is only editable if 'bAutomaticallyDecideMeshType' is set to false.
	UPROPERTY(EditAnywhere, Category = "Mesh Settings", meta = (DisplayName = "Mesh Type", EditCondition = "!bAutomaticallyDecideMeshType"))
	EMeshType MeshType;

	// Determines whether materials should be imported along with the mesh.
	UPROPERTY(EditAnywhere, Category = "Material Settings", meta = (DisplayName = "Import Materials"))
	bool bImportMaterials = true;

	// Override the current master material by providing a custom master material.
	// Set your created master material here if you want to use a custom material for this mesh.
	UPROPERTY(EditAnywhere, Category = "Material Settings", meta = (EditCondition = "bImportMaterials == true", DisplayName = "Override Master Material", 
		Tooltip = "Override the current master material by providing a custom master material. If you want to use a custom material for this mesh, set your created master material here."))
	TSoftObjectPtr<UMaterial> OverrideMasterMaterial;

	// Override the skeleton for this Skeletal Mesh.
	UPROPERTY(EditAnywhere, Category = "Skeletal Mesh Settings", meta = (EditCondition = "MeshType == EMeshType::SkeletalMesh"))
	TSoftObjectPtr<USkeleton> OverrideSkeleton;


	bool bInitialized;



};