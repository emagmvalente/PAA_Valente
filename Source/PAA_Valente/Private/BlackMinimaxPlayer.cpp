// Fill out your copyright notice in the Description page of Project Settings. a


#include "BlackMinimaxPlayer.h"

// Sets default values
ABlackMinimaxPlayer::ABlackMinimaxPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABlackMinimaxPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlackMinimaxPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABlackMinimaxPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABlackMinimaxPlayer::OnTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Minimax) Turn"));
	GameInstance->SetTurnMessage(TEXT("AI (Minimax) Turn"));

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			AChessGameMode* GameMode = (AChessGameMode*)(GetWorld()->GetAuthGameMode());

			FVector2D BestMove = FindBestMove(GameMode->CB->TileMap);
			FVector Location = GameMode->CB->GetRelativeLocationByXYPosition(BestMove.X, BestMove.Y);

			// Get the piece to move in that location and destroy a possible white occupant
			GameMode->CB->TileMap[BestMove]->SetOccupantColor(EOccupantColor::B);

		}, 3, false);
}

void ABlackMinimaxPlayer::OnWin()
{
}

int32 ABlackMinimaxPlayer::EvaluateGrid(TMap<FVector2D, ATile*>& Board)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	ATile** WhiteKingTile = GameMode->CB->TileMap.Find(GameMode->CB->Kings[0]->Relative2DPosition());
	ATile** BlackKingTile = GameMode->CB->TileMap.Find(GameMode->CB->Kings[1]->Relative2DPosition());

	for (APiece* AllyPiece : GameMode->CB->BlackPieces)
	{
		ATile** CurrentTile = GameMode->CB->TileMap.Find(AllyPiece->Relative2DPosition());
		(*CurrentTile)->SetOccupantColor(EOccupantColor::E);
		
		for (ATile* Move : AllyPiece->Moves)
		{
			Move->SetOccupantColor(EOccupantColor::B);
		}
	}

	return int32();
}

int32 ABlackMinimaxPlayer::MiniMax(TMap<FVector2D, ATile*>& Board, int32 Depth, bool IsMax)
{
	return int32();
}

FVector2D ABlackMinimaxPlayer::FindBestMove(TMap<FVector2D, ATile*>& Board)
{
	int32 BestVal = -100000000;
	FVector2D BestMove;
	BestMove.X = -1;
	BestMove.Y = -1;

	for (int32 Row = 0; Row < 8; Row++)
	{
		for (int32 Col = 0; Col < 8; Col++)
		{
			if ((Board[FVector2D(Row, Col)])->GetOccupantColor() != EOccupantColor::B)
			{
				// Make the move (set the AI player owner)
				(Board[FVector2D(Row, Col)])->SetOccupantColor(EOccupantColor::B);

				// compute evaluation function for this
				// move.
				int32 MoveVal = MiniMax(Board, 0, false);

				// Undo the move
				(Board[FVector2D(Row, Col)])->SetOccupantColor(EOccupantColor::B);

				// If the value of the current move is
				// more than the best value, then update
				// best/
				if (MoveVal > BestVal)
				{
					BestMove.X = Row;
					BestMove.Y = Col;
					BestVal = MoveVal;
				}
			}
		}
	}

	return BestMove;
}

