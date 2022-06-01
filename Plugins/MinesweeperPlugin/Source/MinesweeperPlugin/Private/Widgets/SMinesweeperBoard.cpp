// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "SMinesweeperBoard.h"
#include "SlateOptMacros.h"
#include "SMinesweeperCell.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMinesweeperBoard::Construct(const FArguments& InArgs)
{
	OnGameStarted = InArgs._OnGameStarted;
	OnGameFinished = InArgs._OnGameFinished;

	BoardGridWidget = SNew(SGridPanel);

	ChildSlot
	[
		BoardGridWidget.ToSharedRef()
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMinesweeperBoard::GenerateNewBoard(const FMinesweeperGameConfig Config)
{
	// Step 0: Clear Previous Game Data
	TopEdge.Empty();
	BottomEdge.Empty();
	LeftEdge.Empty();
	RightEdge.Empty();
	HasGameStarted = false;

	// Step 1: Initialize our board with the new number of cells, all in their default state.
	GridWidth = Config.GridWidth;
	GridHeight = Config.GridHeight;
	const int32 NewBoardCellCount = GridWidth * GridHeight;

	for (int32 i = 0; i < GridWidth; ++i)
	{
		TopEdge.Add(i);
		BottomEdge.Add(NewBoardCellCount - i - 1);
	}

	for (int32 i = 0; i < GridHeight; ++i)
	{
		LeftEdge.Add(i * GridWidth);
		RightEdge.Add(((i+1) * GridWidth)-1);
	}

	BoardState.Init(FMinesweeperCellState(), NewBoardCellCount);

	// Step 2: Choose cells to promote to Mines
	const uint8 NumMinesToMake = FMath::Clamp<uint8>(Config.MineCount, 0, NewBoardCellCount);

	uint8 NumMinesMade = 0;

	while (NumMinesMade < NumMinesToMake)
	{
		// Grab a random cell and check if its not a mine.
		FMinesweeperCellState& PossiblePromotion = BoardState[FMath::RandRange(0, BoardState.Num()-1)];

		if (!PossiblePromotion.IsMine)
		{
			// Make it a mine
			PossiblePromotion.IsMine = true;
			NumMinesMade++;
		}
	}

	// Step 3: Tell every cell how many mines they are next too
	for (int32 i = 0; i < BoardState.Num(); ++i)
	{
		FMinesweeperCellState& BoardCell = BoardState[i];

		TArray<int32> Neighbors = GetCellNeighbors(i);

		for (const int32 Neighbor : Neighbors)
		{
			const FMinesweeperCellState& NeighborState = BoardState[Neighbor];

			if (NeighborState.IsMine)
			{
				BoardCell.NumberOfNearbyMines++;
			}
		}
	}

	// Set the number of cells that must be revealed to win.
	CellsLeftToReveal = NewBoardCellCount - NumMinesMade;

	// Step 4: Generate board widgets
	BoardGridWidget->ClearChildren();

	for (int32 ColumnIndex = 0; ColumnIndex < GridWidth; ColumnIndex++)
	{
		for (int32 RowIndex = 0; RowIndex < GridHeight; RowIndex++)
		{
			const int32 CellIndex = WidthAndHeightToIndex(ColumnIndex, RowIndex);

			TSharedRef<SMinesweeperCell> NewCell =
					SNew(SMinesweeperCell)
					.CellIndex(CellIndex)
					.OnClicked(this, &SMinesweeperBoard::HandleCellClicked);

			NewCell->SetState(BoardState[CellIndex]);

			BoardGridWidget->AddSlot(ColumnIndex, RowIndex)
			[
				NewCell
			];
		}
	}
}

int32 SMinesweeperBoard::WidthAndHeightToIndex(const uint8 ColumnIndex, const uint8 RowIndex) const
{
	return (RowIndex * GridWidth) + ColumnIndex;
}

void SMinesweeperBoard::IndexToWidthAndHeight(const int32 Index, uint8& OutWidth, uint8& OutHeight) const
{
	OutHeight = Index % GridWidth;
	OutWidth = Index - OutHeight;
}

TArray<int32> SMinesweeperBoard::GetCellNeighbors(const int32 FromIndex) const
{
	// This entire function is certainly not the best way to do this, but it's the most straight-forward and readable
	// way. I could probably make a "more efficient" version if i tried, but it would certainly be nigh unreadable.

	const bool IsTopEdge = TopEdge.Contains(FromIndex);
	const bool IsBottomEdge = BottomEdge.Contains(FromIndex);
	const bool IsRightEdge = RightEdge.Contains(FromIndex);
	const bool IsLeftEdge = LeftEdge.Contains(FromIndex);

	TArray<int32> OutNeighbors;

	const int32 SignedGridWidth = GridWidth;

	if (!IsTopEdge)
	{
		OutNeighbors.Add(FromIndex - SignedGridWidth);

		if (!IsLeftEdge)
		{
			OutNeighbors.Add(FromIndex - SignedGridWidth - 1);
		}

		if (!IsRightEdge)
		{
			OutNeighbors.Add(FromIndex - SignedGridWidth + 1);
		}
	}

	if (!IsLeftEdge)
	{
		OutNeighbors.Add(FromIndex - 1);
	}

	if (!IsRightEdge)
	{
		OutNeighbors.Add(FromIndex + 1);
	}

	if (!IsBottomEdge)
	{
		OutNeighbors.Add(FromIndex + SignedGridWidth);

		if (!IsLeftEdge)
		{
			OutNeighbors.Add(FromIndex + SignedGridWidth - 1);
		}

		if (!IsRightEdge)
		{
			OutNeighbors.Add(FromIndex + SignedGridWidth + 1);
		}
	}

	return OutNeighbors;
}

void SMinesweeperBoard::HandleCellClicked(const TSharedPtr<SMinesweeperCell> Cell, const bool IsRightMouse)
{
	const int32 CellIndex = Cell->GetCellIndex();

	if (!BoardState.IsValidIndex(CellIndex))
	{
		return;
	}

	if (!HasGameStarted)
	{
		HasGameStarted = true;
		// ReSharper disable once CppExpressionWithoutSideEffects
		OnGameStarted.ExecuteIfBound();
	}

	FMinesweeperCellState& ClickedCellState = BoardState[CellIndex];

	if (IsRightMouse)
	{
		// Mark cell as flagged, or un-flag, if currently flagged.
		ClickedCellState.FlaggedAsMine = !ClickedCellState.FlaggedAsMine;
	}
	else
	{
		// If the cell was a mine, end the game
		if (ClickedCellState.IsMine)
		{
			EndGame(false);
		}
		// Otherwise, calculate new cell states.
		else
		{
			RecurseRevealCells(CellIndex);
		}
	}


	Cell->SetState(ClickedCellState);
}

void SMinesweeperBoard::RecurseRevealCells(const int32 FromIndex)
{
	TArray<int32> CellsToReveal = {FromIndex};
	TArray<int32> RevealedCells;

	while (CellsToReveal.Num() > 0)
	{
		const int32 ThisCell = CellsToReveal[0];
		FMinesweeperCellState& CellState = BoardState[ThisCell];
		CellsToReveal.RemoveAt(0);

		if (!CellState.Open)
		{
			CellState.Open = true;
			CellsLeftToReveal--;

			// Check if we won
			if (CellsLeftToReveal == 0)
			{
				// We won the game.
				EndGame(true);
				return;
			}
		}

		// Reveal all unrevealed neighbors of cells with 0 nearby mines.
		if (CellState.NumberOfNearbyMines == 0)
		{
			TArray<int32> NewCells = GetCellNeighbors(ThisCell);

			for (int32 NewCell : NewCells)
			{
				if (!RevealedCells.Contains(NewCell))
				{
					CellsToReveal.Add(NewCell);
					RevealedCells.Add(NewCell);
				}
			}
		}
	}

	// Push changes to the board state to the visual board
	UpdateBoard();
}

void SMinesweeperBoard::EndGame(const bool Success)
{
	// Reveal all cells when the game is over
	for (FMinesweeperCellState& BoardCell : BoardState)
	{
		BoardCell.Open = true;
	}

	UpdateBoard();

	// ReSharper disable once CppExpressionWithoutSideEffects
	OnGameFinished.ExecuteIfBound(Success);
}

void SMinesweeperBoard::UpdateBoard()
{
	FChildren* ChildrenPointer = BoardGridWidget->GetChildren();

	for (int32 i = 0; i < ChildrenPointer->Num(); ++i)
	{
		const TSharedRef<SMinesweeperCell> Cell = StaticCastSharedRef<SMinesweeperCell>(ChildrenPointer->GetChildAt(i));
		Cell->SetState(BoardState[Cell->GetCellIndex()]);
	}
}