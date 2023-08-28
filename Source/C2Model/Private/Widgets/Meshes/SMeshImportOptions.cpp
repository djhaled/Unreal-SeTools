#include "SMeshImportOptions.h"

#include "EditorStyleSet.h"
#include "ObjectEditorUtils.h"
#include "Widgets/Input/SButton.h"
#include "UserMeshOptions.h"
#include "SlateOptMacros.h"
#include "Structures/C2Mesh.h"
#include "Structures/C2MeshHeader.h"

#define LOCTEXT_NAMESPACE "PSAImportFac"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMeshImportOptions::Construct(const FArguments& InArgs)
{
	MeshHeader = InArgs._MeshHeader;
	WidgetWindow = InArgs._WidgetWindow;
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bHideSelectionTip = true;
	TSharedRef<IDetailsView> Details = EditModule.CreateDetailView(DetailsViewArgs);
	EditModule.CreatePropertyTable();
	UObject* Container = NewObject<UUserMeshOptions>();
	Options = Cast<UUserMeshOptions>(Container);
	Details->SetObject(Container);
	TSharedPtr<SBox> ImportMapHeaderDisplay;
	Details->SetEnabled(true);

	this->ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
		.Padding(10)
		[
			SNew(SVerticalBox)
        
			// ImportMapHeaderDisplay
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(ImportMapHeaderDisplay, SBox)
			]
        
			// Details
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2)
			[
				SNew(SBox)
				.WidthOverride(400)
				[
					Details
				]
			]
        
			// Apply/Apply to All/Cancel Buttons
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
            
				// Apply Button
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.Padding(2)
				[
					SNew(SButton)
					.Text(LOCTEXT("Import", "Apply"))
					.OnClicked(this, &SMeshImportOptions::OnImport)
				]
            
				// Apply to All Button
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2)
				[
					SNew(SButton)
					.Text(LOCTEXT("ImportAll", "Apply to All"))
					.OnClicked(this, &SMeshImportOptions::OnImportAll)
				]
            
				// Cancel Button
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2)
				[
					SNew(SButton)
					.Text(LOCTEXT("Cancel", "Cancel"))
					.OnClicked(this, &SMeshImportOptions::OnCancel)
				]
			]
		]
	];

	ImportMapHeaderDisplay->SetContent(CreateMapHeader(MeshHeader).ToSharedRef());


}

TSharedPtr<SBorder> SMeshImportOptions::CreateMapHeader(C2Mesh* MeshHeader)
{
	check(MeshHeader);
	return SNew(SBorder)
		.Padding(FMargin(3))
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			// Add map name to our info display
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				AddNewHeaderProperty(FText::FromString("Model: "),FText::FromString(MeshHeader->Header->MeshName)).ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				AddNewHeaderProperty(FText::FromString("Vertices:"),FText::AsNumber(MeshHeader->surf_vertCounter)).ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				AddNewHeaderProperty(FText::FromString("Materials:"),FText::AsNumber(MeshHeader->Header->MaterialCountBuffer)).ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				AddNewHeaderProperty(FText::FromString("Bones:"),FText::AsNumber(MeshHeader->Header->BoneCountBuffer)).ToSharedRef()
			]
		];
}
/* Generate new slate line for our map info display
 * @InKey - Name of the property
 * @InValue - Value of the property*/
TSharedPtr<SHorizontalBox> SMeshImportOptions::AddNewHeaderProperty(FText InKey, FText InValue)
{
	return SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Font(FAppStyle::GetFontStyle("CurveEd.LabelFont"))
					.Text(InKey)
				]
				+SHorizontalBox::Slot()
				//.Padding(50 - InKey.ToString().Len(), 0, 0, 0)
				//.AutoWidth()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Font(FAppStyle::GetFontStyle("CurveEd.InfoFont"))
					.Text(InValue)
				];
}
bool SMeshImportOptions::ShouldImport()
{
	return UserDlgResponse == EPSAImportOptionDlgResponse::Import;
}
bool SMeshImportOptions::ShouldImportAll()
{
	return UserDlgResponse == EPSAImportOptionDlgResponse::ImportAll;
}
FReply SMeshImportOptions::OnImportAll()
{
	UserDlgResponse = EPSAImportOptionDlgResponse::ImportAll;
	return HandleImport();
}
FReply SMeshImportOptions::OnImport()
{
	UserDlgResponse = EPSAImportOptionDlgResponse::Import;
	return HandleImport();
}
FReply SMeshImportOptions::OnCancel()
{
	UserDlgResponse = EPSAImportOptionDlgResponse::Cancel;
	return FReply::Handled();
}
FReply SMeshImportOptions::HandleImport()
{
	if (WidgetWindow.IsValid())
	{
		WidgetWindow.Pin()->RequestDestroyWindow();
	}
	return FReply::Handled();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


