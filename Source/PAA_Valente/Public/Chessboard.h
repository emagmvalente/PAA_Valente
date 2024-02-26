// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "GameFramework/Actor.h"
#include "Chessboard.generated.h"

class APiece;

// macro declaration for a dynamic multicast delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReset);

UCLASS()
class PAA_VALENTE_API AChessboard : public AActor
{
	GENERATED_BODY()
	
public:
	// Keeps track of the chessboard's tiles
	UPROPERTY(Transient)
	TArray<ATile*> TileArray;

	TArray<APiece*> WhitePieces;
	TArray<APiece*> BlackPieces;

	TArray<FString> HistoryOfMoves;

	// Given a position returns a tile
	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> TileMap;

	TMap<FVector, APiece*> PieceMap;

	// Spazio prefissato che c'è tra una cella e l'altra, indipendentemente dalla grandezza delle tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float NormalizedCellPadding;

	static const int32 NOT_ASSIGNED = -1;

	// BlueprintAssignable Usable with Multicast Delegates only. Exposes the property for assigning in Blueprints.
	// declare a variable of type FOnReset (delegate)
	UPROPERTY(BlueprintAssignable)
	FOnReset OnResetEvent;

	// size of field
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Size;

	// TSubclassOf template class that provides UClass type safety
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATile> TileClass;

	// tile padding dimension
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CellPadding;

	// tile size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TileSize;

	// Sets default values for this actor's properties
	AChessboard();

	// Called when an instance of this class is placed (in editor) or spawned
	virtual void OnConstruction(const FTransform& Transform) override;

	// remove all signs from the field
	UFUNCTION(BlueprintCallable)
	void ResetField();

	// generate an empty game field
	void GenerateField();

	FString GenerateStringFromPositions();
	void GeneratePositionsFromString(FString& String);

	// return a (x,y) position given a hit (click) on a field tile
	FVector2D GetPosition(const FHitResult& Hit);

	// Con "Relative Position" si intende la posizione nella griglia, con "Position" si intende la posizione nello spazio di Unreal

	// return a relative position given (x,y) position
	FVector GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const;

	// return (x,y) position given a relative position
	FVector2D GetXYPositionByRelativeLocation(const FVector& Location) const;

	TArray<ATile*> GetTileArray() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<TCHAR> Alphabet = { 'B', 'K', 'N', 'P', 'Q', 'R', 'b', 'k', 'n', 'p', 'q', 'r',
							  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '/' };


};
