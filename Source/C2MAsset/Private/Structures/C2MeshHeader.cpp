#include "C2MeshHeader.h"
#include "BinaryReader.h"

C2MeshHeader::C2MeshHeader()
{
	MeshName = "";
	VertexCount = 0;
	SurfaceCount = 1;
	FaceCount = 0;
}
C2MeshHeader::C2MeshHeader(FString InName)
{
	MeshName = InName;
	VertexCount = 0;
	SurfaceCount = 1;
	FaceCount = 0;
}
void C2MeshHeader::ParseHeader(FLargeMemoryReader& Reader)
{
	BinaryReader::readString(Reader, &MeshName);
	Reader << VertexCount;
	Reader << SurfaceCount;
	Reader << FaceCount;
}