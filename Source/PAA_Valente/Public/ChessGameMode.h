// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chessboard.h"
#include "Blueprint/UserWidget.h"
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
	bool bIsWhiteOnCheck;
	bool bIsBlackOnCheck;
	bool bIsGameOver;

public:

	AChessGameMode();
	virtual void BeginPlay() override;

	// Winning / Draw / Losing - FIELDS
	bool bIsBlackThinking;

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
	void SetKings();
	void TurnPlayer();
	void SetWhiteCheckStatus(bool NewStatus);
	void SetBlackCheckStatus(bool NewStatus);
	void SetGameOver(bool NewStatus);
	bool GetWhiteCheckStatus() const;
	bool GetBlackCheckStatus() const;
	bool GetGameOver() const;
	int32 GetCurrentTurnFlag() const;
	void ResetVariablesForRematch();

	// Winning / Draw / Losing - METHODS
	void VerifyCheck();
	void VerifyDraw();
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
