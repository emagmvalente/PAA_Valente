// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Piece.h"
#include "GameFramework/Actor.h"
#include "PieceBishop.generated.h"

UCLASS()
class PAA_VALENTE_API APieceBishop : public APiece
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APieceBishop();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void MoveToLocation(const FVector& TargetLocation) override;
	virtual void ColorPossibleMoves() const override;

};
