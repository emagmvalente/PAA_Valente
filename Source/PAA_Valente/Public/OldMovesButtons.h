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

public:
	FString AssociatedString;
	UTextBlock* MoveDone;

	AChessGameMode* GameMode;
	AChessPlayerController* CPC;

	APiece* PieceMoved;
	bool bItWasACapture = false;
	FVector2D NewPosition;

	bool bIsCheckmate = false;

	// Only for Pawn's captures
	FVector2D OldPosition;

	UOldMovesButtons(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void ButtonOnClickFunction();

	void CreateText();
	TCHAR PieceParsing(APiece* PieceToParse);
	FString LocationParsing(FVector2D& Location);
};
