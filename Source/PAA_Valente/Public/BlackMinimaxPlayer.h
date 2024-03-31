// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ChessGameInstance.h"
#include "ChessGameMode.h"
#include "Piece.h"
#include "Kismet/GameplayStatics.h"
#include "BlackMinimaxPlayer.generated.h"

UCLASS()
class PAA_VALENTE_API ABlackMinimaxPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABlackMinimaxPlayer();
	UChessGameInstance* GameInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bIsACapture = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnTurn();
	void OnWin();

	int32 EvaluateGrid(TMap<FVector2D, ATile*>& Board);
	int32 MiniMax(TMap<FVector2D, ATile*>& Board, int32 Depth, bool IsMax);
	FVector2D FindBestMove(TMap<FVector2D, ATile*>& Board);
};
