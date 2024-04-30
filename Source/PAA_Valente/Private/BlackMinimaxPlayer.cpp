// Fill out your copyright notice in the Description page of Project Settings. a


#include "BlackMinimaxPlayer.h"
#include "Piece.h"
#include "PiecePawn.h"
#include "EngineUtils.h"
#include "MainHUD.h"
#include "ChessPlayerController.h"
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

bool ABlackMinimaxPlayer::GetThinkingStatus() const
{
	return bThinking;
}

void ABlackMinimaxPlayer::DestroyPlayer()
{
	this->Destroy();
}

void ABlackMinimaxPlayer::SetTeam(EColor TeamColor)
{
	AChessGameMode* GameMode = (AChessGameMode*)(GetWorld()->GetAuthGameMode());

	AllyColor = TeamColor;
	AllyOccupantColor = (TeamColor == EColor::W) ? EOccupantColor::W : EOccupantColor::B;
	AllyPieces = (TeamColor == EColor::W) ? &GameMode->CB->WhitePieces : &GameMode->CB->BlackPieces;
}

FTimerHandle* ABlackMinimaxPlayer::GetTimerHandle()
{
	FTimerHandle* HandlerPtr = &TimerHandle;
	return HandlerPtr;
}

void ABlackMinimaxPlayer::OnTurn()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Minimax) Turn"));
	GameInstance->SetTurnMessage(TEXT("AI (Minimax) Turn"));

	// Set thinking state to avoid replay when black's moving
	bThinking = true;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			// Declarations
			AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
			AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());
			UMainHUD* MainHUD = CPC->MainHUDWidget;

			// Find enemy team attributes
			TArray<APiece*>* EnemyPieces = (AllyColor == EColor::W) ? &GameMode->CB->BlackPieces : &GameMode->CB->WhitePieces;
			EColor EnemyColor = (AllyColor == EColor::W) ? EColor::B : EColor::W;
			EOccupantColor EnemyOccupantColor = (AllyColor == EColor::W) ? EOccupantColor::B : EOccupantColor::W;

			// Find the best move with the best piece
			ATile* BestTile = FindBestMove();

			// Getting previous tile
			ATile* PreviousTilePtr = GameMode->CB->TileMap[BestPiece->GetVirtualPosition()];
			FVector2D OldPosition = BestPiece->GetVirtualPosition();
			// Getting the new tile and the new position
			FVector TilePositioning = GameMode->CB->GetRelativeLocationByXYPosition(BestTile->GetGridPosition().X, BestTile->GetGridPosition().Y);
			TilePositioning.Z = 10.0f;

			// If it's an eating move, then delete the white piece
			if (BestTile->GetOccupantColor() == AllyOccupantColor)
			{
				// Search the white piece who occupies the tile and capture it
				for (APiece* EnemyPiece : *EnemyPieces)
				{
					if (EnemyPiece->GetActorLocation() == TilePositioning)
					{
						EnemyPieces->Remove(EnemyPiece);
						EnemyPiece->Destroy();
						bIsACapture = true;
						break;
					}
				}
			}

			// Moving the piece
			BestPiece->SetActorLocation(TilePositioning);
			BestPiece->SetVirtualPosition(BestTile->GetGridPosition());

			// Promote procedure
			if (!BestPiece->GetWasMoved())
			{
				// To comunicate the first move
				BestPiece->SetWasMoved(true);
			}

			// Pawn tie / promote check procedure
			if (BestPiece->IsA<APiecePawn>())
			{
				Cast<APiecePawn>(BestPiece)->Promote();
				// To comunicate a general move for 50 moves rule
				GameMode->APawnHasMoved();
			}

			// Setting the actual tile occupied by a black, setting the old one empty
			PreviousTilePtr->SetOccupantColor(EOccupantColor::E);
			BestTile->SetOccupantColor(AllyOccupantColor);

			// Generate the FEN string and add it to the history of moves for replays
			FString LastMove = GameMode->CB->GenerateStringFromPositions();
			GameMode->CB->HistoryOfMoves.Add(LastMove);

			// Create dinamically the move button
			if (MainHUD)
			{
				MainHUD->AddButton(LastMove, BestPiece, bIsACapture, BestPiece->GetVirtualPosition(), OldPosition);
			}

			bIsACapture = false;

			// Turn ending
			bThinking = false;
			if (!Cast<APiecePawn>(BestPiece) || Cast<APiecePawn>(BestPiece)->GetVirtualPosition().X != 0)
			{
				GameMode->TurnPlayer();
			}

		}, 3, false);
}

void ABlackMinimaxPlayer::OnWin()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Minimax) Wins!"));
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
	GameInstance->IncrementScoreAiPlayer();
}

