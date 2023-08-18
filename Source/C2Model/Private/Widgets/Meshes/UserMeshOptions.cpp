#include "UserMeshOptions.h"

UUserMeshOptions::UUserMeshOptions()
{
	bAutomaticallyDecideMeshType = true;
	MeshType = EMeshType::StaticMesh;
	bImportMaterials = true;
	OverrideMasterMaterial = nullptr;
	OverrideSkeleton = nullptr;
}
