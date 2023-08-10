#include "Structures/C2MapHeader.h"
#include <map>

std::map<uint8_t,FString> GameVersions =
{
	{ 0,  "modern_warfare" },
	{ 1,  "world_at_war" },
	{ 2,  "modern_warfare_2" },
	{ 3,  "black_ops1" },
	{ 4,  "modern_warfare_3" },
	{ 5,  "black_ops_2" },
	{ 6,  "ghosts" },
	{ 7,  "advanced_warfare" },
	{ 8,  "black_ops_3" },
	{ 9,  "modern_warfare_rm" },
	{ 10, "infinite_warfare" },
	{ 11, "world_war_2" },
	{ 12, "black_ops_4" },
	{ 13, "modern_warfare_4" },
	{ 14, "modern_warfare_2_rm" },
	{ 15, "black_ops_5" },
	{ 16, "vanguard" },
};

// Default constructor
UC2MapHeader::UC2MapHeader()
{
	FileVersion = 0;
	GameVersion = "modern_warfare";
	MapName = "";
	SkyBoxName = ";";
	ObjectsCount = 0;
	ObjectsOffset = 0;
	InstancesCount = 0;
	InstanceOffset = 0;
	ImagesCount = 0;
	ImageOffset = 0;
	MaterialsCount = 0;
	MaterialsOffset = 0;
	LightsCount = 0;
	LightsOffset = 0;
}

void UC2MapHeader::ParseHeader(FLargeMemoryReader& Reader)
{
	Reader.ByteOrderSerialize(&Magic, sizeof(Magic));
	Reader << FileVersion;
	uint8_t GameID;
	Reader << GameID;
	GameVersion = GameVersions[GameID];
	BinaryReader::readString(Reader, &MapName);
	BinaryReader::readString(Reader, &SkyBoxName);
	Reader << ObjectsCount;
	Reader << ObjectsOffset;
	Reader << InstancesCount;
	Reader << InstanceOffset;
	Reader << ImagesCount;
	Reader << ImageOffset;
	Reader << MaterialsCount;
	Reader << MaterialsOffset;
	Reader << LightsCount;
	Reader << LightsOffset;
}
