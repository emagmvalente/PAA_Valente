// Fill out your copyright notice in the Description page of Project Settings.


#include "PiecePawn.h"
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
	// Movement Logic
	Moves.Empty();

	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector ActorLocation = RelativePosition();
	FVector2D TileLocation(ActorLocation.X, ActorLocation.Y);

	// If the pawn is black, then invert his movements
	if (Color == EColor::B)
	{
		Direction = FVector2D(-1,0);
	}

	FVector2D NextPosition = FVector2D(ActorLocation.X, ActorLocation.Y) + Direction;
	ATile** NextTile = GameMode->CB->TileMap.Find(NextPosition);

	// If it's pawn's first move, then add two tiles in front of him
	if (bFirstMove && NextPosition.X >= 0 && NextPosition.X < 8 && (*NextTile)->GetTileStatus() == ETileStatus::EMPTY)
	{
		Moves.Add((*NextTile));
		NextPosition += Direction;
		NextTile = GameMode->CB->TileMap.Find(NextPosition);
		if (NextPosition.X >= 0 && NextPosition.X < 8 && (*NextTile)->GetTileStatus() == ETileStatus::EMPTY)
		{
			Moves.Add((*NextTile));
		}
	}

	// Else add one tile in front of him
	else if (!bFirstMove && NextPosition.X >= 0 && NextPosition.X < 8 && (*NextTile)->GetTileStatus() == ETileStatus::EMPTY)
	{
		Moves.Add((*NextTile));
	}

	// Eating Logic
	EatablePieces.Empty();
	TArray<FVector2D> EatingDirections;
	if (Color == EColor::W)
	{
		EatingDirections = { FVector2D(1, -1), FVector2D(1, 1) };
	}
	if (Color == EColor::B)
	{
		EatingDirections = { FVector2D(-1, -1), FVector2D(-1, 1) };
	}

	for (const FVector2D EatingDirection : EatingDirections)
	{
		NextPosition = FVector2D(ActorLocation.X, ActorLocation.Y) + EatingDirection;
		NextTile = GameMode->CB->TileMap.Find(NextPosition);
		if (NextTile != nullptr && !IsSameColorAsTileOccupant((*NextTile)) && (*NextTile)->GetTileStatus() != ETileStatus::EMPTY)
		{
			EatablePieces.Add((*NextTile));
		}
	}
}
