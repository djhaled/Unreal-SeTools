// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "C2MCreateOptions.h"
class FText;
class ISlateStyle;
class UC2MAsset;
class C2MModelInstance;
DECLARE_LOG_CATEGORY_EXTERN(LogC2M, Log, All);
/**
 * Implements the UTextAsset asset editor widget.
 */
class SC2MAssetEditor
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SC2MAssetEditor)
	{ }
	SLATE_END_ARGS()

public:

	/** Virtual destructor. */
	virtual ~SC2MAssetEditor();

	/**
	 * Construct this widget
	 *
	 * @param InArgs The declaration data for this widget.
	 * @param InTextAsset The UTextAsset asset to edit.
	 * @param InStyleSet The style set to use.
	 */
	void Construct(const FArguments& InArgs, UC2MAsset* InTextAsset, const TSharedRef<ISlateStyle>& InStyle);

	FReply OnCreateMap();
	FReply OnButtonTest();
	FReply OnButtonTest1();
	TArray<C2MModelInstance*> GetUniqueMergedModelInstances(TArray<C2MModelInstance*> OldInstances);
	FReply OnButtonTest2();
	FReply OnButtonTest3();
	FReply OnButtonTest4();
	FReply OnButtonTest5();

private:

	// /** Callback for text changes in the editable text box. */
	// void HandleEditableTextBoxTextChanged(const FText& NewText);
	//
	// /** Callback for committed text in the editable text box. */
	// void HandleEditableTextBoxTextCommitted(const FText& Comment, ETextCommit::Type CommitType);
	//
	// /** Callback for property changes in the text asset. */
	// void HandleTextAssetPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent);


private:

	/** Holds the editable text box widget. */
	TSharedPtr<SMultiLineEditableTextBox> EditableTextBox;
	UC2MCreateOptions* CreateOptions;
	/** Pointer to the text asset that is being edited. */
	UC2MAsset* C2MAsset;
	TSharedPtr<class IDetailsView> DetailsView;
};
