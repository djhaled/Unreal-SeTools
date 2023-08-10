#pragma once

#include "CoreMinimal.h"
#include "C2MImportOptions.h"
#include "C2MAsset.h"
#include "Modules/ModuleManager.h"
#include "Factories/ImportSettings.h"


class FToolBarBuilder;
class FMenuBuilder;

class UC2MImportUI
{
public:
	UC2MImportUI(){};
	//C2MImportOptions ImportOptions = C2MImportOptions();
	
	static void CreateC2MImportWindow(UC2MapHeader* MapHeader, UC2MImportOptions* ImportOptions);
private:
};
