#include "UserMeshOptions.h"

UUserMeshOptions::UUserMeshOptions()
{
	MeshType = EMeshType::StaticMesh;
	bImportMaterials = true;
	OverrideMasterMaterial = nullptr;
	OverrideSkeleton = nullptr;
}
