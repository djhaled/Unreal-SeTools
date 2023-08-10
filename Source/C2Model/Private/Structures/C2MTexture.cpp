#include "Structures/C2MTexture.h"

C2MTexture::C2MTexture()
{
	TextureName = "";
	TextureType = "";
	TexturePath = "";
}

void C2MTexture::ParseTexture(FLargeMemoryReader& Reader)
{
	BinaryReader::readString(Reader, &TextureName);
	//TextureName = TextureName.Replace(TEXT("~"),TEXT("_")).Replace(TEXT("$"),TEXT("_")); // UE Does not like those signs :D
	BinaryReader::readString(Reader, &TextureType);
}

/* Remove characters & signs Unreal doesn't like */
FString C2MTexture::NoIllegalSigns(const FString& InString)
{
	return InString.Replace(TEXT("~"),TEXT("_")).Replace(TEXT("$"),TEXT("_")).Replace(TEXT("&"),TEXT("_")).Replace(TEXT("#"),TEXT("_"));
}