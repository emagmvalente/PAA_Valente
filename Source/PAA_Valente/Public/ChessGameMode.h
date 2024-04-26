// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chessboard.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "PlayerInterface.h"
#include "GameFramework/GameModeBase.h"
#include "ChessGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PAA_VALENTE_API AChessGameMode : public AGameModeBase
{
	GENERATED_BODY()

	int32 TurnFlag;
	int32 MovesWithoutCaptureOrPawnMove;
	bool bOnMenu;

public:

	AChessGameMode();
	virtual void BeginPlay() override;

	TArray<IPlayerInterface*> Players;

	// Pawn Promotion - FIELDS
	APiece* PawnToPromote;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PawnPromotionWidgetClass;
	UUserWidget* PawnPromotionWidgetInstance;

	// Chessboard References - FIELDS
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChessboard> CBClass;
	UPROPERTY(VisibleAnywhere)
	AChessboard* CB;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	// Logic and Utilities - METHODS
	void TurnPlayer();
	void ResetVariablesForRematch();
	int32 GetTurnFlag() const;
	UFUNCTION(BlueprintCallable)
	void SpawnHumanAndAI(bool bSpawnMinimax);

	// Winning / Draw / Losing - METHODS
	bool VerifyCheck();
	bool VerifyCheckmate();
	bool VerifyDraw();
	bool CheckThreeOccurrences();
	bool KingvsKing();
	bool FiftyMovesRule();
	bool Stalemate();

	// Pawn Promotion - METHODS
	UFUNCTION(BlueprintCallable)
	void PromoteToQueen();
	UFUNCTION(BlueprintCallable)
	void PromoteToRook();
	UFUNCTION(BlueprintCallable)
	void PromoteToBishop();
	UFUNCTION(BlueprintCallable)
	void PromoteToKnight();

};
