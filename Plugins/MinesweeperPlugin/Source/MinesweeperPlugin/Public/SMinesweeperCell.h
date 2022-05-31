// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinesweeperDataTypes.h"
#include "Widgets/SCompoundWidget.h"

/**
 *
 */
class MINESWEEPERPLUGIN_API SMinesweeperCell : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnClickedWithCell, TSharedPtr<SMinesweeperCell>)

	SLATE_BEGIN_ARGS(SMinesweeperCell)
	{}

	SLATE_EVENT(FOnClickedWithCell, OnClicked)
	SLATE_ATTRIBUTE(int32, CellIndex)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetState(const FMinesweeperCellState& State);

	int32 GetCellIndex() const { return CellIndex.Get(); }

private:
	void OnCellClicked();

private:
	// OnClick event that returns provides our index.
	FOnClickedWithCell OnClicked;

	TAttribute<int32> CellIndex;
	FMinesweeperCellState CellState;

	TSharedPtr<SButton> CellButton;
	TSharedPtr<STextBlock> CellText;
};