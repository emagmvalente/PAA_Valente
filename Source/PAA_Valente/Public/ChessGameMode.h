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

public:

	AChessGameMode();
	virtual void BeginPlay() override;

	// Winning / Draw / Losing - FIELDS
	bool bIsGameOver;
	bool bIsWhiteOnCheck;
	bool bIsBlackOnCheck;
	bool bIsBlackThinking;
	int32 MovesWithoutCaptureOrPawnMove;

	// Logic - FIELDS
	int32 TurnFlag;

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

	// Winning / Draw / Losing - METHODS
	void VerifyCheck();
	void VerifyDraw();
	bool CheckThreeOccurrences();
	bool KingvsKing();
	bool FiftyMovesRule();

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
