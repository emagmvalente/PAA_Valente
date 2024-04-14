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
	BestPieceToMove = nullptr;
	BestTileToMove = nullptr;
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

			CallMiniMaxAndFoundAttributes(2);

			FVector2D OldPosition = BestPieceToMove->Relative2DPosition();
			ATile* PreviousTile = GameModeCallback->CB->TileMap[OldPosition];

			FVector Location = GameModeCallback->CB->GetRelativeLocationByXYPosition(BestTileToMove->GetGridPosition().X, BestTileToMove->GetGridPosition().Y);
			Location.Z = 10.f;

			if (BestTileToMove->GetOccupantColor() == EOccupantColor::W)
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

			BestPieceToMove->SetActorLocation(Location);
			if (Cast<APiecePawn>(BestPieceToMove))
			{
				Cast<APiecePawn>(BestPieceToMove)->ResetTurnsWithoutMoving();
				Cast<APiecePawn>(BestPieceToMove)->Promote();
				if (Cast<APiecePawn>(BestPieceToMove)->GetIsFirstMove())
				{
					Cast<APiecePawn>(BestPieceToMove)->PawnMovedForTheFirstTime();
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

			PreviousTile->SetOccupantColor(EOccupantColor::E);
			BestTileToMove->SetOccupantColor(EOccupantColor::B);

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
						LastButton->CreateText(BestPieceToMove, bIsACapture, BestPieceToMove->Relative2DPosition(), OldPosition);
					}
				}
			}

			bIsACapture = false;

			// Turn ending
			GameModeCallback->bIsBlackThinking = false;
			if (!Cast<APiecePawn>(BestPieceToMove) || Cast<APiecePawn>(BestPieceToMove)->Relative2DPosition().X != 0)
			{
				BestPieceToMove = nullptr;
				BestTileToMove = nullptr;
				GameModeCallback->TurnPlayer();
			}
		}, 3, false);
}

void ABlackMinimaxPlayer::OnWin()
{
}

int32 ABlackMinimaxPlayer::EvaluateGrid()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	// Check & Checkmate cases
	if (GameMode->VerifyCheck())
	{
		if (GameMode->GetIsWhiteOnCheck())
		{
			if (GameMode->VerifyCheckmate())
			{
				return -100;
			}
			
			ATile* WhiteKingTile = GameMode->CB->TileMap[GameMode->CB->Kings[0]->GetVirtualPosition()];

			// If white's on check and piece that threatens could be eaten, return it's value in positive
			for (APiece* BlackPiece : GameMode->CB->BlackPieces)
			{
				if (BlackPiece->EatablePiecesPosition.Contains(WhiteKingTile))
				{
					for (APiece* WhitePiece : GameMode->CB->WhitePieces)
					{
						WhitePiece->PossibleMoves(WhitePiece->GetVirtualPosition());
						WhitePiece->FilterOnlyLegalMoves();
						ATile* ThreatenPiecePosition = GameMode->CB->TileMap[BlackPiece->GetVirtualPosition()];
						if (WhitePiece->EatablePiecesPosition.Contains(ThreatenPiecePosition))
						{
							return -BlackPiece->GetPieceValue();
						}
					}

					// If none can save the white king, return an high value for black
					return -10;
				}
			}
		}

		else if (GameMode->GetIsBlackOnCheck())
		{
			if (GameMode->VerifyCheckmate())
			{
				return 100;
			}

			ATile* BlackKingTile = GameMode->CB->TileMap[GameMode->CB->Kings[1]->GetVirtualPosition()];

			// If black's on check and piece that threatens could be eaten, return it's value in negative
			for (APiece* WhitePiece : GameMode->CB->WhitePieces)
			{
				if (WhitePiece->EatablePiecesPosition.Contains(BlackKingTile))
				{
					for (APiece* BlackPiece : GameMode->CB->BlackPieces)
					{
						BlackPiece->PossibleMoves(BlackPiece->GetVirtualPosition());
						BlackPiece->FilterOnlyLegalMoves();
						ATile* ThreatenPiecePosition = GameMode->CB->TileMap[WhitePiece->GetVirtualPosition()];
						if (WhitePiece->EatablePiecesPosition.Contains(ThreatenPiecePosition))
						{
							return -WhitePiece->GetPieceValue();
						}
					}

					// If none can save the black king, return an high value for white
					return 10;
				}
			}
		}
	}

	return 0;
}

int32 ABlackMinimaxPlayer::MiniMax(int32 Depth, bool IsMax)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	int32 Score = EvaluateGrid();

	if (Score == 100 || Score == -100)
	{
		return Score;
	}


	if (IsMax)
	{
		int32 BestForMax = -10000000;

		for (APiece* WhitePiece : GameMode->CB->WhitePieces)
		{
			WhitePiece->PossibleMoves(WhitePiece->GetVirtualPosition());
			WhitePiece->FilterOnlyLegalMoves();
			
			TArray<ATile*> MovesAndEatablePositions = WhitePiece->Moves;
			MovesAndEatablePositions.Append(WhitePiece->EatablePiecesPosition);

			for (ATile* Move : MovesAndEatablePositions)
			{
				FVector2D PreviousPosition = WhitePiece->GetVirtualPosition();
				ATile* PreviousTile = GameMode->CB->TileMap[PreviousPosition];

				PreviousTile->SetOccupantColor(EOccupantColor::E);
				Move->SetOccupantColor(EOccupantColor::W);
				WhitePiece->SetVirtualPosition(Move->GetGridPosition());

				int32 CurrentValue = MiniMax(Depth - 1, !IsMax);

				if (CurrentValue >= BestForMax)
				{
					BestForMax = CurrentValue;
					BestPieceToMove = WhitePiece;
					BestTileToMove = Move;
				}

				PreviousTile->SetOccupantColor(EOccupantColor::W);
				Move->SetOccupantColor(EOccupantColor::E);
				WhitePiece->SetVirtualPosition(PreviousPosition);
			}
		}

		return BestForMax;
	}

	else
	{
		int32 BestForMin = 10000000;

		for (APiece* BlackPiece : GameMode->CB->BlackPieces)
		{
			BlackPiece->PossibleMoves(BlackPiece->GetVirtualPosition());
			BlackPiece->FilterOnlyLegalMoves();

			TArray<ATile*> MovesAndEatablePositions = BlackPiece->Moves;
			MovesAndEatablePositions.Append(BlackPiece->EatablePiecesPosition);

			for (ATile* Move : MovesAndEatablePositions)
			{
				FVector2D PreviousPosition = BlackPiece->GetVirtualPosition();
				ATile* PreviousTile = GameMode->CB->TileMap[PreviousPosition];

				PreviousTile->SetOccupantColor(EOccupantColor::E);
				Move->SetOccupantColor(EOccupantColor::B);
				BlackPiece->SetVirtualPosition(Move->GetGridPosition());

				int32 CurrentValue = MiniMax(Depth - 1, IsMax);

				if (CurrentValue <= BestForMin)
				{
					BestForMin = CurrentValue;
					BestPieceToMove = BlackPiece;
					BestTileToMove = Move;
				}

				PreviousTile->SetOccupantColor(EOccupantColor::B);
				Move->SetOccupantColor(EOccupantColor::E);
				BlackPiece->SetVirtualPosition(PreviousPosition);
			}
		}

		return BestForMin;
	}

	// This return will never be used
	return int32();
}

void ABlackMinimaxPlayer::CallMiniMaxAndFoundAttributes(int32 Depth)
{
	MiniMax(Depth, false);
}
