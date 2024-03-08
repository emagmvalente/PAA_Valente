// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackRandomPlayer.h"
#include "Piece.h"
#include "PiecePawn.h"
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
	APiece* ChosenPiece = nullptr;
	TArray<ATile*> MovesAndEatablePieces;

	if (GameMode->bIsBlackOnCheck)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Black is on Check!"));
	}

	do
	{
		MovesAndEatablePieces.Empty();

		// Picking a random piece
		int32 RandIdx0 = FMath::Rand() % GameMode->CB->BlackPieces.Num();
		ChosenPiece = GameMode->CB->BlackPieces[RandIdx0];

		// Calculate piece's possible moves.
		ChosenPiece->PossibleMoves();
		ChosenPiece->FilterOnlyLegalMoves();

		MovesAndEatablePieces = ChosenPiece->Moves;
		MovesAndEatablePieces.Append(ChosenPiece->EatablePieces);

	} while (MovesAndEatablePieces.Num() == 0);
	
	// Getting previous tile
	ATile** PreviousTilePtr = GameMode->CB->TileMap.Find(FVector2D(ChosenPiece->RelativePosition().X, ChosenPiece->RelativePosition().Y));

	// Getting the new tile and the new position
	int32 RandIdx1 = FMath::Rand() % MovesAndEatablePieces.Num();
	ATile* DestinationTile = MovesAndEatablePieces[RandIdx1];
	FVector TilePositioning = GameMode->CB->GetRelativeLocationByXYPosition(DestinationTile->GetGridPosition().X, DestinationTile->GetGridPosition().Y);
	TilePositioning.Z = 10.0f;
	APiece* PieceToCapture = nullptr;

	// If it's an eating move, then delete the white piece
	if (DestinationTile->GetOccupantColor() == EOccupantColor::W)
	{
		// Search the white piece who occupies the tile and capture it
		for (APiece* WhitePiece : GameMode->CB->WhitePieces)
		{
			if (WhitePiece->GetActorLocation() == TilePositioning)
			{
				GameMode->CB->WhitePieces.Remove(WhitePiece);
				WhitePiece->PieceCaptured();
				break;
			}
		}
	}

	// Moving the piece
	ChosenPiece->SetActorLocation(TilePositioning);
	if (Cast<APiecePawn>(ChosenPiece) && Cast<APiecePawn>(ChosenPiece)->bFirstMove == true)
	{
		Cast<APiecePawn>(ChosenPiece)->bFirstMove = false;
	}

	// Setting the actual tile occupied by a black, setting the old one empty
	(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);
	DestinationTile->SetOccupantColor(EOccupantColor::B);

	// Generate the FEN string and add it to the history of moves for replays
	FString LastMove = GameMode->CB->GenerateStringFromPositions();
	GameMode->CB->HistoryOfMoves.Add(LastMove);

	// Turn ending
	GameMode->TurnPlayer(this);

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

bool ABlackRandomPlayer::IsCheckStatus()
{
	return false;
}
