// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinesweeperDataTypes.h"
#include "SMinesweeperCell.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SGridPanel.h"

/**
 *
 */
class MINESWEEPERPLUGIN_API SMinesweeperBoard : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnGameFinished, bool)

	SLATE_BEGIN_ARGS(SMinesweeperBoard)
	{}

	SLATE_EVENT(FOnGameFinished, OnGameFinished)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void GenerateNewBoard(FMinesweeperGameConfig Config);

private:
	int32 WidthAndHeightToIndex(uint8 ColumnIndex, uint8 RowIndex) const;
	void IndexToWidthAndHeight(int32 Index, uint8& OutWidth, uint8& OutHeight) const;

	// Get the neighbors for a cell. This is usually 8, but might be less if its an edge or corner cell.
	TArray<int32> GetCellNeighbors(int32 FromIndex) const;

	void HandleCellClicked(TSharedPtr<SMinesweeperCell> Cell);

	void RecurseRevealCells(int32 FromIndex);

	void EndGame(bool Success);

	// Sync visual board with internal state.
	void UpdateBoard();

private:
	FOnGameFinished OnGameFinished;

	/**The board we are displaying */
	TArray<FMinesweeperCellState> BoardState;

	uint8 GridWidth = 0;
	uint8 GridHeight = 0;

	TSharedPtr<SGridPanel> BoardGridWidget;

	// When this reaches 0 the game is over.
	int32 CellsLeftToReveal = 0;

	TArray<int32> TopEdge;
	TArray<int32> LeftEdge;
	TArray<int32> RightEdge;
	TArray<int32> BottomEdge;
};