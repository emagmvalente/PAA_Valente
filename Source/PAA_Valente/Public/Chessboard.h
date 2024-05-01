// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "GameFramework/Actor.h"
#include "Chessboard.generated.h"

class APiece;

UCLASS()
class PAA_VALENTE_API AChessboard : public AActor
{
	GENERATED_BODY()
	
public:
	// Keeps track of the chessboard's tiles
	UPROPERTY(Transient)
	TArray<ATile*> TileArray;

	// Array of white pieces and black pieces currently on the board
	TArray<APiece*> WhitePieces;
	TArray<APiece*> BlackPieces;
	TArray<APiece*> Kings;

	// History of old moves for replays
	TArray<FString> HistoryOfMoves;

	// Given a position returns a tile
	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> TileMap;

	// size of field
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Size;

	// TSubclassOf template class that provides UClass type safety
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATile> TileClass;

	// tile size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TileSize;

	// Sets default values for this actor's properties
	AChessboard();

	// Called when an instance of this class is placed (in editor) or spawned
	virtual void OnConstruction(const FTransform& Transform) override;

	// Remove everything from the field
	UFUNCTION(BlueprintCallable)
	void ResetField();

	// Generate a game field with pieces
	void GenerateField();

	// FEN methods
	FString GenerateStringFromPositions();
	void GeneratePositionsFromString(FString& String);

	void SetTilesOwners();

	// Return a relative position given (x,y) position
	FVector GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const;

	// Return (x,y) position given a relative position
	FVector2D GetXYPositionByRelativeLocation(const FVector& Location) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
