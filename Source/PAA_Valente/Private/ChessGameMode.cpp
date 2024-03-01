// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameMode.h"
#include "WhitePlayer.h"
#include "PieceKing.h"
#include "ChessPlayerController.h"
#include "BlackRandomPlayer.h"
#include "EngineUtils.h"

AChessGameMode::AChessGameMode()
{
	DefaultPawnClass = AWhitePlayer::StaticClass();
	PlayerControllerClass = AChessPlayerController::StaticClass();
	FieldSize = 8;
	bIsGameOver = false;
}

void AChessGameMode::BeginPlay()
{
	Super::BeginPlay();

	AWhitePlayer* HumanPlayer = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));

	// Random Player
	auto* AI = GetWorld()->SpawnActor<ABlackRandomPlayer>(FVector(), FRotator());

	// MiniMax Player
	//auto* AI = GetWorld()->SpawnActor<ATTT_MinimaxPlayer>(FVector(), FRotator());

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

	SetEnemyKing();
	HumanPlayer->OnTurn();
}

void AChessGameMode::SetEnemyKing()
{
	APiece* WhiteKing = nullptr;
	APiece* BlackKing = nullptr;

	// Finds WhiteKing
	for (int32 i = 0; i < CB->WhitePieces.Num(); i++)
	{
		if (Cast<APieceKing>(CB->WhitePieces[i]))
		{
			WhiteKing = CB->WhitePieces[i];
			break;
		}
	}
	// Finds BlackKing
	for (int32 i = 0; i < CB->BlackPieces.Num(); i++)
	{
		if (Cast<APieceKing>(CB->BlackPieces[i]))
		{
			BlackKing = CB->BlackPieces[i];
			break;
		}
	}

	// Sets the enemy king for each piece
	for (int32 i = 0; i < CB->WhitePieces.Num(); i++)
	{
		CB->WhitePieces[i]->EnemyKing = BlackKing;
	}
	for (int32 i = 0; i < CB->BlackPieces.Num(); i++)
	{
		CB->BlackPieces[i]->EnemyKing = WhiteKing;
	}
}

void AChessGameMode::TurnPlayer(IPlayerInterface* Player)
{
	AWhitePlayer* HumanPlayer = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));
	ABlackRandomPlayer* AIPlayer = Cast<ABlackRandomPlayer>(*TActorIterator<ABlackRandomPlayer>(GetWorld()));

	if (Player->PlayerNumber == 0)
	{
		AIPlayer->OnTurn();
	}
	else if (Player->PlayerNumber == 1)
	{
		HumanPlayer->OnTurn();
	}
}
