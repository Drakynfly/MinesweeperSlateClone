// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MinesweeperDataTypes.h"
#include "Widgets/SCompoundWidget.h"

// SButtons don't detect right mouse clicks :(
// So this is a basic child that includes right mouse detection.
class SMineButton : public SButton
{
	DECLARE_DELEGATE_RetVal_OneParam(FReply, FOnClickedWithMouseButton, bool /*IsRightMouseClick*/)

	SLATE_BEGIN_ARGS(SMineButton)
	{}

	SLATE_DEFAULT_SLOT( FArguments, Content )

	SLATE_EVENT(FOnClickedWithMouseButton, OnClickedWithMouseButton)

	SLATE_END_ARGS()

	SMineButton();

	void Construct(const FArguments& InArgs);

protected:
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
	// Index:
	// == 1 - Left Mouse
	// == 2 - Right Mouse
	// == 3 - Touch Event
	// @todo Could be an enum if I spent more then 12 seconds implementing this
	FReply ExecuteOnClickWithInputIndex(uint8 Index);

	FOnClickedWithMouseButton OnClickedWithMouseButton;
};


/**
 * Displays the state of one square.
 */
class SMinesweeperCell : public SCompoundWidget
{
public:
	// Bool for detecting left/right mouse
	DECLARE_DELEGATE_TwoParams(FOnClickedWithCellAndButton, TSharedPtr<SMinesweeperCell>, bool)

	SLATE_BEGIN_ARGS(SMinesweeperCell)
	{}

	SLATE_EVENT(FOnClickedWithCellAndButton, OnClicked)
	SLATE_ATTRIBUTE(int32, CellIndex)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetState(const FMinesweeperCellState& State);

	int32 GetCellIndex() const { return CellIndex.Get(); }

private:
	void OnCellClicked(bool IsRightMouse);

private:
	// OnClick event that returns provides our index.
	FOnClickedWithCellAndButton OnClicked;

	TAttribute<int32> CellIndex;
	FMinesweeperCellState CellState;

	TSharedPtr<STextBlock> CellText;
};