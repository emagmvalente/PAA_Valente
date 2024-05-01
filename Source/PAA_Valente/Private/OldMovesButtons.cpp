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
	if (AssociatedString != FString("") && !GameMode->Players[1]->GetThinkingStatus())
	{
		GameMode->CB->Kings[0]->DecolorPossibleMoves();
		Cast<AWhitePlayer>(GameMode->Players[0])->Deselect();
		GameMode->CB->GeneratePositionsFromString(AssociatedString);
		GameMode->CB->SetTilesOwners();
	}
	// If the black player is moving, replay is not allowed
	else if (GameMode->Players[1]->GetThinkingStatus())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Shhh! Black is thinking... Replay later."));
		GameInstance->SetNotificationMessage(TEXT("Shhh! Black is thinking... Replay later."));
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
		if (PieceMoved->IsA<APiecePawn>())
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
	if (PieceToParse->IsA<APieceKing>())
	{
		return 'K';
	}
	else if (PieceToParse->IsA<APieceQueen>())
	{
		return 'Q';
	}
	else if (PieceToParse->IsA<APieceRook>())
	{
		return 'R';
	}
	else if (PieceToParse->IsA<APieceBishop>())
	{
		return 'B';
	}
	else if (PieceToParse->IsA<APieceKnight>())
	{
		return 'N';
	}

	return '\0';
}

FString UOldMovesButtons::LocationParsing(FVector2D& Location)
{
	FString ResultantString = FString("");

	if (Location.Y >= 0 && Location.Y <= 7)
	{
		ResultantString.AppendChar('a' + Location.Y);
	}

	int32 LocationXRoundedToIntPlusOne = FMath::RoundToInt(Location.X) + 1;
	FString XPosition = FString::FromInt(LocationXRoundedToIntPlusOne);

	ResultantString.Append(XPosition);

	return ResultantString;
}
