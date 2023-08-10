#include "C2MUtilsImport.h"

FVector C2MUtilsImport::ConvertDir(FVector Vector)
{
	FVector Out;
	Out[0] = Vector[0];
	Out[1] = -Vector[1];
	Out[2] = Vector[2];

	return Out;
}
