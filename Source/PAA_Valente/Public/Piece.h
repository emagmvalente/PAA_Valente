// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessGameMode.h"
#include "GameFramework/Actor.h"
#include "Piece.generated.h"

UENUM()
enum class EColor : uint8
{
	W,
	B,
	E
};

UCLASS()
class PAA_VALENTE_API APiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	EColor Color;
	const TArray<FVector2D> Directions;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ChangeMaterial changes the skin of the piece (Default: White)
	void ChangeMaterial(UMaterialInterface* NewMaterial);

	// IsPathObstructed checks if the path to move the piece from tile to tile is obstructed by any other piece.
	bool IsPathObstructed(FVector2D CurrentTileLocation, FVector2D TargetTileLocation, FVector MoveDirection);

	// MoveToLocation moves the piece from tile to tile, it depends from the type of piece
	virtual void MoveToLocation(const FVector& TargetLocation) {};

	// Destroys the piece
	void PieceCaptured();

	// Returns piece's relative position with Z = 10.f
	FVector RelativePosition() const;

	void PossibleMoves(FVector& ActorLocation, TArray<ATile*>& ResultantArray);

};
