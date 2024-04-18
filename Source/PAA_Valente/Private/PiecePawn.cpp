// Fill out your copyright notice in the Description page of Project Settings.


#include "PiecePawn.h"
#include "PieceQueen.h"
#include "PieceRook.h"
#include "PieceBishop.h"
#include "PieceKnight.h"
#include "ChessPlayerController.h"
#include "EngineUtils.h"

// Sets default values
APiecePawn::APiecePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// template function that creates a components
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);

	PieceValue = 1;
	TurnsWithoutMoving = 0;
	bIsFirstMove = true;
}

// Called when the game starts or when spawned
void APiecePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void APiecePawn::Promote()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->PawnPromotionWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), GameMode->PawnPromotionWidgetClass);

	// If it's white, show thw widget
	if (Color == EColor::W && VirtualPosition.X == 7)
	{
		if (GameMode->PawnPromotionWidgetInstance)
		{
			GameMode->PawnPromotionWidgetInstance->AddToViewport();
			GameMode->PawnToPromote = this;
		}
	}

	// If it's black, chose randomly
	else if (Color == EColor::B && VirtualPosition.X == 0)
	{
		GameMode->PawnToPromote = this;
		int32 RandIdx0 = FMath::Rand() % 4;
		switch (RandIdx0)
		{
			case 0:
				GameMode->PromoteToQueen();
				break;
			case 1:
				GameMode->PromoteToRook();
				break;
			case 2:
				GameMode->PromoteToBishop();
				break;
			case 3:
				GameMode->PromoteToKnight();
				break;
			default: 
				return;
		}
	}
}

// Called every frame
void APiecePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APiecePawn::PossibleMoves()
{
	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode->CB->TileMap.Contains(VirtualPosition))
	{
		// Emptying from old moves (if there are any)
		Moves.Empty();

		ATile* StartTile = GameMode->CB->TileMap[VirtualPosition];
		ATile* NextTile = nullptr;

		// If the pawn is black, then invert his movements
		if (Color == EColor::B)
		{
			Directions = { FVector2D(-1, 0), FVector2D(-1, -1), FVector2D(-1, 1) };
		}

		for (const FVector2D& Direction : Directions)
		{
			FVector2D NextPosition = VirtualPosition + Direction;
			if (GameMode->CB->TileMap.Contains(NextPosition))
			{
				NextTile = GameMode->CB->TileMap[NextPosition];
				// If it's the upper / lower direction and the tile's empty, then add to moves
				if (Direction == Directions[0] && NextTile->GetOccupantColor() == EOccupantColor::E)
				{
					Moves.Add(NextTile);

					// If is pawn's first move, then add the next tile too to moves
					NextPosition += Direction;
					if (GameMode->CB->TileMap.Contains(NextPosition))
					{
						NextTile = GameMode->CB->TileMap[NextPosition];
						if (NextTile != nullptr && Direction == Directions[0] && bIsFirstMove && NextTile->GetOccupantColor() == EOccupantColor::E)
						{
							Moves.Add(NextTile);
						}
					}
				}
				else if (Direction != Directions[0] && NextTile->GetOccupantColor() != EOccupantColor::E)
				{
					// If it's any other direction and the tile's occupied by an enemy, add to moves
					if (StartTile->GetOccupantColor() != NextTile->GetOccupantColor())
					{
						Moves.Add(NextTile);
					}
				}
			}
		}
	}
}

void APiecePawn::IncrementTurnsWithoutMoving()
{
	TurnsWithoutMoving++;
}

void APiecePawn::ResetTurnsWithoutMoving()
{
	TurnsWithoutMoving = 0;
}

void APiecePawn::PawnMovedForTheFirstTime()
{
	bIsFirstMove = false;
}

int32 APiecePawn::GetTurnsWithoutMoving() const
{
	return TurnsWithoutMoving;
}

bool APiecePawn::GetIsFirstMove() const
{
	return bIsFirstMove;
}
