// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

struct FMinesweeperGameConfig
{
	FMinesweeperGameConfig()
	{
		// Default values are set to typical "beginner" board;
		GridWidth = 8;
		GridHeight = 8;
		MineCount = 10;
	}

	uint8 GridWidth;
	uint8 GridHeight;
	uint8 MineCount;
};

struct FMinesweeperCellState
{
	FMinesweeperCellState() {}

	// Is the cell "revealed"
	bool Open = false;

	bool IsMine = false;

	bool FlaggedAsMine = false;

	int32 NumberOfNearbyMines = 0;
};