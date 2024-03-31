// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ChessGameInstance.h"
#include "ChessGameMode.h"
#include "Piece.h"
#include "Kismet/GameplayStatics.h"
#include "BlackRandomPlayer.generated.h"

UCLASS()
class PAA_VALENTE_API ABlackRandomPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABlackRandomPlayer();
	UChessGameInstance* GameInstance;

	int32 PlayerNumber;

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

};
