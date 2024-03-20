// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chessboard.h"
#include "Blueprint/UserWidget.h"
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
	bool bIsBlackThinking;

	int32 TurnFlag;

	// TSubclassOf is a template class that provides UClass type safety.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChessboard> CBClass;

	UPROPERTY(VisibleAnywhere)
	AChessboard* CB;

	APiece* PawnToPromote;

	// Dichiarazione della variabile di tipo TSubclassOf per il widget
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PawnPromotionWidgetClass;

	// Puntatore al widget effettivamente creato durante l'esecuzione
	UUserWidget* PawnPromotionWidgetInstance;

	// field size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	AChessGameMode();

	virtual void BeginPlay() override;

	void SetKings();
	void VerifyCheck(APiece* Piece);
	void VerifyWin(APiece* Piece);

	void VerifyTie(APiece* Piece);

	UFUNCTION(BlueprintCallable)
	void PromoteToQueen();

	UFUNCTION(BlueprintCallable)
	void PromoteToRook();

	UFUNCTION(BlueprintCallable)
	void PromoteToBishop();

	UFUNCTION(BlueprintCallable)
	void PromoteToKnight();

	// called at the end of the game turn
	void TurnPlayer();

};
