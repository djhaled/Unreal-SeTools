#include "C2MConstant.h"

C2MConstant::C2MConstant()
{
	// Default
}
void C2MConstant::ParseConstant(FLargeMemoryReader& Reader)
{
	Reader << Hash;
	Reader << Value;
}