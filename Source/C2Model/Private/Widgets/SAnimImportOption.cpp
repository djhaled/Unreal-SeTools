#include "SAnimImportOption.h"
#include "ObjectEditorUtils.h"
#include "Widgets/Input/SButton.h"
#include "SAnimOptions.h"
#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "PSAImportFac"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SAnimImportOption::Construct(const FArguments& InArgs)
{
	WidgetWindow = InArgs._WidgetWindow;
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bHideSelectionTip = true;
	TSharedRef<IDetailsView> Details = EditModule.CreateDetailView(DetailsViewArgs);
	EditModule.CreatePropertyTable();
	UObject* Container = NewObject<USAnimOptions>();
	Stun = Cast<USAnimOptions>(Container);
	Details->SetObject(Container);
	Details->SetEnabled(true);


	this->ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Brushes.Panel"))
		.Padding(10)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			.Padding(FMargin(3))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		]
		]

	// Data row struct
	// Curve interpolation
	// Details panel
	+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2)
		[
			SNew(SBox)
			.WidthOverride(400)
		[
			Details
		]
		]
	+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.Padding(2)
		[
			SNew(SButton)
			.Text(LOCTEXT("Import", "Apply"))
		.OnClicked(this, &SAnimImportOption::OnImport)
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2)
		[
			SNew(SButton)
			.Text(LOCTEXT("ImportAll", "Apply to All"))
		.OnClicked(this, &SAnimImportOption::OnImportAll)
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2)
		[
			SNew(SButton)
			.Text(LOCTEXT("Cancel", "Cancel"))
		.OnClicked(this, &SAnimImportOption::OnCancel)
		]
		]
		]
	// Apply/Apply to All/Cancel
		];


}
bool SAnimImportOption::ShouldImport()
{
	return UserDlgResponse == EPSAImportOptionDlgResponse::Import;
}
bool SAnimImportOption::ShouldImportAll()
{
	return UserDlgResponse == EPSAImportOptionDlgResponse::ImportAll;
}
FReply SAnimImportOption::OnImportAll()
{
	UserDlgResponse = EPSAImportOptionDlgResponse::ImportAll;
	return HandleImport();
}
FReply SAnimImportOption::OnImport()
{
	UserDlgResponse = EPSAImportOptionDlgResponse::Import;
	return HandleImport();
}
FReply SAnimImportOption::OnCancel()
{
	UserDlgResponse = EPSAImportOptionDlgResponse::Cancel;
	return FReply::Handled();
}
FReply SAnimImportOption::HandleImport()
{
	if (WidgetWindow.IsValid())
	{
		WidgetWindow.Pin()->RequestDestroyWindow();
	}
	return FReply::Handled();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


