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
			// Declarations
			AChessGameMode* GameModeCallback = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
			AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());
			APiece* ChosenPiece = nullptr;
			TArray<ATile*> MovesAndEatablePieces;
			UMainHUD* MainHUD = CPC->MainHUDWidget;

			Minimax(2, false);

			// Getting previous tile
			ATile** PreviousTilePtr = GameModeCallback->CB->TileMap.Find(BestPieceToMove->GetVirtualPosition());
			FVector2D OldPosition = BestPieceToMove->GetVirtualPosition();

			// Getting the new tile and the new position
			FVector TilePositioning = GameModeCallback->CB->GetRelativeLocationByXYPosition(BestTileToMove->GetGridPosition().X, BestTileToMove->GetGridPosition().Y);
			TilePositioning.Z = 10.0f;

			// If it's an eating move, then delete the white piece
			if (BestTileToMove->GetOccupantColor() == EOccupantColor::W)
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
			BestPieceToMove->SetActorLocation(TilePositioning);
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

			// Setting the actual tile occupied by a black, setting the old one empty
			(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);
			BestTileToMove->SetOccupantColor(EOccupantColor::B);
			BestPieceToMove->SetVirtualPosition(BestTileToMove->GetGridPosition());

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
						LastButton->CreateText(BestPieceToMove, bIsACapture, BestPieceToMove->GetVirtualPosition(), OldPosition);
					}
				}
			}

			bIsACapture = false;

			// Turn ending
			GameModeCallback->bIsBlackThinking = false;
			if (!Cast<APiecePawn>(BestPieceToMove) || Cast<APiecePawn>(BestPieceToMove)->GetVirtualPosition().X != 0)
			{
				GameModeCallback->TurnPlayer();
			}

		}, 3, false);
}

void ABlackMinimaxPlayer::OnWin()
{
}

int32 ABlackMinimaxPlayer::Mini(int32 Depth)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	int32 StartDepth = Depth;

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
			int32 Score = Maxi(Depth - 1);
			if (Score < Min)
			{
				Min = Score;
				// Store best values to move
				if (Depth == StartDepth)
				{
					BestPieceToMove = BlackPiece;
					BestTileToMove = Move;
				}
			}

			// Restore original values
			BlackPiece->SetVirtualPosition(StartTile->GetGridPosition());
			Move->SetOccupantColor(EOccupantColor::E);
			StartTile->SetOccupantColor(EOccupantColor::B);

			// Restore captured piece
			if (PieceCaptured)
			{
				PieceCaptured->SetVirtualPosition(Move->GetGridPosition());
				GameMode->CB->WhitePieces.Add(PieceCaptured);
				PieceCaptured = nullptr;
			}
		}
	}

	return Min;
}

// Same as mini, but without storing best values (the best piece to move can't be white)
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

			if (Move->GetOccupantColor() == EOccupantColor::W)
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

			int32 Score = Mini(Depth - 1);
			if (Score > Max)
			{
				Max = Score;
			}

			WhitePiece->SetVirtualPosition(StartTile->GetGridPosition());
			Move->SetOccupantColor(EOccupantColor::E);
			StartTile->SetOccupantColor(EOccupantColor::W);

			if (PieceCaptured)
			{
				PieceCaptured->SetVirtualPosition(Move->GetGridPosition());
				GameMode->CB->BlackPieces.Add(PieceCaptured);
				PieceCaptured = nullptr;
			}
		}
	}

	return Max;
}



void ABlackMinimaxPlayer::Minimax(int32 Depth, bool IsMax)
{
	if (IsMax)
	{
		Maxi(Depth);
	}
	else
	{
		Mini(Depth);
	}
}

int32 ABlackMinimaxPlayer::Evaluate()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode->VerifyCheck())
	{
		if (GameMode->GetIsBlackOnCheck())
		{
			if (GameMode->VerifyCheckmate())
			{
				return 100;
			}
			return 10;
		}
		if (GameMode->GetIsWhiteOnCheck())
		{
			if (GameMode->VerifyCheckmate())
			{
				return -100;
			}
			return -10;
		}
	}
	else if (GameMode->VerifyDraw())
	{
		return 0;
	}
	else
	{
		bool WhiteCanEat = false;
		bool BlackCanEat = false;

		int32 BestWhiteEatablePieceValue = 0;
		int32 BestBlackEatablePieceValue = 0;

		for (APiece* BlackPiece : GameMode->CB->BlackPieces)
		{
			if (!BlackPiece->EatablePiecesPosition.IsEmpty())
			{
				BlackCanEat = true;
				for (APiece* WhitePiece : GameMode->CB->WhitePieces)
				{
					ATile* WhitePiecePosition = GameMode->CB->TileMap[WhitePiece->GetVirtualPosition()];
					if (BlackPiece->EatablePiecesPosition.Contains(WhitePiecePosition))
					{
						int32 CurrentEatablePieceValue = WhitePiece->GetPieceValue();
						BestWhiteEatablePieceValue = FMath::Max(BestWhiteEatablePieceValue, CurrentEatablePieceValue);
					}
				}
			}
		}

		for (APiece* WhitePiece : GameMode->CB->WhitePieces)
		{
			if (!WhitePiece->EatablePiecesPosition.IsEmpty())
			{
				WhiteCanEat = true;
				for (APiece* BlackPiece : GameMode->CB->BlackPieces)
				{
					ATile* BlackPiecePosition = GameMode->CB->TileMap[BlackPiece->GetVirtualPosition()];
					if (WhitePiece->EatablePiecesPosition.Contains(BlackPiecePosition))
					{
						int32 CurrentEatablePieceValue = BlackPiece->GetPieceValue();
						BestBlackEatablePieceValue = FMath::Min(BestBlackEatablePieceValue, CurrentEatablePieceValue);
					}
				}
			}
		}

		int32 Result = BestWhiteEatablePieceValue - BestBlackEatablePieceValue;

		return -Result;
	}

	return int32();
}
