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

// DA PROVARE

void ABlackRandomPlayer::OnTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
	GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));

	

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			TArray<ATile*> FreeCells;
			AChessGameMode* GameMode = (AChessGameMode*)(GetWorld()->GetAuthGameMode());
			for (auto& CurrTile : GameMode->CB->GetTileArray())
			{
				if (CurrTile->GetTileStatus() == ETileStatus::EMPTY)
				{
					FreeCells.Add(CurrTile);
				}
			}

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

			if (FreeCells.Num() > 0)
			{
				int32 RandIdx0 = FMath::Rand() % OutBlackPieces.Num();
				int32 RandIdx1 = FMath::Rand() % FreeCells.Num();
				FVector ActualLocation = OutBlackPieces[RandIdx1]->RelativePosition();
				FVector DestinationLocation = GameMode->CB->GetRelativeLocationByXYPosition((FreeCells[RandIdx1])->GetGridPosition()[0], (FreeCells[RandIdx1])->GetGridPosition()[1]);
				
				OutBlackPieces[RandIdx1]->MoveToLocation(DestinationLocation);
				
				FreeCells[RandIdx1]->SetTileStatus(ETileStatus::OCCUPIED);
				FreeCells[RandIdx1]->SetOccupantColor(EOccupantColor::B);

				// Da finire

			}
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

