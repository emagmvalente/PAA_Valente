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
	int32 PieceValue;
	EColor Color;
	FVector2D VirtualPosition;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	TArray<ATile*> Moves;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ChangeMaterial changes the skin of the piece (Default: White)
	void ChangeMaterial(UMaterialInterface* NewMaterial);

	// ONLY FOR HUMAN PLAYER: Color and decolor tiles where the piece can move
	void ColorPossibleMoves();
	void DecolorPossibleMoves();

	// Calculate moves and filter them to legal
	virtual void PossibleMoves() {};
	void FilterOnlyLegalMoves();

	// Getters
	int32 GetPieceValue() const;
	EColor GetColor() const;
	FVector2D GetVirtualPosition() const;

	// Setters
	void SetColor(EColor NewColor);
	void SetVirtualPosition(FVector2D PositionToVirtualize);

};
