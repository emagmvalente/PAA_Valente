// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chessboard.h"
#include "GameFramework/GameModeBase.h"
#include "ChessGameMode.generated.h"

class IPlayerInterface;

/**
 * 
 */
UCLASS()
class PAA_VALENTE_API AChessGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	bool bIsGameOver;
	bool bIsWhiteOnCheck;
	bool bIsBlackOnCheck;

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

	void SetKings();
	void VerifyCheck(APiece* Piece);

	// called at the end of the game turn
	void TurnPlayer(IPlayerInterface* Player);

};
