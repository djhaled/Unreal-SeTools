#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "BinaryReader.h"
#include "Enums.h"
#include "Shared.h"
#include "Serialization/LargeMemoryReader.h"
#include "C2MapHeader.generated.h"

// A struct for C2Map's header
UCLASS(BlueprintType, config=EditorPerProjectUserSettings, AutoExpandCategories=(FTransform), HideCategories=Object)
class C2MASSET_API UC2MapHeader : public UObject
{
	GENERATED_BODY()
public:
	UC2MapHeader();

	char Magic[3];
	UPROPERTY()
	uint8 FileVersion;
	UPROPERTY()
	FString GameVersion;
	UPROPERTY()
	FString MapName;
	UPROPERTY()
	FString SkyBoxName;
	UPROPERTY()
	uint32 ObjectsCount;
	UPROPERTY()
	uint64 ObjectsOffset;
	UPROPERTY()
	uint32 InstancesCount;
	UPROPERTY()
	uint64 InstanceOffset;
	UPROPERTY()
	uint32 ImagesCount;
	UPROPERTY()
	uint64 ImageOffset;
	UPROPERTY()
	uint32 MaterialsCount;
	UPROPERTY()
	uint64 MaterialsOffset;
	UPROPERTY()
	uint32 LightsCount;
	UPROPERTY()
	uint64 LightsOffset;
	

	void ParseHeader(FLargeMemoryReader& Reader);
};
