#pragma once
#include "BinaryReader.h"
#include "Serialization/LargeMemoryReader.h"

class C2MASSET_API C2MTexture
{
public:
	C2MTexture();
	FString TextureName;
	FString TextureType;
	
	void ParseTexture(FLargeMemoryReader& Reader);
	static FString NoIllegalSigns(const FString& InString);
};
