#pragma once

struct GfxColor
{
	uint8_t r, g, b, a;

	FColor ToFColor()
	{
		return FColor(this->r, this->g, this->b, this->a);
	}
	FVector4 ToVector()
	{
		return FVector4(this->r / 255.0f, this->g / 255.0f, this->b / 255.0f, this->a / 255.0f);
	}
};

struct GfxFace
{
	uint32_t index[3];
};

