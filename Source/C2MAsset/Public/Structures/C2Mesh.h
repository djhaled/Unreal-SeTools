#pragma once
#include <iostream>
#include <map>
#include "Shared.h"
#include "C2MSurface.h"
#include "BinaryReader.h"
//#include "RawMesh.h"
#include "Engine/StaticMesh.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "C2MapHeader.h"
#include "C2MeshHeader.h"
#include "Misc/ScopedSlowTask.h"
#include "StaticMeshAttributes.h"

class C2MASSET_API C2Mesh
{
public:
	C2Mesh(){};
	C2MeshHeader* Header;
	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<GfxColor> Colors;
	TArray<TArray<FVector2D>> UVs;
	TArray<C2MSurface*> Surfaces;
	bool bIsXModel = false;
	uint8_t MaterialCount = 1;
	uint8_t UVSetCount = 1;
	
	void ParseMesh(FLargeMemoryReader& Reader);
};
