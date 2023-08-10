#include "C2MSurface.h"

C2MSurface::C2MSurface()
{
	// Default
	UVCount = 0;
}

C2MSurface::C2MSurface(C2MSurface* InSurface)
{
	Name = InSurface->Name;
	Materials = InSurface->Materials;
	UVCount = 0;
}
C2MSurface::C2MSurface(FString InName)
{
	Name = InName;
	UVCount = 0;
}

void C2MSurface::ParseSurface(FLargeMemoryReader& Reader)
{
	BinaryReader::readString(Reader, &Name);
	uint8_t MaterialCount;
	uint32_t FaceCount;
	Reader << UVCount;
	Reader << MaterialCount;
	Materials = BinaryReader::readList<uint16_t>(Reader, MaterialCount);
	Reader << FaceCount;
	Faces = BinaryReader::readList<GfxFace>(Reader, FaceCount);
}
