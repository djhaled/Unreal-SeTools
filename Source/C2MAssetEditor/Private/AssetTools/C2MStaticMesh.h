#pragma once
#include "C2Material.h"
#include "MeshDescription.h"
#include "RawMesh.h"
#include "C2Mesh.h"
#include "C2MImportOptions.h"

class C2MStaticMesh
{
public:
	
	static FMeshDescription CreateMeshDescription(C2Mesh* InMesh);
	static FRawMesh CreateRawMesh(C2Mesh* InMesh);
	static void CreateStaticMeshFromMeshDescription(UC2MapHeader* MapInfo, FMeshDescription& inMeshDescription, C2Mesh* InMesh, UC2MImportOptions* ImportOptions, TArray<C2Material*> CoDMaterials);
	static void CreateStaticMesh(UC2MapHeader* MapInfo, bool bSplitMesh, C2Mesh* InMesh, UC2MImportOptions* ImportOptions, TArray<C2Material*> CoDMaterials);
};

