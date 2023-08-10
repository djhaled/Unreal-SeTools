#pragma once
#include "Utils/BinaryReader.h"
#include "Serialization/LargeMemoryReader.h"

class C2MODEL_API C2MTexture
{
public:
	C2MTexture();
	UObject* TextureObject;
	FString TextureName;
	FString TexturePath;
	FString TextureType;

	void ParseTexture(FLargeMemoryReader& Reader);
	static FString NoIllegalSigns(const FString& InString);
};
