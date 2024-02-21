// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameMode.h"
#include "WhitePlayer.h"
#include "ChessPlayerController.h"
#include "BlackRandomPlayer.h"
#include "EngineUtils.h"

AChessGameMode::AChessGameMode()
{
	DefaultPawnClass = AWhitePlayer::StaticClass();
	PlayerControllerClass = AChessPlayerController::StaticClass();
	FieldSize = 8;
}

void AChessGameMode::BeginPlay()
{
	Super::BeginPlay();

	bIsGameOver = false;

	AWhitePlayer* HumanPlayer = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));
	auto* AI = GetWorld()->SpawnActor<ABlackRandomPlayer>(FVector(), FRotator());

	// Human player = 0
	Players.Add(HumanPlayer);
	HumanPlayer->PlayerNumber = 0;
	// Random Player

	// MiniMax Player
	//auto* AI = GetWorld()->SpawnActor<ATTT_MinimaxPlayer>(FVector(), FRotator());

	// AI player = 1
	Players.Add(AI);
	AI->PlayerNumber = 1;

	if (CBClass != nullptr)
	{
		CB = GetWorld()->SpawnActor<AChessboard>(CBClass);
		CB->Size = FieldSize;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
	}

	float CameraPosX = ((CB->TileSize * (FieldSize + ((FieldSize - 1) * CB->NormalizedCellPadding) - (FieldSize - 1))) / 2) - (CB->TileSize / 2);
	FVector CameraPos(CameraPosX, CameraPosX, 1000.0f);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

	HumanPlayer->OnTurn();
}
