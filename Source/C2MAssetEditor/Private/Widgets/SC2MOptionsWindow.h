#pragma once
#include "C2MapHeader.h"
#include "C2MImportOptions.h"

class SC2MOptionsWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SC2MOptionsWindow )
		: _MapHeader(NULL)
		, _ImportOptions(NULL)
		, _WidgetWindow()
		, _MaxWindowHeight(0.0f)
		, _MaxWindowWidth(0.0f)
	{}

	SLATE_ARGUMENT( UC2MapHeader*, MapHeader )
	SLATE_ARGUMENT( UC2MImportOptions*, ImportOptions )
	SLATE_ARGUMENT( TSharedPtr<SWindow>, WidgetWindow )
	SLATE_ARGUMENT( float, MaxWindowHeight)
	SLATE_ARGUMENT( float, MaxWindowWidth)
SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	static TSharedPtr<SHorizontalBox> AddNewHeaderProperty(FText InKey, FText InValue);
	static TSharedPtr<SBorder> CreateMapHeader(UC2MapHeader* InMapHeader);

	FReply OnImport() const
	{
		ImportOptions->bShouldImport = true;
		if ( WidgetWindow.IsValid() )
		{
			WidgetWindow.Pin()->RequestDestroyWindow();
		}
		return FReply::Handled();
	}

	FReply OnCancel() const
	{
		ImportOptions->bShouldImport = false;
		if ( WidgetWindow.IsValid() )
		{
			WidgetWindow.Pin()->RequestDestroyWindow();
		}
		return FReply::Handled();
	}
	SC2MOptionsWindow() 
		: MapHeader(NULL)
	{}

private:
	UC2MapHeader*	MapHeader;
	UC2MImportOptions* ImportOptions;
	TWeakPtr< SWindow > WidgetWindow;
	TSharedPtr<class IDetailsView> DetailsView;
};
