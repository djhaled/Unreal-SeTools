#include "Utils/C2MUtilsImport.h"

FVector3f C2MUtilsImport::ConvertDir(FVector3f Vector)
{
	FVector3f Out;
	Out[0] = Vector[0];
	Out[1] = Vector[1];
	Out[2] = Vector[2];

	return Out;
}