#pragma once
#include "Structures/C2Material.h"
#include "MeshDescription.h"
#include "Structures/C2Mesh.h"


class C2MStaticMesh
{
public:
	
	static FMeshDescription CreateMeshDescription(C2Mesh* InMesh);
	static UObject* CreateStaticMeshFromMeshDescription(UObject* ParentPackage,FMeshDescription& inMeshDescription, C2Mesh* InMesh,  TArray<C2Material*> CoDMaterials);
	static UObject* CreateStaticMesh(UObject* ParentPackage, C2Mesh* InMesh,  TArray<C2Material*> CoDMaterials);
	static UObject* CreateSkeletalMesh( UObject* ParentPackage, C2Mesh* InMesh,  TArray<C2Material*> CoDMaterials);
	static void ProcessSkeleton(const FSkeletalMeshImportData& ImportData, const USkeleton* Skeleton, FReferenceSkeleton& OutRefSkeleton, int& OutSkeletalDepth);
	static UObject* CreateSkeletalMeshFromMeshDescription( UObject* ParentPackage,FMeshDescription& InMeshDescription, C2Mesh* InMesh,  TArray<C2Material*> CoDMaterials);

};

