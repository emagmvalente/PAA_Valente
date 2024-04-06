// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UENUM()
enum class EOccupantColor : uint8
{
	W,
	B,
	E
};

UCLASS()
class PAA_VALENTE_API ATile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATile();

	EOccupantColor GetOccupantColor() const;

	// set the player owner and the status of a tile
	void SetOccupantColor(const EOccupantColor Color);

	// set the (x, y) position
	void SetGridPosition(const double InX, const double InY);

	// get the (x, y) position
	FVector2D GetGridPosition() const;

	// To change the color of the tile
	UFUNCTION(BlueprintCallable)
	void ChangeMaterial(class UMaterialInterface* NewMaterial);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	EOccupantColor OccupantColor;

	// (x, y) position of the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D TileGridPosition;
};
