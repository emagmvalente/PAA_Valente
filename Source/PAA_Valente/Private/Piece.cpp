// Fill out your copyright notice in the Description page of Project Settings.


#include "Piece.h"
#include "EngineUtils.h"

// Sets default values
APiece::APiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Color = EColor::E;
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
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	// Loading the yellow material and changing the color for every move in moves
	// Loading the red material for eating

	PossibleMoves();
	FilterOnlyLegalMoves();

	UMaterialInterface* LoadYellowMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_Yellow"));
	UMaterialInterface* LoadRedMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_Red"));

	UMaterialInterface* LoadE = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_E"));
	UMaterialInterface* LoadB = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_B"));
	UMaterialInterface* LoadW = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_W"));

	FString MyDoubleString = FString::Printf(TEXT("Moves: %d, EatablePieces: %d"), Moves.Num(), EatablePieces.Num());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, MyDoubleString);

	for (int i = 0; i < Moves.Num(); i++)
	{
		Moves[i]->ChangeMaterial(LoadYellowMaterial);
	}
	for (int i = 0; i < EatablePieces.Num(); i++)
	{
		EatablePieces[i]->ChangeMaterial(LoadRedMaterial);
	}

	for (ATile* Tile : GameMode->CB->TileArray)
	{
		if (Tile->GetOccupantColor() == EOccupantColor::E)
		{
			Tile->ChangeMaterial(LoadE);
		}
		else if (Tile->GetOccupantColor() == EOccupantColor::W)
		{
			Tile->ChangeMaterial(LoadW);
		}
		else
		{
			Tile->ChangeMaterial(LoadB);
		}
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

void APiece::FilterOnlyLegalMoves()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	TArray<ATile*> MovesAndEatablePieces = Moves;
	MovesAndEatablePieces.Append(EatablePieces);

	ATile** StartTile = GameMode->CB->TileMap.Find(FVector2D(RelativePosition().X, RelativePosition().Y));

	ATile** WhiteKingTile = GameMode->CB->TileMap.Find(FVector2D(GameMode->CB->Kings[0]->RelativePosition().X, GameMode->CB->Kings[0]->RelativePosition().Y));
	ATile** BlackKingTile = GameMode->CB->TileMap.Find(FVector2D(GameMode->CB->Kings[1]->RelativePosition().X, GameMode->CB->Kings[1]->RelativePosition().Y));

	if (Color == EColor::W)
	{
		(*StartTile)->SetOccupantColor(EOccupantColor::E);
		for (ATile* Move : MovesAndEatablePieces)
		{
			EOccupantColor ActualOccupantColor = Move->GetOccupantColor();
			Move->SetOccupantColor(EOccupantColor::W);
			for (APiece* BlackPiece : GameMode->CB->BlackPieces)
			{
				BlackPiece->PossibleMoves();
				if (BlackPiece->EatablePieces.Contains(*WhiteKingTile))
				{
					if (Moves.Contains(Move))
					{
						Moves.Remove(Move);
						break;
					}
					else if (EatablePieces.Contains(Move))
					{
						EatablePieces.Remove(Move);
						break;
					}
				}
				if (ActualOccupantColor == EOccupantColor::E)
				{
					Move->SetOccupantColor(EOccupantColor::E);
				}
				else
				{
					Move->SetOccupantColor(EOccupantColor::B);
				}
			}
		}
		(*StartTile)->SetOccupantColor(EOccupantColor::W);
	}

	else
	{
		(*StartTile)->SetOccupantColor(EOccupantColor::E);
		for (ATile* Move : MovesAndEatablePieces)
		{
			EOccupantColor ActualOccupantColor = Move->GetOccupantColor();
			Move->SetOccupantColor(EOccupantColor::B);
			for (APiece* WhitePiece : GameMode->CB->WhitePieces)
			{
				WhitePiece->PossibleMoves();
				if (WhitePiece->EatablePieces.Contains(*BlackKingTile))
				{
					if (Moves.Contains(Move))
					{
						Moves.Remove(Move);
						break;
					}
					else if (EatablePieces.Contains(Move))
					{
						EatablePieces.Remove(Move);
						break;
					}
				}
				if (ActualOccupantColor == EOccupantColor::E)
				{
					Move->SetOccupantColor(EOccupantColor::E);
				}
				else
				{
					Move->SetOccupantColor(EOccupantColor::W);
				}
			}
		}
		(*StartTile)->SetOccupantColor(EOccupantColor::B);
	}
}
