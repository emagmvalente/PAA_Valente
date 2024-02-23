// Fill out your copyright notice in the Description page of Project Settings.


#include "PiecePawn.h"
#include "EngineUtils.h"

// Sets default values
APiecePawn::APiecePawn()
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
void APiecePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APiecePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APiecePawn::MoveToLocation(const FVector& TargetLocation)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector CurrentRelativeLocation3D = RelativePosition();
	FVector MoveDirection = TargetLocation - CurrentRelativeLocation3D;

	if (MoveDirection.Y == 0 && MoveDirection.X >= 0)
	{
		// If it's the pawn's first move then he can go through two tiles, otherwise through one
		if ((bFirstMove == true && FMath::Abs(MoveDirection.X) <= 2) ||
			(bFirstMove == false && FMath::Abs(MoveDirection.X) == 1))
		{
			FVector2D TargetTileLocation(TargetLocation.X, TargetLocation.Y);
			if (!IsPathObstructed(FVector2D(CurrentRelativeLocation3D.X, CurrentRelativeLocation3D.Y), TargetTileLocation, MoveDirection))
			{
				FVector TilePositioning = GameMode->CB->GetRelativeLocationByXYPosition(TargetLocation.X, TargetLocation.Y);
				TilePositioning.Z = 10.0f;
				SetActorLocation(TilePositioning);
				bFirstMove = false;
			}
		}

		// Any other move is illegal
		else
		{
			SetActorLocation(GetActorLocation());
		}
	}
}

void APiecePawn::PossibleMoves()
{
	Moves.Empty();
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector ActorLocation = RelativePosition();
	FVector2D TileLocation(ActorLocation.X, ActorLocation.Y);

	if (Color == EColor::B)
	{
		Direction = FVector2D(-1,0);
	}

	FVector2D NextPosition = FVector2D(ActorLocation.X, ActorLocation.Y) + Direction;
	
	ATile** NextTile = GameMode->CB->TileMap.Find(NextPosition);

	if (bFirstMove && NextPosition.X >= 0 && NextPosition.X < 8)
	{
		Moves.Add((*NextTile));
		NextPosition += Direction;

		if (NextPosition.X >= 0 && NextPosition.X < 8) 
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("In first move"));
			NextTile = GameMode->CB->TileMap.Find(NextPosition);
			Moves.Add((*NextTile));
			bFirstMove = false;
		}
	}
	else if (!bFirstMove && NextPosition.X >= 0 && NextPosition.X < 8)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not in first move"));
		Moves.Add((*NextTile));
	}
}
