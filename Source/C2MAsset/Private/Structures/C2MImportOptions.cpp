#include "C2MImportOptions.h"

void UC2MImportOptions::ParseGameSpecificOptions(UC2MapHeader* MapHeader)
{
	if (MapHeader->GameVersion == "modern_warfare" ||
		MapHeader->GameVersion == "world_at_war" ||
		MapHeader->GameVersion == "modern_warfare_2" ||
		MapHeader->GameVersion == "modern_warfare_3" ||
		MapHeader->GameVersion == "black_ops_3" ||
		MapHeader->GameVersion == "black_ops_4" ||
		MapHeader->GameVersion == "black_ops_5")
			bHasMixMaterial = false;
	if (MapHeader->GameVersion == "modern_warfare" ||
		MapHeader->GameVersion == "world_at_war" ||
		MapHeader->GameVersion == "modern_warfare_2" ||
		MapHeader->GameVersion == "modern_warfare_3")
			bHasComplexBlend = false;
}
