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
	BestPiece = nullptr;
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
			// Declarations
			AChessGameMode* GameModeCallback = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
			AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());
			TArray<ATile*> MovesAndEatablePieces;
			UMainHUD* MainHUD = CPC->MainHUDWidget;

			ATile* BestTile = FindBestMove();

			// Getting previous tile
			ATile** PreviousTilePtr = GameModeCallback->CB->TileMap.Find(BestPiece->GetVirtualPosition());
			FVector2D OldPosition = BestPiece->GetVirtualPosition();

			// Getting the new tile and the new position
			FVector TilePositioning = GameModeCallback->CB->GetRelativeLocationByXYPosition(BestTile->GetGridPosition().X, BestTile->GetGridPosition().Y);
			TilePositioning.Z = 10.0f;

			// If it's an eating move, then delete the white piece
			if (BestTile->GetOccupantColor() == EOccupantColor::W)
			{
				// Search the white piece who occupies the tile and capture it
				for (APiece* WhitePiece : GameModeCallback->CB->WhitePieces)
				{
					if (WhitePiece->GetActorLocation() == TilePositioning)
					{
						GameModeCallback->CB->WhitePieces.Remove(WhitePiece);
						WhitePiece->Destroy();
						bIsACapture = true;
						break;
					}
				}
			}

			// Moving the piece
			BestPiece->SetActorLocation(TilePositioning);
			if (Cast<APiecePawn>(BestPiece))
			{
				Cast<APiecePawn>(BestPiece)->ResetTurnsWithoutMoving();
				Cast<APiecePawn>(BestPiece)->Promote();
				if (Cast<APiecePawn>(BestPiece)->GetIsFirstMove())
				{
					Cast<APiecePawn>(BestPiece)->PawnMovedForTheFirstTime();
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

			// Setting the actual tile occupied by a black, setting the old one empty
			(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);
			BestTile->SetOccupantColor(EOccupantColor::B);
			BestPiece->SetVirtualPosition(BestTile->GetGridPosition());

			// Generate the FEN string and add it to the history of moves for replays
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
						LastButton->CreateText(BestPiece, bIsACapture, BestPiece->GetVirtualPosition(), OldPosition);
					}
				}
			}

			bIsACapture = false;

			// Turn ending
			GameModeCallback->bIsBlackThinking = false;
			if (!Cast<APiecePawn>(BestPiece) || Cast<APiecePawn>(BestPiece)->GetVirtualPosition().X != 0)
			{
				GameModeCallback->TurnPlayer();
			}

		}, 3, false);
}

void ABlackMinimaxPlayer::OnWin()
{
}

ATile* ABlackMinimaxPlayer::FindBestMove()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	int32 BestVal = 1000;
	ATile* BestMove = nullptr;

	// Find the best move for the best piece to move
	for (APiece* BlackPiece : GameMode->CB->BlackPieces)
	{
		ATile* StartTile = GameMode->CB->TileMap[BlackPiece->GetVirtualPosition()];

		BlackPiece->PossibleMoves();
		BlackPiece->FilterOnlyLegalMoves();

		TArray<ATile*> MovesAndEatingPositions = BlackPiece->Moves;
		MovesAndEatingPositions.Append(BlackPiece->EatablePiecesPosition);

		for (ATile* Move : MovesAndEatingPositions)
		{
			// Manage a possible capture
			APiece* PieceCaptured = nullptr;
			if (Move->GetOccupantColor() == EOccupantColor::W)
			{
				for (APiece* WhitePiece : GameMode->CB->WhitePieces)
				{
					if (WhitePiece->GetVirtualPosition() == Move->GetGridPosition())
					{
						WhitePiece->SetVirtualPosition(FVector2D(-1, -1));
						GameMode->CB->WhitePieces.Remove(WhitePiece);
						PieceCaptured = WhitePiece;
						break;
					}
				}
			}

			// Simulate move
			StartTile->SetOccupantColor(EOccupantColor::E);
			Move->SetOccupantColor(EOccupantColor::B);
			BlackPiece->SetVirtualPosition(Move->GetGridPosition());

			// Call Maxi
			int32 MoveVal = Maxi(1);

			// Undo move
			StartTile->SetOccupantColor(EOccupantColor::B);
			Move->SetOccupantColor(EOccupantColor::E);
			BlackPiece->SetVirtualPosition(StartTile->GetGridPosition());

			// Undo a possible capture
			if (PieceCaptured)
			{
				PieceCaptured->SetVirtualPosition(Move->GetGridPosition());
				Move->SetOccupantColor(EOccupantColor::W);
				GameMode->CB->WhitePieces.Add(PieceCaptured);
				PieceCaptured = nullptr;
			}

			// Update values
			if (MoveVal < BestVal)
			{
				BestMove = Move;
				BestPiece = BlackPiece;
				BestVal = MoveVal;
			}
		}
	}

	FString MyDoubleString = FString::Printf(TEXT("%d"), BestVal);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *MyDoubleString);

	return BestMove;
}

