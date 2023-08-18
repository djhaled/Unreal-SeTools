#include "Structures/C2Mesh.h"
#include "Structures/C2Material.h"
void C2Mesh::ParseMesh(FLargeMemoryReader& Reader)
{
	Header = new C2MeshHeader();
	Header->ParseHeader(Reader);
	
	Surfaces.Reserve(Header->SurfaceCount);
	bIsXModel = true;
	
	for (size_t BoneIndex = 0; BoneIndex < Header->BoneCountBuffer; BoneIndex++)
	{
		C2Bone Bone;
		BinaryReader::readString(Reader, &Bone.Name);
		Bones.Add(Bone);
	}
	for (size_t BoneIndex = 0; BoneIndex < Header->BoneCountBuffer; BoneIndex++)
	{
		C2Bone& Bone = Bones[BoneIndex];
		Reader << Bone.non;
		Reader << Bone.ParentIndex;
		Reader << Bone.GlobalPosition;
		FQuat4f GlobalRotationQuat;
		Reader << GlobalRotationQuat;
		Bone.GlobalRotation = GlobalRotationQuat.Rotator();
		Reader << Bone.LocalPosition;
		FQuat4f LocalRotationQuat;
		Reader << LocalRotationQuat;
		Bone.LocalRotation = LocalRotationQuat.Rotator();
	}
	// Used to keep track of verts between surfaces because they are binded to the surface and unreal doesnt know that
	surf_vertCounter = 0;
	for (uint32_t SurfaceIndex = 0; SurfaceIndex < Header->SurfaceCount; SurfaceIndex++)
	{
		C2MSurface* Surface = new C2MSurface();
		Surface->ParseSurface(Reader, Header->BoneCountBuffer, SurfaceIndex, surf_vertCounter);
		Surfaces.Push(Surface);
		surf_vertCounter += Surface->Vertexes.Num();
	}

	for (size_t MaterialIndex = 0; MaterialIndex < Header->MaterialCountBuffer; MaterialIndex++)
	{
		C2ModelMaterial Material;

		BinaryReader::readString(Reader, &Material.MaterialName);
		for (size_t TextureTypeIndex = 0; TextureTypeIndex < 3; TextureTypeIndex++)
		{
			FString LocalTextureName;
			BinaryReader::readString(Reader, &LocalTextureName);
			int32 Index = LocalTextureName.Find(TEXT("\\_images\\"), ESearchCase::IgnoreCase);
			LocalTextureName = LocalTextureName.Mid(Index);
			Material.TextureNames.Add(LocalTextureName);
			Material.TextureTypes.Add(TTypes[TextureTypeIndex]);
		}
		Materials.Add(Material);
	}
	Reader.Close();
	
}

