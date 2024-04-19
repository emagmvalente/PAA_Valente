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

UOldMovesButtons::UOldMovesButtons(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AssociatedString = FString("");
	MoveDone = ObjectInitializer.CreateDefaultSubobject<UTextBlock>(this, TEXT("MoveDone"));
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
	}
	// If the black player is moving, replay is not allowed
	else if (GameMode->bIsBlackThinking)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Shhh! Black is thinking... Replay later."));
	}
}

void UOldMovesButtons::SetAssociatedString(FString& AssociatedStringToPrint)
{
	AssociatedString = AssociatedStringToPrint;
}

void UOldMovesButtons::CreateText(APiece* PieceMoved, bool bItWasACapture, FVector2D NewPosition, FVector2D OldPosition)
{
	FString TextToPutOnButton = "";
	if (PieceMoved)
	{
		TextToPutOnButton.AppendChar(PieceParsing(PieceMoved));
	}
	if (bItWasACapture)
	{
		// In algebraic notation, pawn doesn't have a letter, so it will be used his location
		if (Cast<APiecePawn>(PieceMoved))
		{
			FString OldPositionString = LocationParsing(OldPosition);
			TCHAR OldPositionChar = OldPositionString[0];
			TextToPutOnButton.AppendChar(OldPositionChar);

		}
		TextToPutOnButton.AppendChar('x');
	}
	TextToPutOnButton.Append(LocationParsing(NewPosition));

	// Add special chars
	if (GameMode->VerifyCheck() && !GameMode->VerifyCheckmate())
	{
		TextToPutOnButton.AppendChar('+');
	}
	else if (GameMode->VerifyCheck() && GameMode->VerifyCheckmate())
	{
		TextToPutOnButton.AppendChar('#');
	}

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

FString UOldMovesButtons::LocationParsing(FVector2D& Location)
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
