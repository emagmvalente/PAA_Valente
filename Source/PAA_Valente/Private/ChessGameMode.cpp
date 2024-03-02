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

	SetKings();
	HumanPlayer->OnTurn();
}

void AChessGameMode::SetKings()
{
	APiece* WhiteKing = nullptr;
	APiece* BlackKing = nullptr;

	int32 InitNumberOfPieces = 16;

	// Finds WhiteKing
	for (int32 i = 0; i < InitNumberOfPieces; i++)
	{
		if (Cast<APieceKing>(CB->WhitePieces[i]))
		{
			WhiteKing = CB->WhitePieces[i];
			break;
		}
	}
	// Finds BlackKing
	for (int32 i = 0; i < InitNumberOfPieces; i++)
	{
		if (Cast<APieceKing>(CB->BlackPieces[i]))
		{
			BlackKing = CB->BlackPieces[i];
			break;
		}
	}

	// Sets the enemy king for each piece
	for (int32 i = 0; i < InitNumberOfPieces; i++)
	{
		CB->WhitePieces[i]->WhiteKing = WhiteKing;
		CB->WhitePieces[i]->BlackKing = BlackKing;

		CB->BlackPieces[i]->WhiteKing = WhiteKing;
		CB->BlackPieces[i]->BlackKing = BlackKing;
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
