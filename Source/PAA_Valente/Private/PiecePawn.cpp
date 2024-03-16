// Fill out your copyright notice in the Description page of Project Settings.


#include "PiecePawn.h"
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
}

// Called when the game starts or when spawned
void APiecePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APiecePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APiecePawn::PossibleMoves()
{
	Moves.Empty();
	EatablePieces.Empty();

	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector ActorLocation = RelativePosition();
	FVector2D TileLocation(ActorLocation.X, ActorLocation.Y);
	ATile** NextTile = GameMode->CB->TileMap.Find(TileLocation);

	// If the pawn is black, then invert his movements
	if (Color == EColor::B)
	{
		Directions = { FVector2D(-1, 0), FVector2D(-1, -1), FVector2D(-1, 1) };
	}

	for (const FVector2D& Direction : Directions)
	{
		FVector2D NextPosition = TileLocation + Direction;
		NextTile = GameMode->CB->TileMap.Find(NextPosition);

		if (NextTile != nullptr)
		{
			if (Direction == Directions[0] && (*NextTile)->GetOccupantColor() == EOccupantColor::E)
			{
				Moves.Add((*NextTile));

				NextPosition += Direction;
				NextTile = GameMode->CB->TileMap.Find(NextPosition);

				if (Direction == Directions[0] && bFirstMove && (*NextTile)->GetOccupantColor() == EOccupantColor::E)
				{
					Moves.Add(*NextTile);
				}

				continue;
			}
			else if (!IsSameColorAsTileOccupant(*NextTile) && (*NextTile)->GetOccupantColor() != EOccupantColor::E && Direction != Directions[0])
			{
				EatablePieces.Add(*NextTile);
				continue;
			}
		}
		else
		{
			continue;
		}
	}
}
