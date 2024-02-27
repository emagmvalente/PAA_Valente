// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackRandomPlayer.h"
#include "Piece.h"

#include "PieceBishop.h"
#include "PieceKing.h"
#include "PieceKnight.h"
#include "PiecePawn.h"
#include "PieceQueen.h"
#include "PieceRook.h"

#include "EngineUtils.h"

// Sets default values
ABlackRandomPlayer::ABlackRandomPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GameInstance = Cast<UChessGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	PlayerNumber = 1;
}

// Called when the game starts or when spawned
void ABlackRandomPlayer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABlackRandomPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABlackRandomPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABlackRandomPlayer::OnTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
	GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));

	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FTimerHandle TimerHandle;
	APiece* ChosenPiece = nullptr;

	do
	{
		// Picking a random piece
		int32 RandIdx0 = FMath::Rand() % GameMode->CB->BlackPieces.Num();
		ChosenPiece = GameMode->CB->BlackPieces[RandIdx0];

		// Calculate piece's possible moves.
		ChosenPiece->PossibleMoves();

	} while (ChosenPiece->Moves.Num() == 0);
	
	// Getting previous tile
	FVector PreviousLocation = ChosenPiece->RelativePosition();
	ATile** PreviousTilePtr = GameMode->CB->TileMap.Find(FVector2D(PreviousLocation.X, PreviousLocation.Y));

	// Merging the array of moves and the array of eatable pieces
	TArray MovesAndEatablePieces = ChosenPiece->Moves;
	MovesAndEatablePieces.Append(ChosenPiece->EatablePieces);

	FString DebugMessage1 = FString::Printf(TEXT("Numero di mosse %d"), ChosenPiece->Moves.Num());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, *DebugMessage1);

	FString DebugMessage2 = FString::Printf(TEXT("Numero di pezzi mangiabili %d"), ChosenPiece->EatablePieces.Num());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, *DebugMessage2);

	FString DebugMessage3 = FString::Printf(TEXT("Numero di mosse + numero di pezzi mangiabili %d"), MovesAndEatablePieces.Num());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, *DebugMessage3);

	// Moving the piece and getting the new tile
	int32 RandIdx1 = FMath::Rand() % MovesAndEatablePieces.Num();
	ATile* DestinationTile = MovesAndEatablePieces[RandIdx1];
	FVector2D RelativePositionOfTile = DestinationTile->GetGridPosition();
	FVector TilePositioning = GameMode->CB->GetRelativeLocationByXYPosition(RelativePositionOfTile.X, RelativePositionOfTile.Y);
	TilePositioning.Z = 10.0f;

	// Spawning actor if the tile is empty or if there's an eatable piece
	if (DestinationTile->GetTileStatus() == ETileStatus::EMPTY)
	{
		ChosenPiece->SetActorLocation(TilePositioning);

		if (ChosenPiece->RelativePosition() == PreviousLocation)
		{
			// If the move wasn't legal and the piece is still on the previous position, then recall the function
			OnTurn();
		}
		else
		{
			// Setting the actual tile occupied by a black, setting the old one empty
			(*PreviousTilePtr)->SetTileStatus(ETileStatus::EMPTY);
			(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);

			DestinationTile->SetTileStatus(ETileStatus::OCCUPIED);
			DestinationTile->SetOccupantColor(EOccupantColor::B);

			// Generate the FEN string and add it to the history of moves for replays
			FString LastMove = GameMode->CB->GenerateStringFromPositions();
			GameMode->CB->HistoryOfMoves.Add(LastMove);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LastMove);

			// Turn ending
			GameMode->TurnPlayer(this);
		}
	}

	else if (DestinationTile->GetTileStatus() == ETileStatus::OCCUPIED && DestinationTile->GetOccupantColor() == EOccupantColor::W)
	{
		ChosenPiece->SetActorLocation(TilePositioning);

		if (ChosenPiece->RelativePosition() == PreviousLocation)
		{
			// If the move wasn't legal and the piece is still on the previous position, then recall the function
			OnTurn();
		}
		else
		{
			// Search the white piece who occupies the tile and capture it
			for (int32 i = 0; i < GameMode->CB->WhitePieces.Num(); ++i)
			{
				if (GameMode->CB->WhitePieces[i]->GetActorLocation() == TilePositioning)
				{
					GameMode->CB->WhitePieces[i]->PieceCaptured();
					break;
				}
			}

			// Setting the actual tile occupied by a black, setting the old one empty
			(*PreviousTilePtr)->SetTileStatus(ETileStatus::EMPTY);
			(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);

			DestinationTile->SetTileStatus(ETileStatus::OCCUPIED);
			DestinationTile->SetOccupantColor(EOccupantColor::B);

			// Generate the FEN string and add it to the history of moves for replays
			FString LastMove = GameMode->CB->GenerateStringFromPositions();
			GameMode->CB->HistoryOfMoves.Add(LastMove);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LastMove);

			// Turn ending
			GameMode->TurnPlayer(this);
		}
	}

	else
	{
		OnTurn();
	}
	
}

void ABlackRandomPlayer::OnWin()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Wins!"));
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
	GameInstance->IncrementScoreAiPlayer();
}

void ABlackRandomPlayer::OnLose()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Loses!"));
	// GameInstance->SetTurnMessage(TEXT("AI Loses!"));
}

