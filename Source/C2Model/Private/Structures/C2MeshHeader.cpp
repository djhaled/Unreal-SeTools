#include "Structures/C2MeshHeader.h"

#include "Utils/BinaryReader.h"

C2MeshHeader::C2MeshHeader()
{
	DataPresentFlags = SEModelDataPresenceFlags::SEMODEL_PRESENCE_MESH;
	GameName = "";
	MeshName = "";
	BoneCountBuffer = 0;
	SurfaceCount = 1;
	MaterialCountBuffer = 0;
}
C2MeshHeader::C2MeshHeader(FString InName)
{
	DataPresentFlags = SEModelDataPresenceFlags::SEMODEL_PRESENCE_MESH;
	GameName = "";
	MeshName = InName;
	BoneCountBuffer = 0;
	SurfaceCount = 1;
	MaterialCountBuffer = 0;
}
void C2MeshHeader::ParseHeader(FLargeMemoryReader& Reader)
{
	Reader.ByteOrderSerialize(&Magic, sizeof(Magic));
	uint16_t Version;
	uint16_t HeaderSize;
	Reader << Version;
	Reader << HeaderSize;
	Reader << DataPresentFlags;
	uint8_t DataPresent2;
	uint8_t DataPresent3;
	Reader << DataPresent2;
	Reader << DataPresent3;
	Reader << BoneCountBuffer;
	Reader << SurfaceCount;
	Reader << MaterialCountBuffer;
	uint8_t ReservedBytes[3];
	Reader.ByteOrderSerialize(&ReservedBytes, sizeof(ReservedBytes));
}