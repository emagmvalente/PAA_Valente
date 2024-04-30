// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Piece.h"
#include "GameFramework/Actor.h"
#include "PieceKing.generated.h"

UCLASS()
class PAA_VALENTE_API APieceKing : public APiece
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APieceKing();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	TArray<APiece*> Rooks;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FVector2D> Directions = { FVector2D(1, 0), FVector2D(-1, 0), FVector2D(0, 1), FVector2D(0, -1),
										   FVector2D(1, 1), FVector2D(1, -1), FVector2D(-1, 1), FVector2D(-1, -1) };

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;	
	
	virtual void PossibleMoves() override;

	void AddCastlingToPossibleMoves(APiece* RookToCastleWith);
	void PerformCastling(APiece* RookToCastleWith);

	APiece* GetLeftRook() const;
	APiece* GetRightRook() const;

};
