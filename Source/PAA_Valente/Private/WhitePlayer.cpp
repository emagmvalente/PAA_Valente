// Fill out your copyright notice in the Description page of Project Settings.


#include "WhitePlayer.h"
#include "Piece.h"
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
	AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());
	//Structure containing information about one hit of a trace, such as point of impact and surface normal at that point
	FHitResult Hit = FHitResult(ForceInit);
	// GetHitResultUnderCursor function sends a ray from the mouse position and gives the corresponding hit results
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

	if (Hit.bBlockingHit && IsMyTurn)
	{
		if (APiece* CurrPiece = Cast<APiece>(Hit.GetActor()))
		{
			if (CurrPiece->Color == EColor::W)
			{
				CPC->SelectedPieceToMove = CurrPiece;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Piece Picked"));
			}
		}
	}
}

void AWhitePlayer::TileSelection()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());
	//Structure containing information about one hit of a trace, such as point of impact and surface normal at that point
	FHitResult Hit = FHitResult(ForceInit);
	// GetHitResultUnderCursor function sends a ray from the mouse position and gives the corresponding hit results
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

	if (Hit.bBlockingHit && IsMyTurn)
	{
		if (ATile* CurrTile = Cast<ATile>(Hit.GetActor()))
		{
			if (CurrTile->GetTileStatus() == ETileStatus::EMPTY && CPC->SelectedPieceToMove != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Tile Clicked"));

				FVector PreviousLocation = CPC->SelectedPieceToMove->RelativePosition();
				ATile** PreviousTilePtr = GameMode->CB->TileMap.Find(FVector2D(PreviousLocation.X, PreviousLocation.Y));

				FVector2D TilePosition = CurrTile->GetGridPosition();
				ATile** ActualTilePtr = GameMode->CB->TileMap.Find(TilePosition);

				FVector NewLocation(TilePosition.X, TilePosition.Y, 10.f);
				CPC->SelectedPieceToMove->MoveToLocation(NewLocation);

				if (CPC->SelectedPieceToMove->RelativePosition() == NewLocation)
				{
					(*ActualTilePtr)->SetTileStatus(ETileStatus::OCCUPIED);
					(*ActualTilePtr)->SetOccupantColor(EOccupantColor::W);

					(*PreviousTilePtr)->SetTileStatus(ETileStatus::EMPTY);
					(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);

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

