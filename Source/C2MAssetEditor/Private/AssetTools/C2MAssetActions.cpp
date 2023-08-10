// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
// Based on TextAsset example - https://github.com/ue4plugins/TextAsset
#include "C2MAssetActions.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "C2MAsset.h"
#include "Styling/SlateStyle.h"

#include "C2MAssetEditorToolkit.h"


#define LOCTEXT_NAMESPACE "AssetTypeActions"


/* FC2MAssetActions constructors
 *****************************************************************************/

FC2MAssetActions::FC2MAssetActions(const TSharedRef<ISlateStyle>& InStyle)
	: Style(InStyle)
{ }


/* FAssetTypeActions_Base overrides
 *****************************************************************************/

bool FC2MAssetActions::CanFilter()
{
	return true;
}


// void FC2MAssetActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
// {
// 	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);
//
// 	auto C2MAssets = GetTypedWeakObjectPtrs<UC2MAsset>(InObjects);
//
// 	MenuBuilder.AddMenuEntry(
// 		LOCTEXT("C2MAsset_ReverseText", "Reverse Text"),
// 		LOCTEXT("C2MAsset_ReverseTextToolTip", "Reverse the text stored in the selected text asset(s)."),
// 		FSlateIcon(),
// 		FUIAction(
// 			FExecuteAction::CreateLambda([=]{
// 				for (auto& C2MAsset : C2MAssets)
// 				{
// 					if (C2MAsset.IsValid() && !C2MAsset->Text.IsEmpty())
// 					{
// 						C2MAsset->Text = FText::FromString(C2MAsset->Text.ToString().Reverse());
// 						C2MAsset->PostEditChange();
// 						C2MAsset->MarkPackageDirty();
// 					}
// 				}
// 			}),
// 			FCanExecuteAction::CreateLambda([=] {
// 				for (auto& C2MAsset : C2MAssets)
// 				{
// 					if (C2MAsset.IsValid() && !C2MAsset->Text.IsEmpty())
// 					{
// 						return true;
// 					}
// 				}
// 				return false;
// 			})
// 		)
// 	);
// }


uint32 FC2MAssetActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}


FText FC2MAssetActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_C2MAsset", "C2M Asset");
}


UClass* FC2MAssetActions::GetSupportedClass() const
{
	return UC2MAsset::StaticClass();
}


FColor FC2MAssetActions::GetTypeColor() const
{
	return FColor::Green;
}


bool FC2MAssetActions::HasActions(const TArray<UObject*>& InObjects) const
{
	return true;
}


void FC2MAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid()
		? EToolkitMode::WorldCentric
		: EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto C2MAsset = Cast<UC2MAsset>(*ObjIt);

		if (C2MAsset != nullptr)
		{
			TSharedRef<FC2MAssetEditorToolkit> EditorToolkit = MakeShareable(new FC2MAssetEditorToolkit(Style));
			EditorToolkit->Initialize(C2MAsset, Mode, EditWithinLevelEditor);
		}
	}
}


#undef LOCTEXT_NAMESPACE
