#pragma once

enum CoDLight
{
	SUN,
	SPOT_1,
	SPOT_2,
	SPOT_3,
	POINT
};

enum ConstantDataType
{
	UInt,
	UInt2,
	UInt3,
	UInt4,
	Float1,
	Float2,
	Float3,
	Float4,
	Boolean,
};

enum CoDMaterialSetting
{
	colorTint,
	alphaRevealParams,
	detailScale,
	detailScale1,
	detailScale2,
	detailScale3,
	specColorTint,
	rowCount,
	columnCount,
	imageTime
};

enum BlendingMode
{
	BLEND_VERTEX_SIMPLE, // Multiply Vertex Alpha with Reveal Texture
	BLEND_VERTEX_COMPLEX, // Mixture of Vertex Alpha and Reveal Texture (Game dependant)
	BLEND_MULTIPLY, // Multiply two base colors
	BLEND_TRANSLUCENT
};