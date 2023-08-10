#include "C2MModelInstance.h"
#include "C2MTexture.h"

C2MModelInstance::C2MModelInstance()
{
	Name = "";
	FTransform VTransform = FTransform(FRotator(0,0 ,0),
		FVector(0, 0, 0),
		FVector(0, 0, 0));
	Transform.Add(VTransform);


}

C2MModelInstance::C2MModelInstance(FString InName)
{
	Name = InName;
}

void C2MModelInstance::ParseModelInstance(FLargeMemoryReader& Reader)
{
	BinaryReader::readString(Reader, &Name);
	Name = C2MTexture::NoIllegalSigns(Name);
	FVector Position;
	FVector Scale;
	FRotator Rotator;
	uint8_t RotationMode;
	// We do not know yet whether rotation is quat or angles
	FQuat RotQuat;
	FVector RotDegrees;
	
	Reader << Position;
	Reader << RotationMode;
	switch (RotationMode)
	{
		// COD Quat
		default:
		case 0:
			Reader << RotQuat;
			Rotator = FRotator(FQuat(-RotQuat.X, RotQuat.Y, -RotQuat.Z, RotQuat.W));
			break;

		// COD Angles
		case 1:
			Reader << RotDegrees;
			Rotator = FRotator(-RotDegrees.Y, -RotDegrees.Z, RotDegrees.X);
			break;
	}
		
	Reader << Scale;
	Transform[0] = (FTransform(Rotator, FVector(Position.X, -Position.Y, Position.Z), Scale));
}