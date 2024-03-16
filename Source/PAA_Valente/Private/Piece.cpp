// Fill out your copyright notice in the Description page of Project Settings.


#include "Piece.h"
#include "PieceKing.h"
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

FVector2D APiece::Relative2DPosition() const
{
	return FVector2D(RelativePosition().X, RelativePosition().Y);
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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, *MyDoubleString);

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

void APiece::FilterOnlyLegalMoves()
{
	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	TArray<ATile*> MovesAndEatablePieces = Moves;
	MovesAndEatablePieces.Append(EatablePieces);
	ATile** StartTile = GameMode->CB->TileMap.Find(Relative2DPosition());
	
	ATile** KingTile = nullptr;
	EOccupantColor AllyColor = EOccupantColor::E;
	EOccupantColor EnemyColor = EOccupantColor::E;
	TArray<APiece*> EnemyPieces;

	// Assignments
	if (Color == EColor::W)
	{
		KingTile = GameMode->CB->TileMap.Find(GameMode->CB->Kings[0]->Relative2DPosition());
		AllyColor = EOccupantColor::W;
		EnemyColor = EOccupantColor::B;
		EnemyPieces = GameMode->CB->BlackPieces;
	}
	else if (Color == EColor::B)
	{
		KingTile = GameMode->CB->TileMap.Find(GameMode->CB->Kings[1]->Relative2DPosition());
		AllyColor = EOccupantColor::B;
		EnemyColor = EOccupantColor::W;
		EnemyPieces = GameMode->CB->WhitePieces;
	}

	// Simulating the movement of the piece from the start tile
	(*StartTile)->SetOccupantColor(EOccupantColor::E);

	// Iterating through all possible moves and eatable pieces
	for (ATile* Move : MovesAndEatablePieces)
	{
		// Storing the real occupant color to restore it later
		EOccupantColor ActualOccupantColor = Move->GetOccupantColor();
		Move->SetOccupantColor(AllyColor); // Setting the occupant color of the tile to ally color temporarily
		bool bIsMoveSafe = true;

		// Checking if any enemy piece can threaten the move
		for (APiece* EnemyPiece : EnemyPieces)
		{
			// Checking if the piece calculated isn't the threatening piece, if yes then overwrite it to simulate a capture
			if (!(ActualOccupantColor == EnemyColor &&
				Move->GetGridPosition() == EnemyPiece->Relative2DPosition()))
			{
				EnemyPiece->PossibleMoves();
				// Checking if the piece is a king, if yes then any move is equal to moving the king tile, 
				// so don't consider "the king tile" but consinder "the move"
				if (Cast<APieceKing>(this))
				{
					if (EnemyPiece->EatablePieces.Contains(Move))
					{
						bIsMoveSafe = false;
						break;
					}
				}
				// Else consider "king tile"
				else
				{
					if (EnemyPiece->EatablePieces.Contains(*KingTile))
					{
						bIsMoveSafe = false;
						break;
					}
				}
			}
		}

		// Removing unsafe moves from the list
		if (!bIsMoveSafe)
		{
			if (Moves.Contains(Move))
			{
				Moves.Remove(Move);
			}
			else if (EatablePieces.Contains(Move))
			{
				EatablePieces.Remove(Move);
			}
		}

		// Restoring the original occupant color of the tile
		Move->SetOccupantColor(ActualOccupantColor);
	}

	// Restoring the original occupant color of the start tile
	(*StartTile)->SetOccupantColor(AllyColor);
}
