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

	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FTimerHandle TimerHandle;
	APiece* ChosenPiece = nullptr;

	do
	{
		int32 RandIdx0 = FMath::Rand() % GameMode->CB->BlackPieces.Num();
		// Picking a random piece
		ChosenPiece = GameMode->CB->BlackPieces[RandIdx0];

		// Calcolare le mosse possibili del pezzo selezionato
		ChosenPiece->PossibleMoves();

	} while (ChosenPiece->Moves.Num() == 0);
	
	int32 RandIdx1 = FMath::Rand() % ChosenPiece->Moves.Num();

	FVector PreviousLocation = ChosenPiece->RelativePosition();
	ATile** PreviousTilePtr = GameMode->CB->TileMap.Find(FVector2D(PreviousLocation.X, PreviousLocation.Y));

	// Moving the piece
	ATile* DestinationTile = ChosenPiece->Moves[RandIdx1];
	FVector2D RelativePositionOfTile = DestinationTile->GetGridPosition();

	FVector TilePositioning = GameMode->CB->GetRelativeLocationByXYPosition(RelativePositionOfTile.X, RelativePositionOfTile.Y);
	TilePositioning.Z = 10.0f;
	if (DestinationTile->GetTileStatus() == ETileStatus::EMPTY)
	{
		ChosenPiece->SetActorLocation(TilePositioning);

		if (ChosenPiece->RelativePosition() == PreviousLocation)
		{
			OnTurn();
		}
		else
		{
			(*PreviousTilePtr)->SetTileStatus(ETileStatus::EMPTY);
			(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);

			DestinationTile->SetTileStatus(ETileStatus::OCCUPIED);
			DestinationTile->SetOccupantColor(EOccupantColor::B);

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

