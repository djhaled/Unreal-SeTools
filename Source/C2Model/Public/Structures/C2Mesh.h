#pragma once
#include <iostream>
#include <map>
#include "Shared.h"
#include "C2MSurface.h"
#include "Utils/BinaryReader.h"
//#include "RawMesh.h"
#include "Engine/StaticMesh.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "C2MeshHeader.h"
#include "Misc/ScopedSlowTask.h"
#include "StaticMeshAttributes.h"

struct C2ModelMaterial
{
	FString MaterialName;
	TArray<FString> TextureNames;
	TArray<FString> TextureTypes;

};
struct C2Bone
{
	FString Name;
	uint8  non;
	uint32_t ParentIndex;
	FVector3f  GlobalPosition;
	FRotator3f GlobalRotation;
	FVector3f  LocalPosition;
	FRotator3f LocalRotation;


};

class C2MODEL_API C2Mesh
{
public:
	C2Mesh(){};
	C2MeshHeader* Header;
	TArray<C2Bone> Bones;
	TArray<C2MSurface*> Surfaces;  
	TArray<C2ModelMaterial> Materials;
	bool bIsXModel = false;
	bool bIsSkeletal = false;
	TArray<FString> TTypes = { "colorMap", "normalMap", "specularMap" };
	int surf_vertCounter = 0;
	uint8_t MaterialCount = 1;
	uint8_t UVSetCount = 1;
	
	void ParseMesh(FLargeMemoryReader& Reader);
};
