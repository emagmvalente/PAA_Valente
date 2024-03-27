// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
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

	AChessGameMode* GameMode;
	AChessPlayerController* CPC;

	UOldMovesButtons();

	UFUNCTION()
	void ButtonOnClickFunction();
};
