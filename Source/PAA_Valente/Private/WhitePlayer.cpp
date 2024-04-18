// Fill out your copyright notice in the Description page of Project Settings.


#include "WhitePlayer.h"
#include "Piece.h"
#include "PiecePawn.h"
#include "ChessGameMode.h"
#include "ChessPlayerController.h"
#include "MainHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

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

	GameInstance = Cast<UChessGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	
	bIsACapture = false;
	IsMyTurn = false;
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

	// Detecting player's click
	FHitResult Hit = FHitResult(ForceInit);
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

	if (Hit.bBlockingHit && IsMyTurn && (GameMode->CB->GenerateStringFromPositions() == LastMoveDone))
	{
		if (APiece* PieceClicked = Cast<APiece>(Hit.GetActor()))
		{
			// Ally Piece
			if (PieceClicked->GetColor() == EColor::W)
			{
				// Save the piece
				CPC->SelectedPieceToMove = PieceClicked;

				// Deleting possible old colorations
				PieceClicked->DecolorPossibleMoves();
				PieceClicked->ColorPossibleMoves();
			}
			// Enemy Piece
			else if (PieceClicked->GetColor() == EColor::B && CPC->SelectedPieceToMove != nullptr)
			{
				if (GameMode->CB->TileMap.Contains(PieceClicked->GetVirtualPosition()))
				{
					ATile* DestinationTile = GameMode->CB->TileMap[PieceClicked->GetVirtualPosition()];

					if (CPC->SelectedPieceToMove->Moves.Contains(DestinationTile))
					{
						GameMode->CB->BlackPieces.Remove(PieceClicked);
						PieceClicked->Destroy();
						bIsACapture = true;
						DestinationTile->SetOccupantColor(EOccupantColor::E);
						TileSelection(DestinationTile);
					}
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

	else if (Hit.bBlockingHit && IsMyTurn && (GameMode->CB->GenerateStringFromPositions() != LastMoveDone))
	{
		if (CPC->SelectedPieceToMove)
		{
			CPC->SelectedPieceToMove->DecolorPossibleMoves();
		}
		GameMode->CB->GeneratePositionsFromString(LastMoveDone);
		GameMode->CB->SetTilesOwners();
		GameMode->SetKings();
	}
}

void AWhitePlayer::TileSelection(ATile* CurrTile)
{
	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());
	FVector2D OldPosition = CPC->SelectedPieceToMove->GetVirtualPosition();
	ATile* PreviousTilePtr = GameMode->CB->TileMap[OldPosition];
	ATile* ActualTilePtr = GameMode->CB->TileMap[CurrTile->GetGridPosition()];
	UMainHUD* MainHUD = CPC->MainHUDWidget;

	if (CurrTile->GetOccupantColor() == EOccupantColor::E)
	{
		// If a tile is clicked, decolor possible moves
		CPC->SelectedPieceToMove->DecolorPossibleMoves();

		// If the move is legal, move the piece
		if (CPC->SelectedPieceToMove->Moves.Contains(CurrTile))
		{
			FVector ActorPositioning = GameMode->CB->GetRelativeLocationByXYPosition(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y);
			ActorPositioning.Z = 10.0f;
			CPC->SelectedPieceToMove->SetActorLocation(ActorPositioning);
			CPC->SelectedPieceToMove->SetVirtualPosition(ActualTilePtr->GetGridPosition());

			// Pawn tie / promote check procedure
			if (CPC->SelectedPieceToMove->IsA<APiecePawn>())
			{
				if (Cast<APiecePawn>(CPC->SelectedPieceToMove)->GetIsFirstMove())
				{
					Cast<APiecePawn>(CPC->SelectedPieceToMove)->PawnMovedForTheFirstTime();
				}
				Cast<APiecePawn>(CPC->SelectedPieceToMove)->Promote();
			}
		}

		// Setting the actual tile occupied by a white, setting the old one empty
		if (CPC->SelectedPieceToMove->GetVirtualPosition() == CurrTile->GetGridPosition())
		{
			ActualTilePtr->SetOccupantColor(EOccupantColor::W);
			PreviousTilePtr->SetOccupantColor(EOccupantColor::E);

			// Generate the FEN string and add it to the history of moves for replays
			FString LastMove = GameMode->CB->GenerateStringFromPositions();
			GameMode->CB->HistoryOfMoves.Add(LastMove);
			
			// Create dinamically the move button
			if (MainHUD)
			{
				MainHUD->AddButton(LastMove, CPC->SelectedPieceToMove, bIsACapture, CPC->SelectedPieceToMove->GetVirtualPosition(), OldPosition);
			}

			bIsACapture = false;

			// Turn ending
			IsMyTurn = false;
			if (!Cast<APiecePawn>(CPC->SelectedPieceToMove) || Cast<APiecePawn>(CPC->SelectedPieceToMove)->GetVirtualPosition().X != 7)
			{
				CPC->SelectedPieceToMove = nullptr;
				GameMode->TurnPlayer();
			}
			// Else -> A pawn has been promoted, then the turn's passed in GameMode promotion's segment
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