ATile* ABlackMinimaxPlayer::FindBestMove()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	// Find enemy team attributes
	TArray<APiece*>* EnemyPieces = (AllyColor == EColor::W) ? &GameMode->CB->BlackPieces : &GameMode->CB->WhitePieces;
	EColor EnemyColor = (AllyColor == EColor::W) ? EColor::B : EColor::W;
	EOccupantColor EnemyOccupantColor = (AllyColor == EColor::W) ? EOccupantColor::B : EOccupantColor::W;

	int32 BestVal = 1000;
	ATile* BestMove = nullptr;

	// Find the best move for the best piece to move
	for (APiece* AllyPiece : *AllyPieces)
	{
		ATile* StartTile = GameMode->CB->TileMap[AllyPiece->GetVirtualPosition()];

		AllyPiece->PossibleMoves();
		AllyPiece->FilterOnlyLegalMoves();

		TArray OriginalMoves = AllyPiece->Moves;

		for (ATile* Move : OriginalMoves)
		{
			// Manage a possible capture
			APiece* PieceCaptured = nullptr;
			if (Move->GetOccupantColor() == EnemyOccupantColor)
			{
				for (APiece* EnemyPiece : *EnemyPieces)
				{
					if (EnemyPiece->GetVirtualPosition() == Move->GetGridPosition())
					{
						EnemyPiece->SetVirtualPosition(FVector2D(-1, -1));
						PieceCaptured = EnemyPiece;
						break;
					}
				}
			}

			// Simulate move
			StartTile->SetOccupantColor(EOccupantColor::E);
			Move->SetOccupantColor(AllyOccupantColor);
			AllyPiece->SetVirtualPosition(Move->GetGridPosition());

			// Call Maxi
			int32 MoveVal = Maxi(1, -9999, 9999);

			// Undo move
			StartTile->SetOccupantColor(AllyOccupantColor);
			Move->SetOccupantColor(EOccupantColor::E);
			AllyPiece->SetVirtualPosition(StartTile->GetGridPosition());

			// Undo a possible capture
			if (PieceCaptured)
			{
				PieceCaptured->SetVirtualPosition(Move->GetGridPosition());
				Move->SetOccupantColor(EnemyOccupantColor);
				PieceCaptured = nullptr;
			}

			// Update values
			if (MoveVal < BestVal)
			{
				BestMove = Move;
				BestPiece = AllyPiece;
				BestVal = MoveVal;
			}
		}
		OriginalMoves.Empty();
	}

	return BestMove;
}

int32 ABlackMinimaxPlayer::Mini(int32 Depth, int32 Alpha, int32 Beta)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	// Find enemy team attributes
	TArray<APiece*>* EnemyPieces = (AllyColor == EColor::W) ? &GameMode->CB->BlackPieces : &GameMode->CB->WhitePieces;
	EColor EnemyColor = (AllyColor == EColor::W) ? EColor::B : EColor::W;
	EOccupantColor EnemyOccupantColor = (AllyColor == EColor::W) ? EOccupantColor::B : EOccupantColor::W;

	// Evaluate
	if (Depth == 0)
	{
		return Evaluate();
	}

	// Start simulation for each black piece
	int Min = +99999;
	for (APiece* AllyPiece : *AllyPieces)
	{
		// (-1,-1) is a position that determines a virtual capture, then ignore that piece
		if (AllyPiece->GetVirtualPosition() == FVector2D(-1, -1))
		{
			continue;
		}

		ATile* StartTile = GameMode->CB->TileMap[AllyPiece->GetVirtualPosition()];

		AllyPiece->PossibleMoves();
		AllyPiece->FilterOnlyLegalMoves();

		TArray OriginalMoves = AllyPiece->Moves;

		// For each move in moves and possible captures
		for (ATile* Move : OriginalMoves)
		{
			APiece* PieceCaptured = nullptr;

			// If it's a capture move, then simulate it
			if (Move->GetOccupantColor() == EnemyOccupantColor)
			{
				for (APiece* EnemyPiece : *EnemyPieces)
				{
					if (EnemyPiece->GetVirtualPosition() == Move->GetGridPosition())
					{
						EnemyPiece->SetVirtualPosition(FVector2D(-1, -1));
						PieceCaptured = EnemyPiece;
						break;
					}
				}
			}

			// Simulate move
			StartTile->SetOccupantColor(EOccupantColor::E);
			Move->SetOccupantColor(AllyOccupantColor);
			AllyPiece->SetVirtualPosition(Move->GetGridPosition());

			// Call maxi with updated alpha and beta values
			Min = FMath::Min(Maxi(Depth - 1, Alpha, Beta), Min);

			// Restore original values
			AllyPiece->SetVirtualPosition(StartTile->GetGridPosition());
			Move->SetOccupantColor(EOccupantColor::E);
			StartTile->SetOccupantColor(AllyOccupantColor);

			// Restore captured piece
			if (PieceCaptured)
			{
				PieceCaptured->SetVirtualPosition(Move->GetGridPosition());
				Move->SetOccupantColor(EnemyOccupantColor);
				PieceCaptured = nullptr;
			}

			// Alpha-beta pruning
			Beta = FMath::Min(Beta, Min);
			if (Beta <= Alpha)
			{
				break;
			}
		}
		OriginalMoves.Empty();
	}

	return Min;
}

