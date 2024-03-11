// Fill out your copyright notice in the Description page of Project Settings.


#include "WhitePlayer.h"
#include "Piece.h"
#include "PiecePawn.h"
#include "ChessGameMode.h"
#include "ChessPlayerController.h"

// Sets default values
AWhitePlayer::AWhitePlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// create a camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//set the camera as RootComponent
	SetRootComponent(Camera);

	PlayerNumber = 0;

	GameInstance = Cast<UChessGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

// Called when the game starts or when spawned
void AWhitePlayer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWhitePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AWhitePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AWhitePlayer::PieceSelection()
{
	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());
	FString LastMoveDone = GameMode->CB->HistoryOfMoves.Last();

	if (GameMode->bIsWhiteOnCheck)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("White is on Check!"));
	}

	// Detecting player's click
	FHitResult Hit = FHitResult(ForceInit);
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, GameMode->CB->GenerateStringFromPositions());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, LastMoveDone);

	if (Hit.bBlockingHit && IsMyTurn && (GameMode->CB->GenerateStringFromPositions() == LastMoveDone))
	{
		if (APiece* PieceClicked = Cast<APiece>(Hit.GetActor()))
		{
			/* Calculate moves and color the tiles
			if (CPC->SelectedPieceToMove != nullptr)
			{
				CPC->SelectedPieceToMove->ColorPossibleMoves();
			}
			*/

			// Ally Piece
			if (PieceClicked->Color == EColor::W)
			{
				// Save the piece
				CPC->SelectedPieceToMove = PieceClicked;

				// Deleting possible old colorations
				PieceClicked->DecolorPossibleMoves();
				PieceClicked->ColorPossibleMoves();
			}
			// Enemy Piece
			else if (PieceClicked->Color == EColor::B && CPC->SelectedPieceToMove != nullptr)
			{
				ATile** TilePtr = GameMode->CB->TileMap.Find(FVector2D(PieceClicked->RelativePosition().X, PieceClicked->RelativePosition().Y));

				if (CPC->SelectedPieceToMove->EatablePieces.Contains(*TilePtr))
				{
					GameMode->CB->BlackPieces.Remove(PieceClicked);
					PieceClicked->PieceCaptured();
					(*TilePtr)->SetOccupantColor(EOccupantColor::E);
					TileSelection(*TilePtr);
				}
			}
		}
		else if (ATile* TileClicked = Cast<ATile>(Hit.GetActor()))
		{
			if (CPC->SelectedPieceToMove != nullptr)
			{
				TileSelection(TileClicked);
			}
		}
	}
}

void AWhitePlayer::TileSelection(ATile* CurrTile)
{
	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());
	ATile** PreviousTilePtr = GameMode->CB->TileMap.Find(FVector2D(CPC->SelectedPieceToMove->RelativePosition().X, CPC->SelectedPieceToMove->RelativePosition().Y));
	ATile** ActualTilePtr = GameMode->CB->TileMap.Find(CurrTile->GetGridPosition());

	if (CurrTile->GetOccupantColor() == EOccupantColor::E)
	{
		// If a tile is clicked, decolor possible moves
		CPC->SelectedPieceToMove->DecolorPossibleMoves();

		// If the move is legal, move the piece
		if (CPC->SelectedPieceToMove->Moves.Contains(CurrTile) || CPC->SelectedPieceToMove->EatablePieces.Contains(CurrTile))
		{
			FVector ActorPositioning = GameMode->CB->GetRelativeLocationByXYPosition(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y);
			ActorPositioning.Z = 10.0f;
			CPC->SelectedPieceToMove->SetActorLocation(ActorPositioning);

			if (Cast<APiecePawn>(CPC->SelectedPieceToMove) && Cast<APiecePawn>(CPC->SelectedPieceToMove)->bFirstMove == true)
			{
				Cast<APiecePawn>(CPC->SelectedPieceToMove)->bFirstMove = false;
			}
		}

		// Setting the actual tile occupied by a white, setting the old one empty
		if (CPC->SelectedPieceToMove->RelativePosition() == FVector(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y, 10.f))
		{
			(*ActualTilePtr)->SetOccupantColor(EOccupantColor::W);
			(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);

			// Generate the FEN string and add it to the history of moves for replays
			FString LastMove = GameMode->CB->GenerateStringFromPositions();
			GameMode->CB->HistoryOfMoves.Add(LastMove);

			// Turn ending
			IsMyTurn = false;
			CPC->SelectedPieceToMove = nullptr;
			GameMode->TurnPlayer(this);
		}
	}
}

void AWhitePlayer::OnTurn()
{
	IsMyTurn = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Your Turn"));
	GameInstance->SetTurnMessage(TEXT("Human Turn"));
}

void AWhitePlayer::OnWin()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Win!"));
	GameInstance->SetTurnMessage(TEXT("Human Wins!"));
	GameInstance->IncrementScoreHumanPlayer();
}

void AWhitePlayer::OnLose()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Lose!"));
	GameInstance->SetTurnMessage(TEXT("Human Loses!"));
}

bool AWhitePlayer::IsCheckStatus()
{
	return false;
}
