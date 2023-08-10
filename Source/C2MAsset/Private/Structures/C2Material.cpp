#include "C2Material.h"
#include <map>


C2Material::C2Material()
{
	Header = new C2MaterialHeader();
}
void C2Material::ParseMaterial(FLargeMemoryReader& Reader)
{
	Header->ParseHeader(Reader);
	Textures.Reserve(Header->TextureCount);
	for (int i = 0; i < Header->TextureCount; i++)
	{
		C2MTexture Texture = C2MTexture();
		Texture.ParseTexture(Reader);
		Textures.Push(Texture);
	}
	Constants.Reserve(Header->ConstantCount);
	for (int i = 0; i < Header->ConstantCount; i++)
	{
		C2MConstant Constant = C2MConstant();
		Constant.ParseConstant(Reader);
		Constants.Push(Constant);
	}
}