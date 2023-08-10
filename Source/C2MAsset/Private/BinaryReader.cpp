#include "BinaryReader.h"

void BinaryReader::readString(FArchive& Ar, FString* outText)
{
	Ar.Seek(Ar.Tell() + 1);
	char c;
	// Start reading characters until we hit '\x00'
	while(Ar << c, c != 0)
	{
	    *outText += c;
	}
}

template <typename T>
TArray<T> BinaryReader::readList(FArchive& Ar, uint32_t count)
{
	TArray<T> arr;
	T vec;
	arr.Reserve(count);
	for (uint32_t x = 0; x < count; x++)
	{
		Ar.ByteOrderSerialize(&vec, sizeof(vec));;
		arr.Push(vec);
	}

	return arr;
}