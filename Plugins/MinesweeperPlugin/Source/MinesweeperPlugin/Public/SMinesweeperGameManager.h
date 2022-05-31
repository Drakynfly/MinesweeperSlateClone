// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinesweeperDataTypes.h"
#include "SMinesweeperBoard.h"
#include "Widgets/SCompoundWidget.h"

/**
 *
 */
class MINESWEEPERPLUGIN_API SMinesweeperGameManager : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMinesweeperGameManager)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:

	void SetGridWidth(uint8 NewValue);
	void SetGridHeight(uint8 NewValue);
	void SetMineCount(uint8 NewValue);

	void GenerateNewBoard();
	void HandleGameFinished(bool Success);
private:

	FMinesweeperGameConfig GameConfig;

	TSharedPtr<SMinesweeperBoard> BoardWidget;

	// Text to display "You Win" or "You Lose"
	TSharedPtr<STextBlock> ResultText;

	// @todo, this can technically be higher, but it bugs out at over 63 and wont generate bombs :/
	uint8 MaxGridSize = 50;

	uint8 MaxMines = 99;
};