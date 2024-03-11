// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPlayerController.h"

AChessPlayerController::AChessPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
}

void AChessPlayerController::ClickOnBoard()
{
	const auto HumanPlayer = Cast<AWhitePlayer>(GetPawn());
	if (IsValid(HumanPlayer))
	{
		HumanPlayer->PieceSelection();
	}
}

void AChessPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(ChessContext, 0);
	}
}

void AChessPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(Click, ETriggerEvent::Triggered, this, &AChessPlayerController::ClickOnBoard);
	}
}
