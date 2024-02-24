// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceKnight.h"
#include "EngineUtils.h"

// Sets default values
APieceKnight::APieceKnight()
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
void APieceKnight::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APieceKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APieceKnight::PossibleMoves()
{
	Moves.Empty();
	EatablePieces.Empty();

	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector ActorLocation = RelativePosition();
	FVector2D TileLocation(ActorLocation.X, ActorLocation.Y);
	ATile** NextTile = GameMode->CB->TileMap.Find(TileLocation);

	// For every direction check if the tile is occupied, if not add a possible move
	for (const FVector2D& Direction : Directions)
	{
		FVector2D NextPosition = TileLocation + Direction;
		NextTile = GameMode->CB->TileMap.Find(NextPosition);
		if (NextTile == nullptr || IsSameColorAsTileOccupant((*NextTile)))
		{
			continue;
		}
		else if (!IsSameColorAsTileOccupant((*NextTile)) && (*NextTile)->GetTileStatus() != ETileStatus::EMPTY)
		{
			EatablePieces.Add((*NextTile));
			continue;
		}
		else
		{
			Moves.Add((*NextTile));
		}
	}
}
