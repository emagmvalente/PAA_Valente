// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Piece.h"
#include "GameFramework/Actor.h"
#include "PieceKnight.generated.h"

UCLASS()
class PAA_VALENTE_API APieceKnight : public APiece
{
	GENERATED_BODY()
	
public:

	APieceKnight();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

	TArray<FVector2D> Directions = { FVector2D(2, 1), FVector2D(2, -1), FVector2D(-2, 1), FVector2D(-2, -1),
										   FVector2D(1, 2), FVector2D(1, -2), FVector2D(-1, 2), FVector2D(-1, -2) };

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void PossibleMoves(FVector2D StartingPosition) override;
};
