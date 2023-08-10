#pragma once
#include "Serialization/LargeMemoryReader.h"

class C2MASSET_API C2MeshHeader
{
public:
	C2MeshHeader();
	
	FString MeshName;
	uint32_t VertexCount;
	uint32_t SurfaceCount;
	uint32_t FaceCount;

	C2MeshHeader(FString InName);
	void ParseHeader(FLargeMemoryReader& Reader);
};
