#pragma once

struct GfxColor
{
	uint8_t r, g, b, a;

	FColor ToFColor()
	{
		return FColor(this->r, this->g, this->b, this->a);
	}
	FVector4f ToVector()
	{
		return FVector4f(this->r / 255.0f, this->g / 255.0f, this->b / 255.0f, this->a / 255.0f);
	}
	friend FArchive& operator<<(FArchive& Ar, GfxColor& Color)
	{
		Ar << Color.r;
		Ar << Color.g;
		Ar << Color.b;
		Ar << Color.a;
		return Ar;
	}
};

struct GfxFace
{
	uint32_t index[3];
};

