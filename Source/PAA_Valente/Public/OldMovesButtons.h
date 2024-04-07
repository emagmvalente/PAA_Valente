// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ChessGameMode.h"
#include "ChessPlayerController.h"
#include "OldMovesButtons.generated.h"

/**
 * 
 */
UCLASS()
class PAA_VALENTE_API UOldMovesButtons : public UButton
{
	GENERATED_BODY()

	UTextBlock* MoveDone;
	APiece* PieceMoved;
	bool bItWasACapture;
	FVector2D NewPosition;

	// Only for Pawn's captures
	FVector2D OldPosition;

	FString AssociatedString;

public:

	AChessGameMode* GameMode;
	AChessPlayerController* CPC;

	UOldMovesButtons(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void ButtonOnClickFunction();

	void SetAssociatedString(FString& AssociatedStringToPrint);
	void SetNewLocationToPrint(FVector2D NewLocationToPrint);
	void SetPieceToPrint(APiece* PieceToPrint);
	void SetOldLocationToPrint(FVector2D OldLocationToPrint);
	void SetItWasACapture(bool ItWasACapture);

	void CreateText();
	TCHAR PieceParsing(APiece* PieceToParse);
	FString LocationParsing(FVector2D& Location);
};
