// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MinesweeperDataTypes.h"
#include "SMinesweeperBoard.h"
#include "Widgets/SCompoundWidget.h"

/**
 * High level management of the game. Houses all controls and can stop/start new games.
 */
class SMinesweeperGameManager : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMinesweeperGameManager)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:

	// Game config
	void SetGridWidth(uint8 NewValue);
	void SetGridHeight(uint8 NewValue);
	void SetMineCount(uint8 NewValue);

	// Game control
	void GenerateNewBoard();
	void HandleGameStarted();
	void HandleGameFinished(bool Success);

	// Pseudo-tick used to update game time.
	FText GetTimerText();
private:

	FMinesweeperGameConfig GameConfig;

	TSharedPtr<SMinesweeperBoard> BoardWidget;

	// Text to display "You Win" or "You Lose"
	TSharedPtr<STextBlock> ResultText;

	// @todo, this can technically be higher, but it bugs out at over 63 and wont generate bombs :/
	uint8 MaxGridSize = 30;

	uint8 MaxMines = 99;

	bool IsGameRunning = false;
	FDateTime TimeGameStarted;
	FDateTime GameTimer;
};