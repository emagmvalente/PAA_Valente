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
	Moves.Empty();
	EatablePieces.Empty();

	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector ActorLocation = RelativePosition();
	FVector2D TileLocation(ActorLocation.X, ActorLocation.Y);

	// If the pawn is black, then invert his movements
	if (Color == EColor::B)
	{
		Direction = FVector2D(-1,0);
		EatingDirections = { FVector2D(-1, -1), FVector2D(-1, 1) };
	}

	// More Declarations
	FVector2D NextPosition = FVector2D(ActorLocation.X, ActorLocation.Y) + Direction;
	ATile** NextTile = GameMode->CB->TileMap.Find(NextPosition);

	// Movement logic
	if ((*NextTile) != nullptr && (*NextTile)->GetTileStatus() == ETileStatus::EMPTY)
	{
		// If is not pawn's first move, add only one tile in front of him
		Moves.Add(*NextTile);

		// Else add another tile in front of him
		if (bFirstMove && (*NextTile) != nullptr && (*NextTile)->GetTileStatus() == ETileStatus::EMPTY)
		{
			NextPosition += Direction;
			NextTile = GameMode->CB->TileMap.Find(NextPosition);
			Moves.Add(*NextTile);
		}
	}

	// Eating logic
	for (const FVector2D EatingDirection : EatingDirections)
	{
		NextPosition = FVector2D(ActorLocation.X, ActorLocation.Y) + EatingDirection;
		NextTile = GameMode->CB->TileMap.Find(NextPosition);
		if (NextTile != nullptr && !IsSameColorAsTileOccupant(*NextTile) && (*NextTile)->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			EatablePieces.Add(*NextTile);
		}
	}
}
