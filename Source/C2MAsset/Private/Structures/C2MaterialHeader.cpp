#include "C2MaterialHeader.h"

C2MaterialHeader::C2MaterialHeader()
{
	MaterialName = "";
	Blending = BLEND_VERTEX_SIMPLE;
	SortKey = 0;
	TextureCount = 0;
	ConstantCount = 0;
}
C2MaterialHeader::~C2MaterialHeader(){}

void C2MaterialHeader::ParseHeader(FLargeMemoryReader& Reader)
{
	BinaryReader::readString(Reader, &MaterialName);
	Reader.ByteOrderSerialize(&Blending, 1);
	Reader << SortKey;
	Reader << TextureCount;
	Reader << ConstantCount;
}