// Maxi should be commented as Mini
int32 ABlackMinimaxPlayer::Maxi(int32 Depth, int32 Alpha, int32 Beta)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	TArray<APiece*>* EnemyPieces = (AllyColor == EColor::W) ? &GameMode->CB->BlackPieces : &GameMode->CB->WhitePieces;
	EColor EnemyColor = (AllyColor == EColor::W) ? EColor::B : EColor::W;
	EOccupantColor EnemyOccupantColor = (AllyColor == EColor::W) ? EOccupantColor::B : EOccupantColor::W;

	if (Depth == 0)
	{
		return Evaluate();
	}

	int Max = -99999;
	for (APiece* EnemyPiece : *EnemyPieces)
	{
		if (EnemyPiece->GetVirtualPosition() == FVector2D(-1, -1))
		{
			continue;
		}

		ATile* StartTile = GameMode->CB->TileMap[EnemyPiece->GetVirtualPosition()];

		EnemyPiece->PossibleMoves();
		EnemyPiece->FilterOnlyLegalMoves();

		TArray OriginalMoves = EnemyPiece->Moves;

		for (ATile* Move : OriginalMoves)
		{
			APiece* PieceCaptured = nullptr;

			if (Move->GetOccupantColor() == AllyOccupantColor)
			{
				for (APiece* AllyPiece : *AllyPieces)
				{
					if (AllyPiece->GetVirtualPosition() == Move->GetGridPosition())
					{
						AllyPiece->SetVirtualPosition(FVector2D(-1, -1));
						PieceCaptured = AllyPiece;
						break;
					}
				}
			}

			StartTile->SetOccupantColor(EOccupantColor::E);
			Move->SetOccupantColor(EnemyOccupantColor);
			EnemyPiece->SetVirtualPosition(Move->GetGridPosition());

			Max = FMath::Max(Mini(Depth - 1, Alpha, Beta), Max);

			EnemyPiece->SetVirtualPosition(StartTile->GetGridPosition());
			Move->SetOccupantColor(EOccupantColor::E);
			StartTile->SetOccupantColor(EnemyOccupantColor);

			if (PieceCaptured)
			{
				PieceCaptured->SetVirtualPosition(Move->GetGridPosition());
				Move->SetOccupantColor(AllyOccupantColor);
				PieceCaptured = nullptr;
			}

			Alpha = FMath::Max(Alpha, Max);
			if (Beta <= Alpha)
			{
				break;
			}
		}
		OriginalMoves.Empty();
	}

	return Max;
}

int32 ABlackMinimaxPlayer::Evaluate()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	int32 Result = 0;

	// Check / Win / Lose case
	if (GameMode->VerifyCheck())
	{
		if (GameMode->GetTurnFlag() == 0)
		{
			if (GameMode->VerifyCheckmate())
			{
				Result = 100;
			}
			Result = 10;
		}
		if (GameMode->GetTurnFlag() == 1)
		{
			if (GameMode->VerifyCheckmate())
			{
				Result = -100;
			}
			Result = -10;
		}
	}

	// Draw case
	else if (GameMode->VerifyDraw())
	{
		Result = 0;
	}

	// Normal move case
	else
	{
		int32 RemainingPiecesValues = 0;
		int32 MobilityValues = 0;
		int32 CenterControlValue = 0;

		for (APiece* WhitePiece : GameMode->CB->WhitePieces)
		{
			RemainingPiecesValues += WhitePiece->GetPieceValue();

			WhitePiece->PossibleMoves();
			WhitePiece->FilterOnlyLegalMoves();

			MobilityValues += WhitePiece->Moves.Num();
		}
		for (APiece* BlackPiece : GameMode->CB->BlackPieces)
		{
			RemainingPiecesValues -= BlackPiece->GetPieceValue();

			BlackPiece->PossibleMoves();
			BlackPiece->FilterOnlyLegalMoves();

			MobilityValues -= BlackPiece->Moves.Num();
		}

		Result = (RemainingPiecesValues * 10 + MobilityValues * 5) / (10 + 5);
	}

	return Result;
}
