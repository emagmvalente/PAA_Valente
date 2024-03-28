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
	else if (GameMode->bIsBlackThinking)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Shhh! Black is thinking... Replay later."));
	}
}

void UOldMovesButtons::CreateText()
{
	UTextBlock* MoveDone = NewObject<UTextBlock>(this);

	TCHAR PieceMovedChar = '\0';
	if (PieceMoved)
	{
		PieceMovedChar = PieceParsing(PieceMoved);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("AHAHAHAH"));
	}
	TCHAR PieceCapturedChar = '\0';
	if (PieceCaptured)
	{
		PieceCapturedChar = PieceParsing(PieceCaptured);
	}
	FString NewPositionString = LocationParsing(NewPosition);

	FString TextToPutOnButton = "";
	TextToPutOnButton.AppendChar(PieceMovedChar);
	TextToPutOnButton.AppendChar(PieceCapturedChar);
	TextToPutOnButton.Append(NewPositionString);

	MoveDone->SetText(FText::FromString(TextToPutOnButton));

	AddChild(MoveDone);
}

TCHAR UOldMovesButtons::PieceParsing(APiece* PieceToParse)
{
	TCHAR ResultantChar = '\0';

	if (Cast<APieceKing>(PieceToParse))
	{
		ResultantChar = 'K';
	}
	else if (Cast<APieceQueen>(PieceToParse))
	{
		ResultantChar = 'Q';
	}
	else if (Cast<APieceRook>(PieceToParse))
	{
		ResultantChar = 'R';
	}
	else if (Cast<APieceBishop>(PieceToParse))
	{
		ResultantChar = 'B';
	}
	else if (Cast<APieceKnight>(PieceToParse))
	{
		ResultantChar = 'N';
	}

	if (PieceToParse->Color == EColor::B)
	{
		return FChar::ToLower(ResultantChar);
	}
	return ResultantChar;
}

FString UOldMovesButtons::LocationParsing(FVector& Location)
{
	FString ResultantString = FString("");

	if (Location.X >= 0 && Location.X <= 7)
	{
		ResultantString.AppendChar('a' + Location.X);
	}

	FString YPosition = FString::SanitizeFloat(Location.Y);
	ResultantString.Append(YPosition);

	return ResultantString;
}
