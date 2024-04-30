// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackRandomPlayer.h"
#include "Piece.h"
#include "PiecePawn.h"
#include "PieceRook.h"
#include "PieceKing.h"
#include "EngineUtils.h"
#include "MainHUD.h"
#include "ChessPlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Sets default values
ABlackRandomPlayer::ABlackRandomPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GameInstance = Cast<UChessGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	bIsACapture = false;
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

bool ABlackRandomPlayer::GetThinkingStatus() const
{
	return bThinking;
}

void ABlackRandomPlayer::DestroyPlayer()
{
	this->Destroy();
}

void ABlackRandomPlayer::SetTeam(EColor TeamColor)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	AllyColor = TeamColor;
	AllyOccupantColor = (TeamColor == EColor::W) ? EOccupantColor::W : EOccupantColor::B;
	AllyPieces = (TeamColor == EColor::W) ? &GameMode->CB->WhitePieces : &GameMode->CB->BlackPieces;
}

FTimerHandle* ABlackRandomPlayer::GetTimerHandle()
{
	FTimerHandle* HandlerPtr = &TimerHandle;
	return HandlerPtr;
}

void ABlackRandomPlayer::OnTurn()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
	GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));

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

			// Picking a random piece
			APiece* ChosenPiece = nullptr;
			do
			{
				int32 RandIdx0 = FMath::Rand() % AllyPieces->Num();
				ChosenPiece = (*AllyPieces)[RandIdx0];

				// Calculate piece's possible moves.
				ChosenPiece->PossibleMoves();
				ChosenPiece->FilterOnlyLegalMoves();

			} while (ChosenPiece->Moves.Num() == 0);

			// Getting previous tile
			ATile* PreviousTilePtr = GameMode->CB->TileMap[ChosenPiece->GetVirtualPosition()];
			FVector2D OldPosition = ChosenPiece->GetVirtualPosition();

			// Getting the new tile and the new position
			int32 RandIdx1 = FMath::Rand() % ChosenPiece->Moves.Num();
			ATile* DestinationTile = ChosenPiece->Moves[RandIdx1];
			FVector TilePositioning = GameMode->CB->GetRelativeLocationByXYPosition(DestinationTile->GetGridPosition().X, DestinationTile->GetGridPosition().Y);
			TilePositioning.Z = 10.0f;

			// If it's an eating move, then delete the white piece
			if (DestinationTile->GetOccupantColor() == EnemyOccupantColor)
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
			else if (ChosenPiece->IsA<APieceKing>() && DestinationTile->GetOccupantColor() == AllyOccupantColor)
			{
				APiece* RookToCastleWith = nullptr;
				for (APiece* Rook : Cast<APieceKing>(ChosenPiece)->Rooks)
				{
					if (Rook->GetVirtualPosition() == DestinationTile->GetGridPosition())
					{
						RookToCastleWith = Rook;
						break;
					}
				}
				bThinking = false;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Il nero arrocca"));
				Cast<APieceKing>(ChosenPiece)->PerformCastling(RookToCastleWith);
				return;
			}

			// Moving the piece
			ChosenPiece->SetActorLocation(TilePositioning);
			ChosenPiece->SetVirtualPosition(DestinationTile->GetGridPosition());

			// First move procedure
			if (!ChosenPiece->GetWasMoved())
			{
				// To comunicate the first move
				ChosenPiece->SetWasMoved(true);
			}

			// Pawn tie / promote check procedure
			if (ChosenPiece->IsA<APiecePawn>())
			{
				Cast<APiecePawn>(ChosenPiece)->Promote();
				// To comunicate a general move for 50 moves rule
				GameMode->APawnHasMoved();
			}

			// Setting the actual tile occupied by a black, setting the old one empty
			PreviousTilePtr->SetOccupantColor(EOccupantColor::E);
			DestinationTile->SetOccupantColor(AllyOccupantColor);

			// Generate the FEN string and add it to the history of moves for replays
			FString LastMove = GameMode->CB->GenerateStringFromPositions();
			GameMode->CB->HistoryOfMoves.Add(LastMove);

			// Create dinamically the move button
			if (MainHUD)
			{
				MainHUD->AddButton(LastMove, ChosenPiece, bIsACapture, ChosenPiece->GetVirtualPosition(), OldPosition);
			}

			bIsACapture = false;

			// Turn ending
			bThinking = false;
			if (!Cast<APiecePawn>(ChosenPiece) || Cast<APiecePawn>(ChosenPiece)->GetVirtualPosition().X != 0)
			{
				GameMode->TurnPlayer();
			}


		}, 3, false);
}

void ABlackRandomPlayer::OnWin()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Wins!"));
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
	GameInstance->IncrementScoreAiPlayer();
}
