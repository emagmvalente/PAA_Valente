// Fill out your copyright notice in the Description page of Project Settings.


#include "Piece.h"
#include "EngineUtils.h"

// Sets default values
APiece::APiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Color = EColor::E;
	bIsOnCheck = false;
}

// Called when the game starts or when spawned
void APiece::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APiece::ChangeMaterial(UMaterialInterface* NewMaterial)
{
	UStaticMeshComponent* MeshComponent = FindComponentByClass<UStaticMeshComponent>();
	if (MeshComponent)
	{
		MeshComponent->SetMaterial(0, NewMaterial);
	}
}

void APiece::PieceCaptured()
{
	Destroy();
}

FVector APiece::RelativePosition() const
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	// Getting the absolute location and getting the relative one passing through the position of the tile.
	FVector2D CurrentRelativeLocation2D = GameMode->CB->GetXYPositionByRelativeLocation(GetActorLocation());
	FVector CurrentRelativeLocation3D(CurrentRelativeLocation2D.X, CurrentRelativeLocation2D.Y, 10.f);
	return CurrentRelativeLocation3D;
}

void APiece::ColorPossibleMoves()
{
	// Loading the yellow material and changing the color for every move in moves
	// Loading the red material for eating

	PossibleMoves();

	UMaterialInterface* LoadYellowMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_Yellow"));
	UMaterialInterface* LoadRedMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_Red"));
	for (int i = 0; i < Moves.Num(); i++)
	{
		Moves[i]->ChangeMaterial(LoadYellowMaterial);
	}
	for (int i = 0; i < EatablePieces.Num(); i++)
	{
		EatablePieces[i]->ChangeMaterial(LoadRedMaterial);
	}
}

void APiece::DecolorPossibleMoves()
{
	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	TArray<ATile*> TileArray = GameMode->CB->GetTileArray();
	UMaterialInterface* LoadWhiteMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_White"));
	UMaterialInterface* LoadBlackMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_Black"));

	// Decolor every tile, it's a little bit rudimental but it's the best way to decolor
	for (int i = 0; i < TileArray.Num(); i++)
	{
		FVector2D TilePosition = TileArray[i]->GetGridPosition();
		if ((static_cast<int>(TilePosition.X) + static_cast<int>(TilePosition.Y)) % 2 == 0)
		{
			TileArray[i]->ChangeMaterial(LoadBlackMaterial);
		}
		else
		{
			TileArray[i]->ChangeMaterial(LoadWhiteMaterial);
		}
	}
}

bool APiece::IsSameColorAsTileOccupant(ATile* Tile)
{
	if (Tile->GetOccupantColor() == EOccupantColor::B && Color == EColor::B)
	{
		return true;
	}
	if (Tile->GetOccupantColor() == EOccupantColor::W && Color == EColor::W)
	{
		return true;
	}
	return false;
}
