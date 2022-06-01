// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "SMinesweeperCell.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SMineButton::SMineButton()
{
}

void SMineButton::Construct(const FArguments& InArgs)
{
	OnClickedWithMouseButton = InArgs._OnClickedWithMouseButton;

	SButton::Construct(SButton::FArguments()[InArgs._Content.Widget]);
}

FReply SMineButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	if (IsEnabled() &&
		(MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton
			|| MouseEvent.GetEffectingButton() == EKeys::RightMouseButton
			|| MouseEvent.IsTouchEvent()))
	{
		Press();
		PressedScreenSpacePosition = MouseEvent.GetScreenSpacePosition();

		EButtonClickMethod::Type InputClickMethod = GetClickMethodFromInputType(MouseEvent);

		if(InputClickMethod == EButtonClickMethod::MouseDown)
		{
			//get the reply from the execute function
			Reply = ExecuteOnClick();

			//You should ALWAYS handle the OnClicked event.
			ensure(Reply.IsEventHandled() == true);
		}
		else if (InputClickMethod == EButtonClickMethod::PreciseClick)
		{
			// do not capture the pointer for precise taps or clicks
			//
			Reply = FReply::Handled();
		}
		else
		{
			//we need to capture the mouse for MouseUp events
			Reply = FReply::Handled().CaptureMouse( AsShared() );
		}
	}

	Invalidate(EInvalidateWidget::Layout);

	//return the constructed reply
	return Reply;
}

FReply SMineButton::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	const EButtonClickMethod::Type InputClickMethod = GetClickMethodFromInputType(MouseEvent);
	const bool bMustBePressed = InputClickMethod == EButtonClickMethod::DownAndUp || InputClickMethod == EButtonClickMethod::PreciseClick;
	const bool bMeetsPressedRequirements = (!bMustBePressed || (bIsPressed && bMustBePressed));

	uint8 InputIndex = 0;
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		InputIndex = 1;
	}
	else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		InputIndex = 2;
	}
	else if (MouseEvent.IsTouchEvent())
	{
		InputIndex = 3;
	}

	if (bMeetsPressedRequirements &&
		( InputIndex > 0))
	{
		Release();

		if ( IsEnabled() )
		{
			if (InputClickMethod == EButtonClickMethod::MouseDown )
			{
				// NOTE: If we're configured to click on mouse-down/precise-tap, then we never capture the mouse thus
				//       may never receive an OnMouseButtonUp() call.  We make sure that our bIsPressed
				//       state is reset by overriding OnMouseLeave().
			}
			else
			{
				bool bEventOverButton = IsHovered();

				if ( !bEventOverButton && MouseEvent.IsTouchEvent() )
				{
					bEventOverButton = MyGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition());
				}

				if ( bEventOverButton )
				{
					// If we asked for a precise tap, all we need is for the user to have not moved their pointer very far.
					const bool bTriggerForTouchEvent = InputClickMethod == EButtonClickMethod::PreciseClick;

					// If we were asked to allow the button to be clicked on mouse up, regardless of whether the user
					// pressed the button down first, then we'll allow the click to proceed without an active capture
					const bool bTriggerForMouseEvent = (InputClickMethod == EButtonClickMethod::MouseUp || HasMouseCapture() );

					if ( ( bTriggerForTouchEvent || bTriggerForMouseEvent ) )
					{
						Reply = ExecuteOnClickWithInputIndex(InputIndex);
					}
				}
			}
		}

		//If the user of the button didn't handle this click, then the button's
		//default behavior handles it.
		if ( Reply.IsEventHandled() == false )
		{
			Reply = FReply::Handled();
		}
	}

	//If the user hasn't requested a new mouse captor and the button still has mouse capture,
	//then the default behavior of the button is to release mouse capture.
	if ( Reply.GetMouseCaptor().IsValid() == false && HasMouseCapture() )
	{
		Reply.ReleaseMouseCapture();
	}

	Invalidate(EInvalidateWidget::Layout);

	return Reply;
}

FReply SMineButton::ExecuteOnClickWithInputIndex(const uint8 Index)
{
	if (OnClickedWithMouseButton.IsBound())
	{
		// If Index is equal to 2, then pass true for IsRightMouseClick
		FReply Reply = OnClickedWithMouseButton.Execute(Index == 2);
#if WITH_ACCESSIBILITY
		FSlateApplicationBase::Get().GetAccessibleMessageHandler()->OnWidgetEventRaised(AsShared(), EAccessibleEvent::Activate);
#endif
		return Reply;
	}
	else
	{
		return FReply::Handled();
	}
}

void SMinesweeperCell::Construct(const FArguments& InArgs)
{
	CellIndex = InArgs._CellIndex;
	OnClicked = InArgs._OnClicked;

	CellText = SNew(STextBlock).MinDesiredWidth(10);

	ChildSlot
	[
		SNew(SMineButton)
			.IsEnabled_Lambda([this]()
			{
				return !CellState.Open;
			})
			.OnClickedWithMouseButton_Lambda([this](const bool IsRightMouse)
			//.OnClicked_Lambda([this]()
			{
				OnCellClicked(IsRightMouse);
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
			CellText->SetColorAndOpacity(FSlateColor(FColor::Black));
		}
		else if (State.NumberOfNearbyMines > 0)
		{
			CellDebugText = FText::FromString(FString::FromInt(State.NumberOfNearbyMines));

			// I tried setting the text color to what color the numbers are suppose to be but since the buttons
			// get disabled, they become too washed out and unreadable, so I didn't bother to fix it since the proper
			// solution would be to use textures and/or better handling for disabling buttons without grey-ing out
			// the button entirely, but since visuals don't matter for this test, idk, black it is.
			CellText->SetColorAndOpacity(FSlateColor(FColor::Black));
		}
	}
	else if (State.FlaggedAsMine)
	{
		CellDebugText = FText::FromString("F");
		CellText->SetColorAndOpacity(FSlateColor(FColor::Red));
	}

	if (CellText.IsValid())
	{
		CellText->SetText(CellDebugText);
	}
}

void SMinesweeperCell::OnCellClicked(const bool IsRightMouse)
{
	OnClicked.ExecuteIfBound(SharedThis(this), IsRightMouse);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION