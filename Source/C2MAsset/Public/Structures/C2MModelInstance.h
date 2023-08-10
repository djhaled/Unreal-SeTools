#pragma once
#include "Shared.h"
#include "BinaryReader.h"
#include "Serialization/LargeMemoryReader.h"

class C2MASSET_API C2MModelInstance
{
public:
	C2MModelInstance();
	C2MModelInstance(FString InName);
	FString Name;
	TArray<FTransform> Transform;

	void ParseModelInstance(FLargeMemoryReader& Reader);
};

