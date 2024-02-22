// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceBishop.h"
#include "EngineUtils.h"

// Sets default values
APieceBishop::APieceBishop()
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
void APieceBishop::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APieceBishop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APieceBishop::MoveToLocation(const FVector& TargetLocation)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector CurrentRelativeLocation3D = RelativePosition();
	FVector MoveDirection = TargetLocation - CurrentRelativeLocation3D;

	// Checking if the movement is diagonal
	if (FMath::Abs(MoveDirection.X) == FMath::Abs(MoveDirection.Y))
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