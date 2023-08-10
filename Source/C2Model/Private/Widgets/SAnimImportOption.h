// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 *
 *
 */

enum class EPSAImportOptionDlgResponse : uint8
{
	Import,
	ImportAll,
	Cancel
};
class USAnimOptions;
class C2MODEL_API SAnimImportOption : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAnimImportOption)
		: _WidgetWindow()
	{}

	SLATE_ARGUMENT(TSharedPtr<SWindow>, WidgetWindow)
		SLATE_END_ARGS()

		SAnimImportOption()
		: UserDlgResponse(EPSAImportOptionDlgResponse::Cancel)
	{}
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	/** A property view to edit advanced options */
	TSharedPtr< class IDetailsView >				PropertyView;

	UPROPERTY(Category = MapsAndSets, EditAnywhere)
		mutable USAnimOptions* Stun;

	bool bShouldImportAll;
	/** If we should import */
	bool ShouldImport();

	/** If the current settings should be applied to all items being imported */
	bool ShouldImportAll();


	FReply OnImportAll();
	/** Called when 'Apply' button is pressed */
	FReply OnImport();

	FReply OnCancel();
private:
	EPSAImportOptionDlgResponse		UserDlgResponse;
	FReply HandleImport();

	/** Window that owns us */
	TWeakPtr< SWindow >							WidgetWindow;
};