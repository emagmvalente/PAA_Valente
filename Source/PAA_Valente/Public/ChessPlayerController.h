// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WhitePlayer.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ChessPlayerController.generated.h"

class UMainHUD;

/**
 * 
 */
UCLASS()
class PAA_VALENTE_API AChessPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AChessPlayerController();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* ChessContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* Click;

	UMainHUD* MainHUDWidget;

	APiece* SelectedPieceToMove = nullptr;

	void ClickOnBoard();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
};
