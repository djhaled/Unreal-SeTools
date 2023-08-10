#pragma once

#include "Shared.h"
#include "BinaryReader.h"
#include "Serialization/LargeMemoryReader.h"

class C2MASSET_API C2MSurface

{
public:
	C2MSurface();
	C2MSurface(C2MSurface* InSurface);
	C2MSurface(FString InName);
	FString Name;
	TArray<GfxFace> Faces;
	TArray<uint16_t> Materials;
	uint8_t UVCount;
	
	void ParseSurface(FLargeMemoryReader& Reader);
};

