// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceQueen.h"
#include "EngineUtils.h"

// Sets default values
APieceQueen::APieceQueen()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// template function that creates a components
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);
}

// Called when the game starts or when spawned
void APieceQueen::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APieceQueen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APieceQueen::PossibleMoves()
{
	Moves.Empty();

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
			if (NextPosition.X >= 0 && NextPosition.X < 8 && NextPosition.Y >= 0 && NextPosition.Y < 8)
			{
				if ((*NextTile)->GetTileStatus() == ETileStatus::EMPTY)
				{
					Moves.Add((*NextTile));
				}
				else
				{
					// Se la tile non è vuota, interrompi il loop.
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