int32 ABlackMinimaxPlayer::Mini(int32 Depth)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	// Evaluate
	if (Depth == 0)
	{
		return Evaluate();
	}

	// Start simulation for each black piece
	int Min = +99999;
	for (APiece* BlackPiece : GameMode->CB->BlackPieces)
	{
		ATile* StartTile = GameMode->CB->TileMap[BlackPiece->GetVirtualPosition()];

		BlackPiece->PossibleMoves();
		BlackPiece->FilterOnlyLegalMoves();

		TArray MovesAndEatablePositions = BlackPiece->Moves;
		MovesAndEatablePositions.Append(BlackPiece->EatablePiecesPosition);

		// For each move in moves and possible captures
		for (ATile* Move : MovesAndEatablePositions)
		{
			APiece* PieceCaptured = nullptr;

			// If it's a capture move, then simulate it
			if (Move->GetOccupantColor() == EOccupantColor::W)
			{
				for (APiece* WhitePiece : GameMode->CB->WhitePieces)
				{
					if (WhitePiece->GetVirtualPosition() == Move->GetGridPosition())
					{
						WhitePiece->SetVirtualPosition(FVector2D(-1, -1));
						GameMode->CB->WhitePieces.Remove(WhitePiece);
						PieceCaptured = WhitePiece;
						break;
					}
				}
			}

			// Simulate move
			StartTile->SetOccupantColor(EOccupantColor::E);
			Move->SetOccupantColor(EOccupantColor::B);
			BlackPiece->SetVirtualPosition(Move->GetGridPosition());

			// Call maxi
			Min = FMath::Min(Maxi(Depth - 1), Min);

			// Restore original values
			BlackPiece->SetVirtualPosition(StartTile->GetGridPosition());
			Move->SetOccupantColor(EOccupantColor::E);
			StartTile->SetOccupantColor(EOccupantColor::B);

			// Restore captured piece
			if (PieceCaptured)
			{
				PieceCaptured->SetVirtualPosition(Move->GetGridPosition());
				Move->SetOccupantColor(EOccupantColor::W);
				GameMode->CB->WhitePieces.Add(PieceCaptured);
				PieceCaptured = nullptr;
			}
		}
	}

	return Min;
}

int32 ABlackMinimaxPlayer::Maxi(int32 Depth)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	if (Depth == 0)
	{
		return Evaluate();
	}

	int Max = -99999;
	for (APiece* WhitePiece : GameMode->CB->WhitePieces)
	{
		ATile* StartTile = GameMode->CB->TileMap[WhitePiece->GetVirtualPosition()];

		WhitePiece->PossibleMoves();
		WhitePiece->FilterOnlyLegalMoves();

		TArray MovesAndEatablePositions = WhitePiece->Moves;
		MovesAndEatablePositions.Append(WhitePiece->EatablePiecesPosition);

		for (ATile* Move : MovesAndEatablePositions)
		{
			APiece* PieceCaptured = nullptr;

			if (Move->GetOccupantColor() == EOccupantColor::B)
			{
				for (APiece* BlackPiece : GameMode->CB->BlackPieces)
				{
					if (BlackPiece->GetVirtualPosition() == Move->GetGridPosition())
					{
						BlackPiece->SetVirtualPosition(FVector2D(-1, -1));
						GameMode->CB->BlackPieces.Remove(BlackPiece);
						PieceCaptured = BlackPiece;
						break;
					}
				}
			}

			StartTile->SetOccupantColor(EOccupantColor::E);
			Move->SetOccupantColor(EOccupantColor::W);
			WhitePiece->SetVirtualPosition(Move->GetGridPosition());

			Max = FMath::Max(Mini(Depth - 1), Max);

			WhitePiece->SetVirtualPosition(StartTile->GetGridPosition());
			Move->SetOccupantColor(EOccupantColor::E);
			StartTile->SetOccupantColor(EOccupantColor::W);

			if (PieceCaptured)
			{
				PieceCaptured->SetVirtualPosition(Move->GetGridPosition());
				Move->SetOccupantColor(EOccupantColor::B);
				GameMode->CB->BlackPieces.Add(PieceCaptured);
				PieceCaptured = nullptr;
			}
		}
	}

	return Max;
}

int32 ABlackMinimaxPlayer::Evaluate()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	int32 Result = 0;

	if (GameMode->VerifyCheck())
	{
		if (GameMode->GetIsBlackOnCheck())
		{
			if (GameMode->VerifyCheckmate())
			{
				Result = 100;
			}
			Result = 10;
		}
		if (GameMode->GetIsWhiteOnCheck())
		{
			if (GameMode->VerifyCheckmate())
			{
				Result = -100;
			}
			Result = -10;
		}
	}
	else if (GameMode->VerifyDraw())
	{
		Result = 0;
	}
	else
	{
		for (APiece* WhitePiece : GameMode->CB->WhitePieces)
		{
			Result += WhitePiece->GetPieceValue();
		}
		for (APiece* BlackPiece : GameMode->CB->BlackPieces)
		{
			Result -= BlackPiece->GetPieceValue();
		}
	}
	
	FString MyDoubleString = FString::Printf(TEXT("%d"), Result);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *MyDoubleString);

	return Result;
}
