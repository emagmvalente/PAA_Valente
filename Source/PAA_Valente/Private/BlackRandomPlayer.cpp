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

	do
	{
		// Picking a random piece
		int32 RandIdx0 = FMath::Rand() % GameMode->CB->BlackPieces.Num();
		ChosenPiece = GameMode->CB->BlackPieces[RandIdx0];

		// Calculate piece's possible moves.
		ChosenPiece->PossibleMoves();

	} while (ChosenPiece->Moves.Num() == 0);
	
	// Getting previous tile
	ATile** PreviousTilePtr = GameMode->CB->TileMap.Find(FVector2D(ChosenPiece->RelativePosition().X, ChosenPiece->RelativePosition().Y));

	// Merging the array of moves and the array of eatable pieces
	TArray MovesAndEatablePieces = ChosenPiece->Moves;
	MovesAndEatablePieces.Append(ChosenPiece->EatablePieces);

	// Getting the new tile and the new position
	int32 RandIdx1 = FMath::Rand() % MovesAndEatablePieces.Num();
	ATile* DestinationTile = MovesAndEatablePieces[RandIdx1];
	FVector TilePositioning = GameMode->CB->GetRelativeLocationByXYPosition(DestinationTile->GetGridPosition().X, DestinationTile->GetGridPosition().Y);
	TilePositioning.Z = 10.0f;

	// Moving the piece
	ChosenPiece->SetActorLocation(TilePositioning);
	if (Cast<APiecePawn>(ChosenPiece) && Cast<APiecePawn>(ChosenPiece)->bFirstMove == true)
	{
		Cast<APiecePawn>(ChosenPiece)->bFirstMove = false;
	}

	// If it's an eating move, then delete the white piece
	if (DestinationTile->GetTileStatus() == ETileStatus::OCCUPIED && DestinationTile->GetOccupantColor() == EOccupantColor::W)
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
	}

	// Setting the actual tile occupied by a black, setting the old one empty
	(*PreviousTilePtr)->SetTileStatus(ETileStatus::EMPTY);
	(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);
	DestinationTile->SetTileStatus(ETileStatus::OCCUPIED);
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
