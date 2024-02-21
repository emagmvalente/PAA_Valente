// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
class IPlayerInterface;
#include "Chessboard.h"
#include "GameFramework/GameModeBase.h"
#include "ChessGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PAA_VALENTE_API AChessGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	bool bIsGameOver;

	// array of player interfaces
	TArray<IPlayerInterface*> Players;
	int32 CurrentPlayer;

	// TSubclassOf is a template class that provides UClass type safety.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChessboard> CBClass;

	UPROPERTY(VisibleAnywhere)
	AChessboard* CB;

	// field size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	AChessGameMode();

	virtual void BeginPlay() override;


};
