#pragma once

#include "Serialization/Archive.h"
#include "Containers/UnrealString.h"

/**
* Base class for serializing arbitrary data in memory.
*/
class C2MASSET_API BinaryReader {
public:
	BinaryReader(){};

	
	static void readString(FArchive& Ar, FString* outText);
	
	template <typename T>
	static TArray<T> readList(FArchive& Ar, uint32_t count);
};