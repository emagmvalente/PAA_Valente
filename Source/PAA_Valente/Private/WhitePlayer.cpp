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

void AWhitePlayer::PieceClicked()
{
	// Declarations
	AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());

	// Detecting player's click
	FHitResult Hit = FHitResult(ForceInit);
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

	if (Hit.bBlockingHit && IsMyTurn)
	{
		if (APiece* CurrPiece = Cast<APiece>(Hit.GetActor()))
		{
			if (CurrPiece->Color == EColor::W)
			{
				// Save the piece
				CPC->SelectedPieceToMove = CurrPiece;

				// Deleting possible old colorations
				CurrPiece->DecolorPossibleMoves();
			}
		}
	}
}

void AWhitePlayer::TileSelection()
{
	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());

	// Detecting player's click
	FHitResult Hit = FHitResult(ForceInit);
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

	// Calculate moves and color the tiles
	if (CPC->SelectedPieceToMove != nullptr)
	{
		CPC->SelectedPieceToMove->ColorPossibleMoves();
	}

	if (Hit.bBlockingHit && IsMyTurn)
	{
		// Movement tile to tile logic
		if (ATile* CurrTile = Cast<ATile>(Hit.GetActor()))
		{
			if (CurrTile->GetTileStatus() == ETileStatus::EMPTY && CPC->SelectedPieceToMove != nullptr)
			{
				// If a tile is clicked, decolor possible moves
				CPC->SelectedPieceToMove->DecolorPossibleMoves();

				// Declarations
				FVector PreviousLocation = CPC->SelectedPieceToMove->RelativePosition();
				ATile** PreviousTilePtr = GameMode->CB->TileMap.Find(FVector2D(PreviousLocation.X, PreviousLocation.Y));
				FVector2D TilePosition = CurrTile->GetGridPosition();
				ATile** ActualTilePtr = GameMode->CB->TileMap.Find(TilePosition);

				// Calculating PossibleMoves and populating Moves array
				CPC->SelectedPieceToMove->PossibleMoves();

				// If the selected tile is in Moves, then allow the move
				FVector2D CurrTilePosition = CurrTile->GetGridPosition();
				
				// If the move is legal, move the piece
				if (CPC->SelectedPieceToMove->Moves.Contains(CurrTile))
				{
					FVector ActorPositioning = GameMode->CB->GetRelativeLocationByXYPosition(CurrTilePosition.X, CurrTilePosition.Y);
					ActorPositioning.Z = 10.0f;
					CPC->SelectedPieceToMove->SetActorLocation(ActorPositioning);

					if (Cast<APiecePawn>(CPC->SelectedPieceToMove) && Cast<APiecePawn>(CPC->SelectedPieceToMove)->bFirstMove == true)
					{
						Cast<APiecePawn>(CPC->SelectedPieceToMove)->bFirstMove = false;
					}
				}

				// Setting the actual tile occupied by a white, setting the old one empty
				if (CPC->SelectedPieceToMove->RelativePosition() == FVector(CurrTilePosition.X, CurrTilePosition.Y, 10.f))
				{
					(*ActualTilePtr)->SetTileStatus(ETileStatus::OCCUPIED);
					(*ActualTilePtr)->SetOccupantColor(EOccupantColor::W);

					(*PreviousTilePtr)->SetTileStatus(ETileStatus::EMPTY);
					(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);

					// Generate the FEN string and add it to the history of moves for replays
					FString LastMove = GameMode->CB->GenerateStringFromPositions();
					GameMode->CB->HistoryOfMoves.Add(LastMove);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LastMove);

					// Turn ending
					IsMyTurn = false;
					GameMode->TurnPlayer(this);
				}

				CPC->SelectedPieceToMove = nullptr;
			}
		}

		// Eating logic
		if (APiece* PieceToEat = Cast<APiece>(Hit.GetActor()))
		{
			if (PieceToEat->Color == EColor::B && CPC->SelectedPieceToMove != nullptr)
			{
				// If a tile is clicked, decolor possible moves
				CPC->SelectedPieceToMove->DecolorPossibleMoves();

				// Declarations
				FVector PreviousLocation = CPC->SelectedPieceToMove->RelativePosition();
				ATile** PreviousTilePtr = GameMode->CB->TileMap.Find(FVector2D(PreviousLocation.X, PreviousLocation.Y));
				FVector ActualLocation = PieceToEat->RelativePosition();
				ATile** ActualTilePtr = GameMode->CB->TileMap.Find(FVector2D(ActualLocation.X, ActualLocation.Y));

				// Calculating PossibleMoves and populating Moves array
				CPC->SelectedPieceToMove->PossibleMoves();

				if (CPC->SelectedPieceToMove->EatablePieces.Contains((*ActualTilePtr)))
				{
					// Delete the captured piece
					PieceToEat->PieceCaptured();

					// Move the piece on the new tile
					FVector ActorPositioning = GameMode->CB->GetRelativeLocationByXYPosition(ActualLocation.X, ActualLocation.Y);
					ActorPositioning.Z = 10.0f;
					CPC->SelectedPieceToMove->SetActorLocation(ActorPositioning);

					// Setting the actual tile occupied by a white, setting the old one empty
					(*ActualTilePtr)->SetTileStatus(ETileStatus::OCCUPIED);
					(*ActualTilePtr)->SetOccupantColor(EOccupantColor::W);

					(*PreviousTilePtr)->SetTileStatus(ETileStatus::EMPTY);
					(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);

					// Generate the FEN string and add it to the history of moves for replays
					FString LastMove = GameMode->CB->GenerateStringFromPositions();
					GameMode->CB->HistoryOfMoves.Add(LastMove);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LastMove);

					// Turn ending
					IsMyTurn = false;
					GameMode->CB->BlackPieces.Remove(PieceToEat);
					CPC->SelectedPieceToMove = nullptr;
					GameMode->TurnPlayer(this);

				}
			}
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

