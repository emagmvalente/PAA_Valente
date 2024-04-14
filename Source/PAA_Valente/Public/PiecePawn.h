// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Piece.h"
#include "GameFramework/Actor.h"
#include "PiecePawn.generated.h"

UCLASS()
class PAA_VALENTE_API APiecePawn : public APiece
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APiecePawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FVector2D> Directions = { FVector2D(1, 0), FVector2D(1, -1), FVector2D(1, 1) };
	int32 TurnsWithoutMoving;
	bool bIsFirstMove;

public:
	void Promote();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PossibleMoves(FVector2D StartingPosition) override;

	void IncrementTurnsWithoutMoving();
	void ResetTurnsWithoutMoving();
	void PawnMovedForTheFirstTime();

	int32 GetTurnsWithoutMoving() const;
	bool GetIsFirstMove() const;
};
