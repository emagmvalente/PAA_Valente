// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackRandomPlayer.h"
#include "Piece.h"
#include "EngineUtils.h"

// Sets default values
ABlackRandomPlayer::ABlackRandomPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GameInstance = Cast<UChessGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
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
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
	GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			TArray<APiece*> OutBlackPieces;

			for (TActorIterator<APiece> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				APiece* Piece = *ActorItr;
				// Controlla se il pezzo è di colore nero
				if (Piece && Piece->Color == EColor::B)
				{
					OutBlackPieces.Add(Piece); // Aggiungi il pezzo nero all'array
				}
			}

			// Picking a random piece
			int32 RandIdx0 = FMath::Rand() % OutBlackPieces.Num();
			APiece* ChosenPiece = OutBlackPieces[RandIdx0];

			// Picking a random tile
			FVector ActualLocation = ChosenPiece->RelativePosition();
			TArray<ATile*> ResultantArrayOfLegalMoves;
			ChosenPiece->PossibleMoves(ActualLocation, ResultantArrayOfLegalMoves);
			int32 RandIdx1 = FMath::Rand() % ResultantArrayOfLegalMoves.Num();
			ATile* DestinationTile = ResultantArrayOfLegalMoves[RandIdx1];

			// Moving the piece
			FVector2D RelativePositionOfTile = DestinationTile->GetGridPosition();
			ChosenPiece->MoveToLocation(FVector(RelativePositionOfTile.X, RelativePositionOfTile.Y, 10.f));

			GameMode->TurnNextPlayer();

		}, 3, false);
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

