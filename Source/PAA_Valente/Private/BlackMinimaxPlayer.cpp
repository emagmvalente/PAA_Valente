// Fill out your copyright notice in the Description page of Project Settings. a


#include "BlackMinimaxPlayer.h"
#include "Piece.h"
#include "PiecePawn.h"
#include "EngineUtils.h"
#include "MainHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Sets default values
ABlackMinimaxPlayer::ABlackMinimaxPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GameInstance = Cast<UChessGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	bIsACapture = false;
	SelectedPieceToMove = nullptr;
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

	AChessGameMode* GameMode = (AChessGameMode*)(GetWorld()->GetAuthGameMode());

	FTimerHandle TimerHandle;

	GameMode->bIsBlackThinking = true;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			AChessGameMode* GameModeCallback = (AChessGameMode*)(GetWorld()->GetAuthGameMode());
			AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());
			UMainHUD* MainHUD = CPC->MainHUDWidget;
			FVector2D OldPosition(0, 0);
			ATile** PreviousTilePtr = nullptr;

			FVector2D BestMove = FindBestMove(false);
			FVector Location = GameModeCallback->CB->GetRelativeLocationByXYPosition(BestMove.X, BestMove.Y);
			Location.Z = 10.f;

			if (SelectedPieceToMove)
			{
				OldPosition = SelectedPieceToMove->Relative2DPosition();
				PreviousTilePtr = GameModeCallback->CB->TileMap.Find(SelectedPieceToMove->Relative2DPosition());
			}

			ATile* DestinationTile = GameModeCallback->CB->TileMap[BestMove];

			if (DestinationTile->GetOccupantColor() == EOccupantColor::W)
			{
				// Search the white piece who occupies the tile and capture it
				for (APiece* WhitePiece : GameModeCallback->CB->WhitePieces)
				{
					if (WhitePiece->GetActorLocation() == Location)
					{
						GameModeCallback->CB->WhitePieces.Remove(WhitePiece);
						WhitePiece->Destroy();
						bIsACapture = true;
						break;
					}
				}
			}

			SelectedPieceToMove->SetActorLocation(Location);
			if (Cast<APiecePawn>(SelectedPieceToMove))
			{
				Cast<APiecePawn>(SelectedPieceToMove)->ResetTurnsWithoutMoving();
				Cast<APiecePawn>(SelectedPieceToMove)->Promote();
				if (Cast<APiecePawn>(SelectedPieceToMove)->GetIsFirstMove())
				{
					Cast<APiecePawn>(SelectedPieceToMove)->PawnMovedForTheFirstTime();
				}
			}
			else
			{
				for (APiece* BlackPawn : GameModeCallback->CB->BlackPieces)
				{
					if (Cast<APiecePawn>(BlackPawn))
					{
						Cast<APiecePawn>(BlackPawn)->IncrementTurnsWithoutMoving();
					}
				}
			}

			(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);
			DestinationTile->SetOccupantColor(EOccupantColor::B);

			FString LastMove = GameModeCallback->CB->GenerateStringFromPositions();
			GameModeCallback->CB->HistoryOfMoves.Add(LastMove);

			// Create dinamically the move button
			if (MainHUD)
			{
				MainHUD->AddButton();
				if (MainHUD->ButtonArray.Num() > 0)
				{
					UOldMovesButtons* LastButton = MainHUD->ButtonArray.Last();
					if (LastButton)
					{
						LastButton->SetAssociatedString(GameModeCallback->CB->HistoryOfMoves.Last());
						LastButton->CreateText(SelectedPieceToMove, bIsACapture, SelectedPieceToMove->Relative2DPosition(), OldPosition);
					}
				}
			}

			bIsACapture = false;

			// Turn ending
			GameModeCallback->bIsBlackThinking = false;
			if (!Cast<APiecePawn>(SelectedPieceToMove) || Cast<APiecePawn>(SelectedPieceToMove)->Relative2DPosition().X != 0)
			{
				SelectedPieceToMove = nullptr;
				GameModeCallback->TurnPlayer();
			}

		}, 3, false);
}

void ABlackMinimaxPlayer::OnWin()
{
}

int32 ABlackMinimaxPlayer::EvaluateGrid(TMap<FVector2D, ATile*>& Board)
{
	return int32();
}

int32 ABlackMinimaxPlayer::MiniMax(TMap<FVector2D, ATile*>& Board, int32 Depth, bool IsMax)
{
	return int32();
}

FVector2D ABlackMinimaxPlayer::FindBestMove(bool IsMax)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	int32 BestVal = 0;
	FVector2D BestMove(-1, -1);

	TArray<APiece*> AllyPieces;
	EColor AllyColor = EColor::E;
	TArray<APiece*> EnemyPieces;
	EColor EnemyColor = EColor::E;

	if (IsMax)
	{
		AllyPieces = GameMode->CB->WhitePieces;
		AllyColor = EColor::W;
		EnemyPieces = GameMode->CB->BlackPieces;
		EnemyColor = EColor::B;
		BestVal = 10000000;
	}
	else
	{
		AllyPieces = GameMode->CB->BlackPieces;
		AllyColor = EColor::B;
		EnemyPieces = GameMode->CB->WhitePieces;
		EnemyColor = EColor::W;
		BestVal = -10000000;
	}

	for (APiece* AllyPiece : AllyPieces)
	{
		AllyPiece->PossibleMoves();
		AllyPiece->FilterOnlyLegalMoves();

		int32 CurrVal = 0;

		if (AllyPiece->EatablePiecesPosition.Num() == 0)
		{
			continue;
		}

		for (ATile* Move : AllyPiece->EatablePiecesPosition)
		{
			for (APiece* EnemyPiece : EnemyPieces)
			{
				if (EnemyPiece->Relative2DPosition() == Move->GetGridPosition())
				{
					CurrVal = EnemyPiece->GetPieceValue();
					break;
				}
			}

			if (((CurrVal < BestVal) && IsMax) || 
				((CurrVal > BestVal) && !IsMax))
			{
				SelectedPieceToMove = AllyPiece;
				BestVal = CurrVal;
				BestMove = Move->GetGridPosition();
			}
		}
	}

	if (BestMove.X == -1 && BestMove.Y == -1)
	{
		TArray<APiece*> PiecesWithLegalMoves;

		for (APiece* AllyPiece : AllyPieces)
		{
			if (AllyPiece->Moves.Num() > 0)
			{
				PiecesWithLegalMoves.Add(AllyPiece);
			}
		}

		int32 RandIdx0 = FMath::Rand() % PiecesWithLegalMoves.Num();
		SelectedPieceToMove = PiecesWithLegalMoves[RandIdx0];
		int32 RandIdx1 = FMath::Rand() % SelectedPieceToMove->Moves.Num();
		BestMove = SelectedPieceToMove->Moves[RandIdx1]->GetGridPosition();
	}

	return BestMove;
}

