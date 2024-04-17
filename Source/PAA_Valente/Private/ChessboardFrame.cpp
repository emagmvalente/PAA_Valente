// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessboardFrame.h"

// Sets default values
AChessboardFrame::AChessboardFrame()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// template function that creates a components
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);

}

// Called when the game starts or when spawned
void AChessboardFrame::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChessboardFrame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

