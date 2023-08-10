#include "C2Mesh.h"

void C2Mesh::ParseMesh(FLargeMemoryReader& Reader)
{
	Header = new C2MeshHeader();
	Header->ParseHeader(Reader);
	Vertices = BinaryReader::readList<FVector>(Reader, Header->VertexCount);
	Normals = BinaryReader::readList<FVector>(Reader, Header->VertexCount);
	UVs.Reserve(Header->VertexCount);
	for (uint32_t i = 0; i < Header->VertexCount; i++)
	{
		TArray<FVector2D> CurrentUVSets;
		uint32_t CurrentUVCount;
		Reader << CurrentUVCount;
		CurrentUVSets = BinaryReader::readList<FVector2D>(Reader, CurrentUVCount);;
		UVs.Push(CurrentUVSets);			
	}
	Colors = BinaryReader::readList<GfxColor>(Reader, Header->VertexCount);
	Surfaces.Reserve(Header->SurfaceCount);
	bIsXModel = Header->MeshName != "mapGeometry" ? true : false;
	for (uint32_t i = 0; i < Header->SurfaceCount; i++)
	{
		C2MSurface* Surface = new C2MSurface();
		Surface->ParseSurface(Reader);
		Surfaces.Push(Surface);
	}
}

