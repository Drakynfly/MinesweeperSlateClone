// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "SMinesweeperGameManager.h"
#include "SlateOptMacros.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SScrollBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMinesweeperGameManager::Construct(const FArguments& InArgs)
{
	const FText TitleText = FText::FromString(TEXT("Minesweeper"));

	const FText WidthButtonText = FText::FromString(TEXT("Width"));
	const FText HeightButtonText = FText::FromString(TEXT("Height"));
	const FText NumMinesText = FText::FromString(TEXT("Mines"));
	const FText GenerateButtonText = FText::FromString(TEXT("New Board"));

	constexpr int32 NumericInputBoxWidth = 25;

	ChildSlot
	[
		// Allow large boards to be scrolled into view.
		SNew(SScrollBox)
		+ SScrollBox::Slot()
			[
			SNew(SVerticalBox)
				// Title Text
				+ SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.AutoHeight()
					.Padding(15)
					[
						SNew(STextBlock).Text(TitleText)
					]
				// Config Editors
				+ SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.AutoHeight()
					.Padding(50, 0)
					[
						// Width Editor
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.Padding(5)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Right)
						.AutoWidth()
						[
							SNew(STextBlock).Text(WidthButtonText)
						]
						+ SHorizontalBox::Slot()
						[
							SNew(SNumericEntryBox<uint8>)
							.MinDesiredValueWidth(NumericInputBoxWidth)
							.EditableTextBoxStyle(FEditorStyle::Get(), "Graph.EditableTextBox")
							.BorderForegroundColor(FSlateColor::UseForeground())
							.Value_Lambda([this]()
							{
								return GameConfig.GridWidth;
							})
							.OnValueCommitted_Lambda([this](const uint8 NewValue, ETextCommit::Type)
							{
								SetGridWidth(NewValue);
							})
						]
						// Height Editor
						+ SHorizontalBox::Slot()
						.Padding(5)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Right)
						.AutoWidth()
						[
							SNew(STextBlock).Text(HeightButtonText)
						]
						+ SHorizontalBox::Slot()
						[
							SNew(SNumericEntryBox<uint8>)
							.MinDesiredValueWidth(NumericInputBoxWidth)
							.EditableTextBoxStyle(FEditorStyle::Get(), "Graph.EditableTextBox")
							.BorderForegroundColor(FSlateColor::UseForeground())
							.Value_Lambda([this]()
							{
								return GameConfig.GridHeight;
							})
							.OnValueCommitted_Lambda([this](const uint8 NewValue, ETextCommit::Type)
							{
								SetGridHeight(NewValue);
							})
						]
						// Mine Editor
						+ SHorizontalBox::Slot()
						.Padding(5)
						.HAlign(HAlign_Right)
						.VAlign(VAlign_Center)
						.AutoWidth()
						[
							SNew(STextBlock).Text(NumMinesText)
						]
						+ SHorizontalBox::Slot()
						[
							SNew(SNumericEntryBox<uint8>)
							.MinDesiredValueWidth(NumericInputBoxWidth)
							.EditableTextBoxStyle(FEditorStyle::Get(), "Graph.EditableTextBox")
							.BorderForegroundColor(FSlateColor::UseForeground())
							.Value_Lambda([this]()
							{
								return GameConfig.MineCount;
							})
							.OnValueCommitted_Lambda([this](const uint8 NewValue, ETextCommit::Type)
							{
								SetMineCount(NewValue);
							})
						]
					]
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(15)
				[
					SNew(SHorizontalBox)
					// New Board Button
					+ SHorizontalBox::Slot()
					.Padding(10)
					.AutoWidth()
					[
						SNew(SButton)
						.Text(GenerateButtonText)
						.OnClicked_Lambda([this]()
						{
						GenerateNewBoard();
						return FReply::Handled();
						})
					]
					// Timer
					+ SHorizontalBox::Slot()
					.Padding(10)
					.AutoWidth()
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text_Lambda([this] () { return GetTimerText(); })
						.ColorAndOpacity(FSlateColor(FColor::Red))
					]
				]
				// Board
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SAssignNew(BoardWidget, SMinesweeperBoard)
						.OnGameStarted(this, &SMinesweeperGameManager::HandleGameStarted)
						.OnGameFinished(this, &SMinesweeperGameManager::HandleGameFinished)
				]
				// Result Text
				+ SVerticalBox::Slot()
				.Padding(15)
				.HAlign(HAlign_Center)
				[
					SAssignNew(ResultText, STextBlock)
				]
			]
		];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMinesweeperGameManager::SetGridWidth(const uint8 NewValue)
{
	GameConfig.GridWidth = FMath::Clamp<uint8>(NewValue, 0, MaxGridSize);
}

void SMinesweeperGameManager::SetGridHeight(const uint8 NewValue)
{
	GameConfig.GridHeight = FMath::Clamp<uint8>(NewValue, 0, MaxGridSize);
}

void SMinesweeperGameManager::SetMineCount(const uint8 NewValue)
{
	GameConfig.MineCount = FMath::Clamp<uint8>(NewValue, 0, MaxMines);
}

void SMinesweeperGameManager::GenerateNewBoard()
{
	// Clear results from previous game, and restart;
	ResultText->SetText(FText());
	BoardWidget->GenerateNewBoard(GameConfig);
	IsGameRunning = false;

	// Reset clock to 0 when a new board is made to clear previous game time.
	TimeGameStarted = FDateTime::Now();
	GameTimer = FDateTime::Now();
}

void SMinesweeperGameManager::HandleGameStarted()
{
	IsGameRunning = true;

	// Reset clock to 0 *again* when the game actually starts.
	TimeGameStarted = FDateTime::Now();
	GameTimer = FDateTime::Now();
}

void SMinesweeperGameManager::HandleGameFinished(const bool Success)
{
	static const FText WinningTest = FText::FromString("You Won!");
	static const FText LosingTest = FText::FromString("You Lost!");

	IsGameRunning = false;

	if (ResultText.IsValid())
	{
		if (Success)
		{
			ResultText->SetText(WinningTest);
			ResultText->SetColorAndOpacity(FSlateColor(FColor::Green));
		}
		else
		{
			ResultText->SetText(LosingTest);
			ResultText->SetColorAndOpacity(FSlateColor(FColor::Red));
		}
	}
}

FText SMinesweeperGameManager::GetTimerText()
{
	// While the game is running, update the timer.
	if (IsGameRunning)
	{
		GameTimer = FDateTime::Now();
	}

	return FText::FromString((GameTimer - TimeGameStarted).ToString(TEXT("%h:%m:%s:%f")).RightChop(1));
}