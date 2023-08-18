#pragma once
#include "Serialization/LargeMemoryReader.h"

class C2MODEL_API C2MeshHeader
{
	enum class SEModelDataPresenceFlags : uint8_t
	{
		// Whether or not this model contains a bone block
		SEMODEL_PRESENCE_BONE = 1 << 0,
		// Whether or not this model contains submesh blocks
		SEMODEL_PRESENCE_MESH = 1 << 1,
		// Whether or not this model contains inline material blocks
		SEMODEL_PRESENCE_MATERIALS = 1 << 2,

		// The file contains a custom data block
		SEMODEL_PRESENCE_CUSTOM = 1 << 7,
	};
public:
	C2MeshHeader();
	char Magic[7];
	SEModelDataPresenceFlags DataPresentFlags;
	FString GameName;
	FString MeshName;
	uint32_t BoneCountBuffer;
	uint32_t SurfaceCount;
	uint32_t MaterialCountBuffer;

	C2MeshHeader(FString InName);
	void ParseHeader(FLargeMemoryReader& Reader);
};
