#pragma once
#include <iostream>
#include <map>
#include "Shared.h"
#include "C2MSurface.h"
#include "Utils/BinaryReader.h"
//#include "RawMesh.h"
#include "Engine/StaticMesh.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "C2MeshHeader.h"
#include "Misc/ScopedSlowTask.h"
#include "StaticMeshAttributes.h"
UENUM()
enum class ESEAnimAnimationType : uint8
{
    SEANIM_ABSOLUTE = 0,
    SEANIM_ADDITIVE = 1,
    SEANIM_RELATIVE = 2,
    SEANIM_DELTA = 3
};

struct AnimationBoneModifier
{
    uint16_t index;
    ESEAnimAnimationType AnimType;
};
template<class T>
struct WraithAnimFrame
{
    // The frame of this animation key
    uint32_t Frame;
    // The frame value of this key
    T Value;

    bool HasFrame(uint32_t frameToCheck) const
    {
        return Frame == frameToCheck;
    }
};

struct BoneInfo
{
    FString Name;
    int index;
    TArray<WraithAnimFrame<FVector3f>> BonePositions;
    TArray<WraithAnimFrame<FQuat4f>> BoneRotations;
    TArray<WraithAnimFrame<FVector3f>> BoneScale;

    FVector3f GetPositionAtFrame(uint32_t FrameAsked) const
    {
        for (const auto& var : BonePositions)
        {
            if (var.HasFrame(FrameAsked))
            {
                return var.Value;
            }
        }
        return FVector3f(-1,-1,-1);
    }

    FQuat4f GetRotationAtFrame(uint32_t FrameAsked) const
    {
        for (const auto& var : BoneRotations)
        {
            if (var.HasFrame(FrameAsked))
            {
                return var.Value;
            }
        }
        return FQuat4f(-1, -1, -1, -1);
    }

    FVector3f GetScaleAtFrame(uint32_t FrameAsked) const
    {
        for (const auto& var : BoneScale)
        {
            if (var.HasFrame(FrameAsked))
            {
                return var.Value;
            }
        }
        return FVector3f(-1, -1, -1);
    }
};


struct FAnimHeader
{
    char Magic[6];
    uint16_t Version;
    uint16_t HeaderSize;
    ESEAnimAnimationType AnimType;
    uint8_t bLooping;
    uint8_t DataFlag;
    uint8_t undef_1;
    uint16_t undef_2;
    float FrameRate;
    uint32_t FrameCountBuffer;
    uint32_t BoneCountBuffer;
    uint8_t AnimationBoneModifiers;
    uint8_t Reserved[3];
    uint32_t NotificationBuffer;
};
enum class ESEAnimDataPresenceFlags : uint8
{
    SEANIM_BONE_LOC = 1 << 0,
    SEANIM_BONE_ROT = 1 << 1,
    SEANIM_BONE_SCALE = 1 << 2,
    SEANIM_PRESENCE_BONE = SEANIM_BONE_LOC | SEANIM_BONE_ROT | SEANIM_BONE_SCALE,
    SEANIM_PRESENCE_NOTE = 1 << 6,
    SEANIM_PRESENCE_CUSTOM = 1 << 7
};

class C2MODEL_API C2Anim
{
public:
	C2Anim(){};
	
	void ParseAnim(FLargeMemoryReader& Reader);
    void ParseHeader(FLargeMemoryReader& Reader);
    TArray<FString> ParseBoneNames(FLargeMemoryReader& Reader) const;
    TArray<AnimationBoneModifier> ParseAnimModifiers(FLargeMemoryReader& Reader) const;
    void ParseBoneData(FLargeMemoryReader& Reader, const TArray<AnimationBoneModifier>& AnimModifiers, const TArray<FString>& BoneNames);
    FAnimHeader Header;
    TArray<BoneInfo> BonesInfos;
    template <typename T>
    void ParseKeyframeData(FLargeMemoryReader& Reader, TArray<WraithAnimFrame<T>>& KeyframeArray);
    static FQuat4f FixRotationAbsolute(FQuat4f QuatRot, FQuat4f InitialRot);
    static FVector3f FixPositionAbsolute(FVector3f QuatPos, FVector3f InitialPos);
};

