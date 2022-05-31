// Fill out your copyright notice in the Description page of Project Settings.

#include "SMinesweeperCell.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMinesweeperCell::Construct(const FArguments& InArgs)
{
	CellIndex = InArgs._CellIndex;
	OnClicked = InArgs._OnClicked;

	CellText = SNew(STextBlock).MinDesiredWidth(10);

	ChildSlot
	[
		SAssignNew(CellButton, SButton)
			.IsEnabled_Lambda([this]()
			{
				return !CellState.Open;
			})
			.OnClicked_Lambda([this]()
			{
				OnCellClicked();
				return FReply::Handled();
			})
		[
			CellText.ToSharedRef()
		]
	];
}

void SMinesweeperCell::SetState(const FMinesweeperCellState& State)
{
	CellState = State;

	FText CellDebugText = FText();

	if (State.Open)
	{
		if (State.IsMine)
		{
			CellDebugText = FText::FromString("M");
		}
		else //if (State.NumberOfNearbyMines > 0)
		{
			CellDebugText = FText::FromString(FString::FromInt(State.NumberOfNearbyMines));
		}
	}

	if (CellText.IsValid())
	{
		CellText->SetText(CellDebugText);
	}
}

void SMinesweeperCell::OnCellClicked()
{
	OnClicked.ExecuteIfBound(SharedThis(this));
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION