#include "Structures/C2MSurface.h"
#include "Structures/C2Mesh.h"
#include "SocketTypes.h"

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

void C2MSurface::ParseSurface(FLargeMemoryReader& Reader,uint32_t buffer_BoneCount,uint16_t surfCount,int global_SurfVertCounter )
{
	// Initialize Class Vars
	Surface_VertexCounter = global_SurfVertCounter;
	BoneCountBuffer = buffer_BoneCount;
	// Read values from the reader
	Reader << Empty;
	Reader << MaterialCount;
	Reader << MaxSkinBuffer;
	Reader << VertCount;
	Reader << FaceCount;

	// Set the surface name
	Name = FString::Format(TEXT("surf_{0}"), { surfCount });

	for (size_t i = 0; i < VertCount; i++)
	{
		C2MVertex Vertex;
		Reader << Vertex.Vertice;
		Vertexes.Add(Vertex);
	}
	
	for (size_t i = 0; i < VertCount; i++)
	{
		C2MVertex& Vertex = Vertexes[i];
		Reader << Vertex.UV;
	}
	for (size_t i = 0; i < VertCount; i++)
	{
		C2MVertex& Vertex = Vertexes[i];
		Reader << Vertex.Normal;
	}
	for (size_t i = 0; i < VertCount; i++)
	{
		C2MVertex& Vertex = Vertexes[i];
		Reader << Vertex.Color;
	}
	for (size_t i = 0; i < VertCount; i++)
	{
		C2MVertex& Vertex = Vertexes[i];
		Vertex.Weights = ParseWeight(Reader, i);
	}
	//
    Faces = ParseFaces(Reader);
	Materials = BinaryReader::readList<int32_t>(Reader, MaterialCount);
}


TArray<GfxFace> C2MSurface::ParseFaces(FLargeMemoryReader& Reader)
{
    TArray<GfxFace> allFaces;
    for (uint32_t i = 0; i < FaceCount; i++)
    {
        GfxFace face;
        if (VertCount <= 0xFF)
        {
            // Read as byte
            uint8_t index1, index2, index3;
            Reader << index3;
            Reader << index2;
            Reader << index1;
            face.index[0] = index1 + Surface_VertexCounter;
            face.index[1] = index2 + Surface_VertexCounter;
            face.index[2] = index3 + Surface_VertexCounter;
        }
        else if (VertCount <= 0xFFFF)
        {
            // Read as short
            uint16_t index1, index2, index3;
            Reader << index3;
            Reader << index2;
            Reader << index1;
        	face.index[0] = index1 + Surface_VertexCounter;
        	face.index[1] = index2 + Surface_VertexCounter;
        	face.index[2] = index3 + Surface_VertexCounter;
        }
        else
        {
            // Read as int
        	int index1,index2,index3;
        	Reader << index1;
        	Reader << index2;
        	Reader << index3;
        	face.index[0] = index1 + Surface_VertexCounter;
        	face.index[1] = index2 + Surface_VertexCounter;
        	face.index[2] = index3 + Surface_VertexCounter;
        }
        allFaces.Add(face);
    }
    return allFaces;
}

TArray<C2Weight> C2MSurface::ParseWeight(FLargeMemoryReader& Reader,  uint32_t CurrentVertIndex)
{
	TArray<C2Weight> C2Weights;
	for (uint32_t i = 0; i < MaxSkinBuffer; i++)
	{
		C2Weight Weight;
		if (BoneCountBuffer <= 0xFF)
		{
			uint8_t lc;
			Reader << lc;
			Weight.WeightID = lc;
		}
		else if (BoneCountBuffer <= 0xFFFF)
		{
			uint16_t lc;
			Reader << lc;
			Weight.WeightID = lc;
		}
		else
		{
			uint32_t lc;
			Reader << lc;
			Weight.WeightID = lc;
		}
		Reader << Weight.WeightValue;
		Weight.VertexIndex = CurrentVertIndex + Surface_VertexCounter; 
		C2Weights.Add(Weight);
	}
	return  C2Weights;
}

