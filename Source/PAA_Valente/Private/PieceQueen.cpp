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

void APieceQueen::MoveToLocation(const FVector& TargetLocation)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector CurrentRelativeLocation3D = RelativePosition();
	FVector MoveDirection = TargetLocation - CurrentRelativeLocation3D;

	// If the movement is diagonal / orizontal / vertical is legal
	if ((FMath::Abs(MoveDirection.X) == FMath::Abs(MoveDirection.Y)) ||
		(FMath::Abs(MoveDirection.X) == 0) || (FMath::Abs(MoveDirection.Y) == 0))
	{
		FVector2D TargetTileLocation(TargetLocation.X, TargetLocation.Y);
		if (!IsPathObstructed(FVector2D(CurrentRelativeLocation3D.X, CurrentRelativeLocation3D.Y), TargetTileLocation, MoveDirection))
		{
			FVector TilePositioning = GameMode->CB->GetRelativeLocationByXYPosition(TargetLocation.X, TargetLocation.Y);
			TilePositioning.Z = 10.0f;
			SetActorLocation(TilePositioning);
		}
	}

	// Any other movement is illegal
	else
	{
		SetActorLocation(GetActorLocation());
	}
}

void APieceQueen::PossibleMoves()
{
	Moves.Empty();
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector ActorLocation = RelativePosition();
	FVector2D TileLocation(ActorLocation.X, ActorLocation.Y);
	ATile** TilePtr = GameMode->CB->TileMap.Find(TileLocation);

	for (const FVector2D& Direction : Directions)
	{
		FVector2D NextPosition = TileLocation + Direction;
		TilePtr = GameMode->CB->TileMap.Find(NextPosition);
		bool bIsObstructed = false;

		while (true)
		{
			if (NextPosition.X >= 0 && NextPosition.X < 8 && NextPosition.Y >= 0 && NextPosition.Y < 8 &&
				!((*TilePtr)->GetTileStatus() == ETileStatus::OCCUPIED))
			{
				TilePtr = GameMode->CB->TileMap.Find(NextPosition);
				Moves.Add((*TilePtr));
				NextPosition += Direction;
			}
			else
			{
				break;
			}
		}
	}
}
