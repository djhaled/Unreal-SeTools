#pragma once

#include "BinaryReader.h"
#include "Enums.h"
#include "Serialization/LargeMemoryReader.h"

class C2MASSET_API C2MaterialHeader
{
public:
	C2MaterialHeader();
	~C2MaterialHeader();

	FString MaterialName;
	BlendingMode Blending;
	uint8_t SortKey;
	uint8_t TextureCount;
	uint8_t ConstantCount;
	
	void ParseHeader(FLargeMemoryReader& Reader);
};
