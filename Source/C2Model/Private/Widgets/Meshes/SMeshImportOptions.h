// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structures/C2Mesh.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Animations/SAnimImportOption.h"



class UUserMeshOptions;
class C2MODEL_API SMeshImportOptions : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMeshImportOptions)
		: _MeshHeader(nullptr)
		, _WidgetWindow()
	{}

	SLATE_ARGUMENT(C2Mesh*, MeshHeader)
	SLATE_ARGUMENT(TSharedPtr<SWindow>, WidgetWindow)
    
	SLATE_END_ARGS()

	SMeshImportOptions()
		: UserDlgResponse(EPSAImportOptionDlgResponse::Cancel)
	{}
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	static TSharedPtr<SHorizontalBox> AddNewHeaderProperty(FText InKey, FText InValue);
	static TSharedPtr<SBorder> CreateMapHeader(C2Mesh* MeshHeader);
	/** A property view to edit advanced options */
	TSharedPtr< class IDetailsView >				PropertyView;

	UPROPERTY(Category = MapsAndSets, EditAnywhere)
		mutable UUserMeshOptions* Options;

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
	C2Mesh* MeshHeader;
	EPSAImportOptionDlgResponse		UserDlgResponse;
	FReply HandleImport();

	/** Window that owns us */
	TWeakPtr< SWindow >							WidgetWindow;
};