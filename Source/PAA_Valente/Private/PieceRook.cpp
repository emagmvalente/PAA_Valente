// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceRook.h"
#include "EngineUtils.h"

// Sets default values
APieceRook::APieceRook()
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
void APieceRook::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APieceRook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APieceRook::PossibleMoves()
{
	Moves.Empty();
	EatablePieces.Empty();

	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector ActorLocation = RelativePosition();
	FVector2D TileLocation(ActorLocation.X, ActorLocation.Y);
	ATile** NextTile = GameMode->CB->TileMap.Find(TileLocation);

	// For every direction check if the tile is occupied, if not add a possible move.
	// By the way, if a piece interrupts a path, then stop adding moves in that direction.
	for (const FVector2D& Direction : Directions)
	{
		FVector2D NextPosition = TileLocation + Direction;
		NextTile = GameMode->CB->TileMap.Find(NextPosition);

		while (true)
		{
			if (NextTile != nullptr)
			{
				if ((*NextTile)->GetOccupantColor() == EOccupantColor::E)
				{
					Moves.Add(*NextTile);
				}
				else if (!IsSameColorAsTileOccupant(*NextTile) && (*NextTile)->GetOccupantColor() != EOccupantColor::E)
				{
					EatablePieces.Add(*NextTile);
					break;
				}
				else
				{
					break;
				}
				NextPosition += Direction;
				NextTile = GameMode->CB->TileMap.Find(NextPosition);
			}
			else
			{
				break;
			}
		}
	}
}
