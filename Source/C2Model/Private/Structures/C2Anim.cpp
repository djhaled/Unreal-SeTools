#include "Structures/C2Anim.h"
#include "Structures/C2Material.h"

void C2Anim::ParseAnim(FLargeMemoryReader& Reader)
{
	ParseHeader(Reader);
	const TArray<FString> BoneNames = ParseBoneNames(Reader);
	const TArray<AnimationBoneModifier> AnimModifiers = ParseAnimModifiers(Reader);
	ParseBoneData(Reader, AnimModifiers, BoneNames);
}

void C2Anim::ParseHeader(FLargeMemoryReader& Reader)
{
	Reader.ByteOrderSerialize(&Header.Magic, sizeof(Header.Magic));
	Reader << Header.Version;
	Reader << Header.HeaderSize;
	Reader << Header.AnimType;
	Reader << Header.bLooping;
	Reader << Header.DataFlag;
	Reader << Header.undef_1;
	Reader << Header.undef_2;
	Reader << Header.FrameRate;
	Reader << Header.FrameCountBuffer;
	Reader << Header.BoneCountBuffer;
	Reader << Header.AnimationBoneModifiers;
	Reader.ByteOrderSerialize(&Header.Reserved, sizeof(Header.Reserved));
	Reader << Header.NotificationBuffer;
}

TArray<FString> C2Anim::ParseBoneNames(FLargeMemoryReader& Reader) const
{
	TArray<FString> BoneNames;
	for (uint32_t an_id = 0; an_id < Header.BoneCountBuffer; an_id++)
	{
		FString bName;
		BinaryReader::readString(Reader, &bName);
		BoneNames.Add(bName);
	}
	return BoneNames;
}

TArray<AnimationBoneModifier> C2Anim::ParseAnimModifiers(FLargeMemoryReader& Reader) const
{
	TArray<AnimationBoneModifier> AnimModifiers;
	for (uint8_t an_mo = 0; an_mo < Header.AnimationBoneModifiers; an_mo++)
	{
		AnimationBoneModifier AnimModifier;
		int16_t Index;
		if (Header.BoneCountBuffer <= 0xFF)
		{
			// Read it as a byte
			uint8_t ByteIndex;
			Reader << ByteIndex;
			Index = ByteIndex;
		}
		else
		{
			// Read it as a short
			uint16_t ShortIndex;
			Reader << ShortIndex;
			Index = ShortIndex;
		}
		AnimModifier.index = Index;
		Reader << AnimModifier.AnimType;
		AnimModifiers.Add(AnimModifier);
	}
	return AnimModifiers;
}

void C2Anim::ParseBoneData(FLargeMemoryReader& Reader, const TArray<AnimationBoneModifier>& AnimModifiers, const TArray<FString>& BoneNames)
{

	for (uint32_t an_tag = 0; an_tag < Header.BoneCountBuffer; an_tag++)
	{
		TArray<WraithAnimFrame<FVector>> Locations;
		TArray<WraithAnimFrame<FVector>> Scales;
		TArray<WraithAnimFrame<FQuat>> Rotations;
		BoneInfo o_BoneInfo;
		o_BoneInfo.Name = BoneNames[an_tag];
		o_BoneInfo.index = an_tag;

		uint8_t random_flag;
		Reader << random_flag;
		if (static_cast<uint8_t>(Header.DataFlag) & static_cast<uint8_t>(ESEAnimDataPresenceFlags::SEANIM_BONE_LOC))
		{
			ParseKeyframeData<FVector>(Reader, o_BoneInfo.BonePositions);
		}
		if (static_cast<uint8_t>(Header.DataFlag) & static_cast<uint8_t>(ESEAnimDataPresenceFlags::SEANIM_BONE_ROT))
		{
			ParseKeyframeData<FQuat>(Reader, o_BoneInfo.BoneRotations);
		}
		if (static_cast<uint8_t>(Header.DataFlag) & static_cast<uint8_t>(ESEAnimDataPresenceFlags::SEANIM_BONE_SCALE))
		{
			ParseKeyframeData<FVector>(Reader, o_BoneInfo.BoneScale);
		}
		BonesInfos.Add(o_BoneInfo);
	}

}
FQuat C2Anim::FixRotationAbsolute(FQuat QuatRot, FQuat InitialRot)
{
	if (FMath::Abs(QuatRot.X) <= 0.0005)
	{
		QuatRot.X = InitialRot.X;
	}
	if (FMath::Abs(QuatRot.Y) <= 0.0005)
	{
		QuatRot.Y = InitialRot.Y;
	}
	if (FMath::Abs(QuatRot.Z) <= 0.0005)
	{
		QuatRot.Z = InitialRot.Z;
	}
	if (FMath::Abs(QuatRot.W) <= 0.0005)
	{
		QuatRot.W = InitialRot.W;
	}

	if (FMath::Abs(QuatRot.X) <= 0.0005 && FMath::Abs(QuatRot.Y) <= 0.0005 && FMath::Abs(QuatRot.Z) <= 0.0005 && FMath::Abs(QuatRot.W) <= 0.0005)
	{
		QuatRot = InitialRot;
	}
	return QuatRot;
}
FVector C2Anim::FixPositionAbsolute(FVector QuatPos, FVector InitialPos)
{
	if (FMath::Abs(QuatPos.X) <= 0.0005)
	{
		QuatPos.X = InitialPos.X;
	}
	if (FMath::Abs(QuatPos.Y) <= 0.0005)
	{
		QuatPos.Y = InitialPos.Y;
	}
	if (FMath::Abs(QuatPos.Z) <= 0.0005)
	{
		QuatPos.Z = InitialPos.Z;
	}

	if (FMath::Abs(QuatPos.X) <= 0.0005 && FMath::Abs(QuatPos.Y) <= 0.0005 && FMath::Abs(QuatPos.Z) <= 0.0005)
	{
		QuatPos = InitialPos;
	}
	return QuatPos;
}
template <typename T>
void C2Anim::ParseKeyframeData(FLargeMemoryReader& Reader, TArray<WraithAnimFrame<T>>& KeyframeArray)
{
	WraithAnimFrame<T> AnimFrame;
	uint32_t keys;
	if (Header.FrameCountBuffer <= 0xFF)
	{
		uint8_t byteIndex;
		Reader << byteIndex;
		keys = byteIndex;
	}
	else if (Header.FrameCountBuffer <= 0xFFFF)
	{
		uint16_t byte16;
		Reader << byte16;
		keys = byte16;
	}
	else
	{
		Reader << keys;
	}

	for (uint32_t key = 0; key < keys; key++)
	{
		uint32_t keyFrame;
		if (Header.FrameCountBuffer <= 0xFF)
		{
			uint8_t byteIndex;
			Reader << byteIndex;
			keyFrame = byteIndex;
		}
		else if (Header.FrameCountBuffer <= 0xFFFF)
		{
			uint16_t byte16;
			Reader << byte16;
			keyFrame = byte16;
		}
		else
		{
			Reader << keyFrame;
		}
		AnimFrame.Frame = keyFrame;
		T KeyValue;
		Reader << KeyValue;
		AnimFrame.Value = KeyValue;
		KeyframeArray.Add(AnimFrame);
	}
}
