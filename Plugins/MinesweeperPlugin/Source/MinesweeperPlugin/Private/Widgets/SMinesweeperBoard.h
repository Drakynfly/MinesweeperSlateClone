// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MinesweeperDataTypes.h"
#include "SMinesweeperCell.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SGridPanel.h"

/**
 * A minesweeper game board. Notifies external widget of game progress via GameStarted/GameFinished events
 */
class SMinesweeperBoard : public SCompoundWidget
{
public:
	DECLARE_DELEGATE(FOnGameStarted)
	DECLARE_DELEGATE_OneParam(FOnGameFinished, bool)

	SLATE_BEGIN_ARGS(SMinesweeperBoard)
	{}

	SLATE_EVENT(FOnGameStarted, OnGameStarted)
	SLATE_EVENT(FOnGameFinished, OnGameFinished)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	/** Must be called to init new game */
	void GenerateNewBoard(FMinesweeperGameConfig Config);

private:
	// UTILS

	// Ravelling function to convert X,Y to array index.
	int32 WidthAndHeightToIndex(uint8 ColumnIndex, uint8 RowIndex) const;

	// Unravelling function. Not actually used, but included for sanity, does the opposite of the above.
	void IndexToWidthAndHeight(int32 Index, uint8& OutWidth, uint8& OutHeight) const;

	// Get the neighbors for a cell. This is usually 8, but might be less if its an edge or corner cell.
	TArray<int32> GetCellNeighbors(int32 FromIndex) const;


	// GAMEPLAY

	void HandleCellClicked(TSharedPtr<SMinesweeperCell> Cell, bool IsRightMouse);

	void RecurseRevealCells(int32 FromIndex);

	void EndGame(bool Success);

	// Sync visual board with internal state.
	void UpdateBoard();

private:
	FOnGameStarted OnGameStarted;
	FOnGameFinished OnGameFinished;

	/** Waits until first reveal to start game */
	bool HasGameStarted = false;

	/** The board we are displaying */
	TArray<FMinesweeperCellState> BoardState;

	uint8 GridWidth = 0;
	uint8 GridHeight = 0;

	TSharedPtr<SGridPanel> BoardGridWidget;

	/** When this reaches 0 the game is over. */
	int32 CellsLeftToReveal = 0;

	/** We cache which cells are on the edges at game start for easier lookup. */
	TArray<int32> TopEdge;
	TArray<int32> LeftEdge;
	TArray<int32> RightEdge;
	TArray<int32> BottomEdge;
};