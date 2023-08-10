// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "C2MAssetEditorSettings.h"

#include "Misc/Paths.h"


UC2MAssetEditorSettings::UC2MAssetEditorSettings()
	: BackgroundColor(FLinearColor::White)
	, ForegroundColor(FLinearColor::Black)
	, Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 10))
	, Margin(4.0f)
{ }
