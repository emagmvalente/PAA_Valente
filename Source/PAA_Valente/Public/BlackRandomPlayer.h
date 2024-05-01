// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ChessGameInstance.h"
#include "ChessGameMode.h"
#include "Piece.h"
#include "PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "BlackRandomPlayer.generated.h"

UCLASS()
class PAA_VALENTE_API ABlackRandomPlayer : public APawn, public IPlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABlackRandomPlayer();
	UChessGameInstance* GameInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	bool bIsACapture;
	bool bThinking;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Interface's methods
	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual bool GetThinkingStatus() const override;
	virtual void DestroyPlayer() override;

};
