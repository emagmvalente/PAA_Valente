// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ChessGameInstance.h"
#include "Piece.h"
#include "PlayerInterface.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WhitePlayer.generated.h"

UCLASS()
class PAA_VALENTE_API AWhitePlayer : public APawn, public IPlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AWhitePlayer();

	// camera component attacched to player pawn
	UCameraComponent* Camera;

	// game instance reference
	UChessGameInstance* GameInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// keeps track of turn
	bool IsMyTurn;
	bool bIsACapture;
	APiece* SelectedPieceToMove;

	EColor AllyColor;
	EOccupantColor AllyOccupantColor;
	TArray<APiece*>* AllyPieces;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PieceSelection();
	void TileSelection(ATile* CurrTile);

	APiece* GetSelectedPieceToMove() const;

	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual bool GetThinkingStatus() const override;
	virtual void DestroyPlayer() override;

	void SetTeam(EColor TeamColor);

};
