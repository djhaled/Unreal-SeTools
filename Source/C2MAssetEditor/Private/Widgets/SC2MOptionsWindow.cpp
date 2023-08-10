#include "SC2MOptionsWindow.h"

#include "C2MImportOptions.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "EditorStyleSet.h"
#include "IDocumentation.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"

#define LOCTEXT_NAMESPACE "C2MOption"

void SC2MOptionsWindow::Construct(const FArguments& InArgs)
{
	MapHeader = InArgs._MapHeader;
	check (MapHeader);
	ImportOptions = InArgs._ImportOptions;
	WidgetWindow = InArgs._WidgetWindow;
	check (ImportOptions);
	TSharedPtr<SBox> ImportMapHeaderDisplay;
	TSharedPtr<SBox> InspectorBox;
	
	this->ChildSlot
	[
		SNew(SBox)
		.MaxDesiredHeight(InArgs._MaxWindowHeight)
		.MaxDesiredWidth(InArgs._MaxWindowWidth)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2)
			[
				SAssignNew(ImportMapHeaderDisplay, SBox)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2)
			[
				SAssignNew(InspectorBox, SBox)
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Right)
			.Padding(2)
			[
				SNew(SUniformGridPanel)
				.SlotPadding(2)
				+ SUniformGridPanel::Slot(0, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.Text(LOCTEXT("C2MOptionWindow_ImportMap", "Import"))
					.ToolTipText(LOCTEXT("C2MOptionWindow_ImportMap_ToolTip", "Import C2M Map with current settings"))
					.OnClicked(this, &SC2MOptionsWindow::OnImport)
				]
				+ SUniformGridPanel::Slot(1, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.Text(LOCTEXT("C2MOptionWindow_Cancel", "Cancel"))
					.ToolTipText(LOCTEXT("C2MOptionWindow_Cancel_ToolTip", "Cancels importing this C2M file"))
					.OnClicked(this, &SC2MOptionsWindow::OnCancel)
				]
			]
		]
	];
	
	
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea);
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	InspectorBox->SetContent(DetailsView->AsShared());
	
	/* Map Information Widget
	 * Here we display general like map name, counts, etc. */
	ImportMapHeaderDisplay->SetContent(CreateMapHeader(MapHeader).ToSharedRef());
	DetailsView->SetObject(ImportOptions);
}

/* Generate new slate line for our map info display
 * @InKey - Name of the property
 * @InValue - Value of the property*/
TSharedPtr<SHorizontalBox> SC2MOptionsWindow::AddNewHeaderProperty(FText InKey, FText InValue)
{
	return SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
					.Text(InKey)
				]
				+SHorizontalBox::Slot()
				//.Padding(50 - InKey.ToString().Len(), 0, 0, 0)
				//.AutoWidth()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Font(FEditorStyle::GetFontStyle("CurveEd.InfoFont"))
					.Text(InValue)
				];
}

TSharedPtr<SBorder> SC2MOptionsWindow::CreateMapHeader(UC2MapHeader* InMapHeader)
{
	check(InMapHeader);
	return SNew(SBorder)
		.Padding(FMargin(3))
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			// Add map name to our info display
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				AddNewHeaderProperty(FText::FromString("Map Name: "),FText::FromString(InMapHeader->MapName)).ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				AddNewHeaderProperty(FText::FromString("Models :"),FText::AsNumber(InMapHeader->ObjectsCount)).ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				AddNewHeaderProperty(FText::FromString("Instances :"),FText::AsNumber(InMapHeader->InstancesCount)).ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				AddNewHeaderProperty(FText::FromString("Materials :"),FText::AsNumber(InMapHeader->MaterialsCount)).ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				AddNewHeaderProperty(FText::FromString("Lights :"),FText::AsNumber(InMapHeader->LightsCount)).ToSharedRef()
			]
		];
}

#undef LOCTEXT_NAMESPACE