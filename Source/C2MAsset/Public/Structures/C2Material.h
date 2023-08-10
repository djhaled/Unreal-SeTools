#pragma once
#include "BinaryReader.h"
#include "C2MaterialHeader.h"
#include "C2MConstant.h"
#include "C2MTexture.h"
#include "Enums.h"
#include "Serialization/LargeMemoryReader.h"

class C2MASSET_API C2Material
{
public:
	C2Material();
	
	C2MaterialHeader* Header;
	TArray<C2MTexture> Textures;
	TArray<C2MConstant> Constants;
	
	void ParseMaterial(FLargeMemoryReader& Reader);
};
