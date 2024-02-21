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

void APieceKnight::MoveToLocation(const FVector& TargetLocation)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector CurrentRelativeLocation3D = RelativePosition();
	FVector MoveDirection = TargetLocation - CurrentRelativeLocation3D;

	// If the movement is orizontal / vertical is legal
	if ((FMath::Abs(MoveDirection.X) == 2 && FMath::Abs(MoveDirection.Y) == 1) ||
		(FMath::Abs(MoveDirection.Y) == 2 && FMath::Abs(MoveDirection.X) == 1))
	{
		FVector2D TargetTileLocation(TargetLocation.X, TargetLocation.Y);
		FVector TilePositioning = GameMode->CB->GetRelativeLocationByXYPosition(TargetLocation.X, TargetLocation.Y);
		TilePositioning.Z = 10.0f;
		SetActorLocation(TilePositioning);
	}

	// Any other movement is illegal
	else
	{
		SetActorLocation(GetActorLocation());
	}
}

void APieceKnight::ColorPossibleMoves() const
{
}
