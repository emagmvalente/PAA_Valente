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
				CPC->SelectedPieceToMove = CurrPiece;
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

	if (Hit.bBlockingHit && IsMyTurn)
	{
		if (ATile* CurrTile = Cast<ATile>(Hit.GetActor()))
		{
			if (CurrTile->GetTileStatus() == ETileStatus::EMPTY && CPC->SelectedPieceToMove != nullptr)
			{
				// Declarations
				FVector PreviousLocation = CPC->SelectedPieceToMove->RelativePosition();
				ATile** PreviousTilePtr = GameMode->CB->TileMap.Find(FVector2D(PreviousLocation.X, PreviousLocation.Y));
				FVector2D TilePosition = CurrTile->GetGridPosition();
				ATile** ActualTilePtr = GameMode->CB->TileMap.Find(TilePosition);

				// Calculating PossibleMoves and populating Moves array
				CPC->SelectedPieceToMove->PossibleMoves();

				// If the selected tile is in Moves, then allow the move
				FVector2D CurrTilePosition = CurrTile->GetGridPosition();
				
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

					// Turn ending
					IsMyTurn = false;
					GameMode->TurnPlayer(this);
				}

				CPC->SelectedPieceToMove = nullptr;
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

