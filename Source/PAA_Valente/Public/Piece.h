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
	int32 PieceValue;
	TArray<ATile*> Moves;
	TArray<ATile*> EatablePiecesPosition;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ChangeMaterial changes the skin of the piece (Default: White)
	void ChangeMaterial(UMaterialInterface* NewMaterial);

	// Returns piece's relative position with Z = 10.f
	FVector RelativePosition() const;
	FVector2D Relative2DPosition() const;

	void ColorPossibleMoves();
	void DecolorPossibleMoves();

	bool IsSameColorAsTileOccupant(ATile* Tile);

	virtual void PossibleMoves() {};
	void FilterOnlyLegalMoves();

};
