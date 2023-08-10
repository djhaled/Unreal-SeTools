// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "C2MAssetEditorToolkit.h"

#include "Editor.h"
#include "EditorReimportHandler.h"
#include "EditorStyleSet.h"
#include "Widgets/SC2MAssetEditor.h"
#include "C2MAsset.h"
#include "UObject/NameTypes.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FC2MAssetEditorToolkit"

DEFINE_LOG_CATEGORY_STATIC(LogC2MAssetEditor, Log, All);


/* Local constants
 *****************************************************************************/

namespace C2MAssetEditor
{
	static const FName AppIdentifier("C2MAssetEditorApp");
	static const FName TabId("C2MEditor");
}


/* FC2MAssetEditorToolkit structors
 *****************************************************************************/

FC2MAssetEditorToolkit::FC2MAssetEditorToolkit(const TSharedRef<ISlateStyle>& InStyle)
	: C2MAsset(nullptr)
	, Style(InStyle)
{ }


FC2MAssetEditorToolkit::~FC2MAssetEditorToolkit()
{
	FReimportManager::Instance()->OnPreReimport().RemoveAll(this);
	FReimportManager::Instance()->OnPostReimport().RemoveAll(this);

	GEditor->UnregisterForUndo(this);
}


/* FC2MAssetEditorToolkit interface
 *****************************************************************************/
void FC2MAssetEditorToolkit::Initialize(UC2MAsset* InC2MAsset, const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost>& InToolkitHost)
{
	C2MAsset = InC2MAsset;

	// Support undo/redo
	C2MAsset->SetFlags(RF_Transactional);
	GEditor->RegisterForUndo(this);

	// create tab layout
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("Standalone_C2MAssetEditor")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewSplitter()
						->SetOrientation(Orient_Vertical)
						->SetSizeCoefficient(0.66f)
						->Split
						(
							FTabManager::NewStack()
								->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
								->SetHideTabWell(true)
								->SetSizeCoefficient(0.1f)
								
						)
						->Split
						(
							FTabManager::NewStack()
								->AddTab(C2MAssetEditor::TabId, ETabState::OpenedTab)
								->SetHideTabWell(true)
								->SetSizeCoefficient(0.9f)
						)
				)
		);

	FAssetEditorToolkit::InitAssetEditor(
		InMode,
		InToolkitHost,
		C2MAssetEditor::AppIdentifier,
		Layout,
		true /*bCreateDefaultStandaloneMenu*/,
		true /*bCreateDefaultToolbar*/,
		InC2MAsset
	);

	RegenerateMenusAndToolbars();
}


/* FAssetEditorToolkit interface
 *****************************************************************************/

FString FC2MAssetEditorToolkit::GetDocumentationLink() const
{
	return FString(TEXT("https://github.com/ue4plugins/C2MAsset"));
}


void FC2MAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_C2MAssetEditor", "C2M Asset Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(C2MAssetEditor::TabId, FOnSpawnTab::CreateSP(this, &FC2MAssetEditorToolkit::HandleTabManagerSpawnTab, C2MAssetEditor::TabId))
		.SetDisplayName(LOCTEXT("C2MEditorTabName", "C2M Editor"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));
}


void FC2MAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(C2MAssetEditor::TabId);
}


/* IToolkit interface
 *****************************************************************************/

FText FC2MAssetEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "C2M Asset Editor");
}


FName FC2MAssetEditorToolkit::GetToolkitFName() const
{
	return FName("C2MAssetEditor");
}


FLinearColor FC2MAssetEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}


FString FC2MAssetEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "C2MAsset ").ToString();
}


/* FGCObject interface
 *****************************************************************************/

void FC2MAssetEditorToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(C2MAsset);
}


/* FEditorUndoClient interface
*****************************************************************************/

void FC2MAssetEditorToolkit::PostUndo(bool bSuccess)
{ }


void FC2MAssetEditorToolkit::PostRedo(bool bSuccess)
{
	PostUndo(bSuccess);
}


/* FC2MAssetEditorToolkit callbacks
 *****************************************************************************/

TSharedRef<SDockTab> FC2MAssetEditorToolkit::HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;

	if (TabIdentifier == C2MAssetEditor::TabId)
	{
		TabWidget = SNew(SC2MAssetEditor, C2MAsset, Style);
	}

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			TabWidget.ToSharedRef()
		];
}


#undef LOCTEXT_NAMESPACE
