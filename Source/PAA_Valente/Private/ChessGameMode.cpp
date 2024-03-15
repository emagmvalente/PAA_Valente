// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameMode.h"
#include "WhitePlayer.h"
#include "PieceKing.h"
#include "ChessPlayerController.h"
#include "BlackRandomPlayer.h"
#include "PlayerInterface.h"
#include "EngineUtils.h"

AChessGameMode::AChessGameMode()
{
	DefaultPawnClass = AWhitePlayer::StaticClass();
	PlayerControllerClass = AChessPlayerController::StaticClass();
	FieldSize = 8;
	bIsGameOver = false;
	bIsWhiteOnCheck = false;
	bIsBlackOnCheck = false;
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
	// Finds WhiteKing
	for (APiece* WhitePiece : CB->WhitePieces)
	{
		if (Cast<APieceKing>(WhitePiece))
		{
			CB->Kings[0] = WhitePiece;
			break;
		}
	}
	// Finds BlackKing
	for (APiece* BlackPiece : CB->BlackPieces)
	{
		if (Cast<APieceKing>(BlackPiece))
		{
			CB->Kings[1] = BlackPiece;
			break;
		}
	}
}

void AChessGameMode::VerifyCheck(APiece* Piece)
{
	ATile** WhiteKingTile = CB->TileMap.Find(FVector2D(CB->Kings[0]->RelativePosition().X, CB->Kings[0]->RelativePosition().Y));
	ATile** BlackKingTile = CB->TileMap.Find(FVector2D(CB->Kings[1]->RelativePosition().X, CB->Kings[1]->RelativePosition().Y));
	
	if (Piece->Color == EColor::B)
	{
		for (APiece* WhitePiece : CB->WhitePieces)
		{
			WhitePiece->PossibleMoves();
			if (WhitePiece->EatablePieces.Contains(*BlackKingTile))
			{
				bIsBlackOnCheck = true;
				break;
			}
			else
			{
				bIsBlackOnCheck = false;
			}
		}
	}

	else if (Piece->Color == EColor::W)
	{
		for (APiece* BlackPiece : CB->BlackPieces)
		{
			BlackPiece->PossibleMoves();
			if (BlackPiece->EatablePieces.Contains(*WhiteKingTile))
			{
				bIsWhiteOnCheck = true;
				break;
			}
			else
			{
				bIsWhiteOnCheck = false;
			}
		}
	}
}

void AChessGameMode::VerifyWin()
{
	for (APiece* WhitePiece : CB->WhitePieces)
	{
		WhitePiece->PossibleMoves();
		WhitePiece->FilterOnlyLegalMoves();
		if (WhitePiece->Moves.Num() > 0 || WhitePiece->EatablePieces.Num() > 0)
		{
			continue;
		}
		else
		{
			bIsGameOver = true;
			break;
		}
	}
	for (APiece* BlackPiece : CB->BlackPieces)
	{
		BlackPiece->PossibleMoves();
		BlackPiece->FilterOnlyLegalMoves();
		if (BlackPiece->Moves.Num() > 0 || BlackPiece->EatablePieces.Num() > 0)
		{
			continue;
		}
		else
		{
			bIsGameOver = true;
			break;
		}
	}
}

void AChessGameMode::TurnPlayer(IPlayerInterface* Player)
{
	AWhitePlayer* HumanPlayer = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));
	ABlackRandomPlayer* AIPlayer = Cast<ABlackRandomPlayer>(*TActorIterator<ABlackRandomPlayer>(GetWorld()));

	if (Player->PlayerNumber == 0)
	{
		VerifyCheck(CB->Kings[1]);
		VerifyWin();
		if (!bIsGameOver)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Game's not over!"));
			AIPlayer->OnTurn();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Game's over!"));
			HumanPlayer->OnWin();
		}
	}
	else if (Player->PlayerNumber == 1)
	{
		VerifyCheck(CB->Kings[0]);
		VerifyWin();
		if (!bIsGameOver)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Game's not over!"));
			HumanPlayer->OnTurn();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Game's over!"));
			AIPlayer->OnWin();
		}
	}
}
