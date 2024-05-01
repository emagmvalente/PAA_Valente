// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ChessGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PAA_VALENTE_API UChessGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Score value for human player
	UPROPERTY(EditAnywhere)
	int32 ScoreHumanPlayer = 0;

	// Score value for AI player
	UPROPERTY(EditAnywhere)
	int32 ScoreAiPlayer = 0;

	// Message to show every turn
	UPROPERTY(EditAnywhere)
	FString CurrentTurnMessage = "Current Player";

	FString NotificationMessage = "";

	// Increment the score for human player
	void IncrementScoreHumanPlayer();

	// Increment the score for AI player
	void IncrementScoreAiPlayer();

	// Get the score for human player
	UFUNCTION(BlueprintCallable)
	int32 GetScoreHumanPlayer();

	// Get the score for AI player
	UFUNCTION(BlueprintCallable)
	int32 GetScoreAiPlayer();

	// Get the current turn message
	UFUNCTION(BlueprintCallable)
	FString GetTurnMessage();

	// Get a notification message
	UFUNCTION(BlueprintCallable)
	FString GetNotificationMessage();

	// Set the turn message
	UFUNCTION(BlueprintCallable)
	void SetTurnMessage(FString Message);

	// Set the wait message
	UFUNCTION(BlueprintCallable)
	void SetNotificationMessage(FString Message);

};

