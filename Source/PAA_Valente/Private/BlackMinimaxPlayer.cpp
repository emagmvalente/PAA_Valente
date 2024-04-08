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
	AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());
	UMainHUD* MainHUD = CPC->MainHUDWidget;

	FTimerHandle TimerHandle;

	GameMode->bIsBlackThinking = true;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			AChessGameMode* GameModeCallback = (AChessGameMode*)(GetWorld()->GetAuthGameMode());
			FVector2D OldPosition(0, 0);
			if (SelectedPieceToMove)
			{
				OldPosition = SelectedPieceToMove->Relative2DPosition();
			}

			FVector2D BestMove = FindBestMove(GameModeCallback->CB->TileMap);
			FVector Location = GameModeCallback->CB->GetRelativeLocationByXYPosition(BestMove.X, BestMove.Y);
			Location.Z = 10.f;

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
						LastButton->SetPieceToPrint(SelectedPieceToMove);
						LastButton->SetItWasACapture(bIsACapture);
						LastButton->SetNewLocationToPrint(SelectedPieceToMove->Relative2DPosition());
						if (Cast<APiecePawn>(CPC->SelectedPieceToMove))
						{
							LastButton->SetOldLocationToPrint(OldPosition);
						}

						LastButton->CreateText();
					}
				}
			}

			bIsACapture = false;

			// Turn ending
			GameModeCallback->bIsBlackThinking = false;
			if (!Cast<APiecePawn>(SelectedPieceToMove) || Cast<APiecePawn>(SelectedPieceToMove)->Relative2DPosition().X != 0)
			{
				GameModeCallback->TurnPlayer();
			}

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
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	int32 BestVal = 100000000;
	FVector2D BestMove;
	BestMove.X = -1;
	BestMove.Y = -1;

	for (APiece* BlackPiece : GameMode->CB->BlackPieces)
	{
		TArray<ATile*> MovesAndEatablePiecePositions = BlackPiece->Moves;
		MovesAndEatablePiecePositions.Append(BlackPiece->EatablePiecesPosition);

		int32 CurrVal = 0;

		for (ATile* Move : MovesAndEatablePiecePositions)
		{
			if (Move->GetOccupantColor() == EOccupantColor::W)
			{
				for (APiece* WhitePiece : GameMode->CB->WhitePieces)
				{
					if (WhitePiece->Relative2DPosition() == Move->GetGridPosition())
					{
						CurrVal = WhitePiece->GetPieceValue();
						bIsACapture = true;
						break;
					}
				}
			}
			else
			{
				CurrVal = 0;
			}

			if (CurrVal < BestVal)
			{
				BestVal = CurrVal;
				BestMove.X = Move->GetGridPosition().X;
				BestMove.Y = Move->GetGridPosition().Y;
				SelectedPieceToMove = BlackPiece;
			}
		}
	}

	for (APiece* WhitePiece : GameMode->CB->WhitePieces)
	{
		if (WhitePiece->Relative2DPosition() == BestMove)
		{
			GameMode->CB->WhitePieces.Remove(WhitePiece);
			WhitePiece->Destroy();
		}
	}

	return BestMove;
}

