// Fill out your copyright notice in the Description page of Project Settings.


#include "OldMovesButtons.h"
#include "Components/TextBlock.h"
#include "Piece.h"
#include "PieceKing.h"
#include "PieceQueen.h"
#include "PieceRook.h"
#include "PieceBishop.h"
#include "PiecePawn.h"
#include "PieceKnight.h"

UOldMovesButtons::UOldMovesButtons()
{
	AssociatedString = FString("");
}

void UOldMovesButtons::ButtonOnClickFunction()
{
	// Recreating the chessboard in its old state
	if (AssociatedString != FString("") && !GameMode->bIsBlackThinking)
	{
		if (CPC->SelectedPieceToMove)
		{
			CPC->SelectedPieceToMove->DecolorPossibleMoves();
		}
		GameMode->CB->GeneratePositionsFromString(AssociatedString);
		GameMode->CB->SetTilesOwners();
		GameMode->SetKings();
	}
	// If the black player is moving, replay is not allowed
	else if (GameMode->bIsBlackThinking)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Shhh! Black is thinking... Replay later."));
	}
}

void UOldMovesButtons::CreateText()
{
	UTextBlock* MoveDone = NewObject<UTextBlock>(this);

	FString TextToPutOnButton = "";
	if (PieceMoved)
	{
		TextToPutOnButton.AppendChar(PieceParsing(PieceMoved));
	}
	if (PieceCaptured)
	{
		TextToPutOnButton.AppendChar(PieceParsing(PieceCaptured));
	}
	TextToPutOnButton.Append(LocationParsing(NewPosition));

	MoveDone->SetText(FText::FromString(TextToPutOnButton));
	MoveDone->SetColorAndOpacity(FLinearColor::Black);

	AddChild(MoveDone);
}

TCHAR UOldMovesButtons::PieceParsing(APiece* PieceToParse)
{
	if (Cast<APieceKing>(PieceToParse))
	{
		return 'K';
	}
	else if (Cast<APieceQueen>(PieceToParse))
	{
		return 'Q';
	}
	else if (Cast<APieceRook>(PieceToParse))
	{
		return 'R';
	}
	else if (Cast<APieceBishop>(PieceToParse))
	{
		return 'B';
	}
	else if (Cast<APieceKnight>(PieceToParse))
	{
		return 'N';
	}

	return '\0';
}

FString UOldMovesButtons::LocationParsing(FVector& Location)
{
	FString ResultantString = FString("");

	if (Location.X >= 0 && Location.X <= 7)
	{
		ResultantString.AppendChar('a' + Location.X);
	}

	int32 LocationYRoundedToIntPlusOne = FMath::RoundToInt(Location.Y) + 1;
	FString YPosition = FString::FromInt(LocationYRoundedToIntPlusOne);

	ResultantString.Append(YPosition);

	return ResultantString;
}
