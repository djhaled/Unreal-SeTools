#pragma once

#include "Shared.h"
#include "Utils/BinaryReader.h"
#include "Serialization/LargeMemoryReader.h"
struct C2Weight
{
	uint32_t VertexIndex;
	uint32_t WeightID;
	float WeightValue;

};
struct C2MVertex
{
	FVector Vertice;
	FVector Normal;
	FVector2D UV;
	GfxColor Color;
	TArray<C2Weight> Weights;
};

class C2MODEL_API C2MSurface

{
public:
	C2MSurface();
	C2MSurface(C2MSurface* InSurface);
	C2MSurface(FString InName);
	FString Name;
	TArray< C2MVertex> Vertexes;
	TArray<GfxFace> Faces;
	TArray<int32_t> Materials;
	uint8_t UVCount;

	//
	int Surface_VertexCounter;
	uint32_t BoneCountBuffer;
	uint8_t Empty;
	uint8_t MaterialCount;
	uint32_t FaceCount;
	uint32_t VertCount;
	uint8_t MaxSkinBuffer;
	//
	void ParseSurface(FLargeMemoryReader& Reader,uint32_t buffer_BoneCount,uint16_t surfCount,int global_SurfVertCounter );
	TArray<GfxFace> ParseFaces(FLargeMemoryReader& Reader );
	TArray<C2Weight> ParseWeight(FLargeMemoryReader& Reader, uint32_t CurrentVertIndex);
};





