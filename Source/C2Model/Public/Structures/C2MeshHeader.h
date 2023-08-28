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
	enum class SEModelBonePresenceFlags : uint8_t
	{
		// Whether or not bones contain global-space matricies
		SEMODEL_PRESENCE_GLOBAL_MATRIX = 1 << 0,
		// Whether or not bones contain local-space matricies
		SEMODEL_PRESENCE_LOCAL_MATRIX = 1 << 1,

		// Whether or not bones contain scales
		SEMODEL_PRESENCE_SCALES = 1 << 2,
	};

	enum class SEModelMeshPresenceFlags : uint8_t
	{
		// Whether or not meshes contain at least 1 uv map
		SEMODEL_PRESENCE_UVSET = 1 << 0,

		// Whether or not meshes contain vertex normals
		SEMODEL_PRESENCE_NORMALS = 1 << 1,

		// Whether or not meshes contain vertex colors (RGBA)
		SEMODEL_PRESENCE_COLOR = 1 << 2,

		// Whether or not meshes contain at least 1 weighted skin
		SEMODEL_PRESENCE_WEIGHTS = 1 << 3,
	};

public:
	C2MeshHeader();
	char Magic[7];
	SEModelDataPresenceFlags DataPresentFlags;
	SEModelBonePresenceFlags BonePresentFlags;
	SEModelMeshPresenceFlags MeshPresentFlags;
	FString GameName;
	FString MeshName;
	uint32_t BoneCountBuffer;
	uint32_t SurfaceCount;
	uint32_t MaterialCountBuffer;

	C2MeshHeader(FString InName);
	void ParseHeader(FLargeMemoryReader& Reader);
};
