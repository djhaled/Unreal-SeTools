#pragma once

#include "Enums.h"
#include <map>

#include "Serialization/LargeMemoryReader.h"

std::map<uint8_t,CoDMaterialSetting> ConstantTypes =
{
	{ 0x88BEFC31, alphaRevealParams },
	{ 0x88BEFC32, alphaRevealParams },
	{ 0xB60C3B3A, colorTint },
	{ 0x7793A24B, colorTint },
	{ 0x7793A248, colorTint },
	{ 0x7793A249, colorTint },
};

class C2MConstant
{
public:
	C2MConstant();
	uint32_t Hash;
	FVector4 Value;

	void ParseConstant(FLargeMemoryReader& Reader);
};
