#include "C2MImportUI.h"
#include "C2MImportOptions.h"
#include "SC2MOptionsWindow.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "SC2MAssetImportUI"

void UC2MImportUI::CreateC2MImportWindow(UC2MapHeader* MapHeader, UC2MImportOptions* ImportOptions)
{
	IMainFrameModule& MainFrameModule = IMainFrameModule::Get();
	/* Window Size & Position setup 
	 * Editor\UnrealEd\Private\Fbx\FbxMainImport.cpp */
	FVector2D C2MImportWindowSize = FVector2D(410.0f, 750.0f); // Max window size it can get based on current slate
	FSlateRect WorkAreaRect = FSlateApplicationBase::Get().GetPreferredWorkArea();
	FVector2D DisplayTopLeft(WorkAreaRect.Left, WorkAreaRect.Top);
	FVector2D DisplaySize(WorkAreaRect.Right - WorkAreaRect.Left, WorkAreaRect.Bottom - WorkAreaRect.Top);
	float ScaleFactor = FPlatformApplicationMisc::GetDPIScaleFactorAtPoint(DisplayTopLeft.X, DisplayTopLeft.Y);
	C2MImportWindowSize *= ScaleFactor;
	FVector2D WindowPosition = (DisplayTopLeft + (DisplaySize - C2MImportWindowSize) / 2.0f);
	
	// Create our main import window
	TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(NSLOCTEXT("UnrealEd", "C2MImportOpionsTitle", "C2M Import Options"))
			.SizingRule(ESizingRule::Autosized)
			.AutoCenter(EAutoCenter::None)
			.ClientSize(C2MImportWindowSize)
			.ScreenPosition(WindowPosition);

	// Create our options window widget
	TSharedPtr<SC2MOptionsWindow> C2MOptionsWindow;
	Window->SetContent
		(
			SAssignNew(C2MOptionsWindow, SC2MOptionsWindow)
			.MapHeader(MapHeader)
			.ImportOptions(ImportOptions)
			.WidgetWindow(Window)
			.MaxWindowWidth(C2MImportWindowSize.X)
			.MaxWindowHeight(C2MImportWindowSize.Y)
		);

	// Load window
	FSlateApplication::Get().AddModalWindow(Window, MainFrameModule.GetParentWindow(), false);
	}




#undef LOCTEXT_NAMESPACE