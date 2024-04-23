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
	PieceValue = 0;
	VirtualPosition.X = -1;
	VirtualPosition.Y = -1;
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
	UMaterialInterface* LoadW = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_W"));
	UMaterialInterface* LoadB = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_B"));

	
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
		else if (Tile->GetOccupantColor() == EOccupantColor::B)
		{
			Tile->ChangeMaterial(LoadB);
		}
	}

	for (ATile* Move : Moves)
	{
		if (Move->GetOccupantColor() == EOccupantColor::B)
		{
			Move->ChangeMaterial(LoadRedMaterial);
			continue;
		}
		Move->ChangeMaterial(LoadYellowMaterial);
	}
}

void APiece::DecolorPossibleMoves()
{
	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	UMaterialInterface* LoadWhiteMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_White"));
	UMaterialInterface* LoadBlackMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_Black"));

	// Decolor every tile, it's a little bit rudimental but it's the best way to decolor
	for (ATile* Tile : GameMode->CB->GetTileArray())
	{
		FVector2D TilePosition = Tile->GetGridPosition();
		if ((static_cast<int>(TilePosition.X) + static_cast<int>(TilePosition.Y)) % 2 == 0)
		{
			Tile->ChangeMaterial(LoadBlackMaterial);
		}
		else
		{
			Tile->ChangeMaterial(LoadWhiteMaterial);
		}
	}
}

void APiece::FilterOnlyLegalMoves()
{
	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode->CB->TileMap.Contains(VirtualPosition) && !Moves.IsEmpty())
	{
		ATile* StartTile = GameMode->CB->TileMap[VirtualPosition];

		ATile* KingTile = nullptr;
		TArray<APiece*> EnemyPieces;

		// Assignments
		if (Color == EColor::W)
		{
			KingTile = GameMode->CB->TileMap[GameMode->CB->Kings[0]->VirtualPosition];
			EnemyPieces = GameMode->CB->BlackPieces;
		}
		else if (Color == EColor::B)
		{
			KingTile = GameMode->CB->TileMap[GameMode->CB->Kings[1]->VirtualPosition];
			EnemyPieces = GameMode->CB->WhitePieces;
		}

		// Simulating the movement of the piece from the start tile
		EOccupantColor AllyColor = StartTile->GetOccupantColor();
		StartTile->SetOccupantColor(EOccupantColor::E);

		TArray OriginalMoves = Moves;

		// Iterating through all possible moves
		for (ATile* Move : OriginalMoves)
		{
			// Storing the real occupant color to restore it later
			EOccupantColor ActualOccupantColor = Move->GetOccupantColor();
			Move->SetOccupantColor(AllyColor);
			bool bIsMoveSafe = true;

			// Checking if any enemy piece can threaten the move
			for (APiece* EnemyPiece : EnemyPieces)
			{
				// Checking if the piece calculated isn't the threatening piece, if yes then skip its moves to simulate a capture
				if (Move->GetGridPosition() != EnemyPiece->VirtualPosition)
				{
					EnemyPiece->PossibleMoves();

					// Checking if the piece is a king, if yes then any move is equal to moving the king tile, 
					// so don't consider "the king tile" but consinder "the move"
					if (Cast<APieceKing>(this))
					{
						if (EnemyPiece->Moves.Contains(Move))
						{
							bIsMoveSafe = false;
							break;
						}
					}
					// Else consider "king tile"
					else
					{
						if (EnemyPiece->Moves.Contains(KingTile))
						{
							bIsMoveSafe = false;
							break;
						}
					}
				}
			}

			// Removing unsafe moves from the list
			if (!bIsMoveSafe && Moves.Contains(Move))
			{
				Moves.Remove(Move);
			}

			// Restoring the original occupant color of the tile
			Move->SetOccupantColor(ActualOccupantColor);
		}

		// Restoring the original occupant color of the start tile
		StartTile->SetOccupantColor(AllyColor);
		OriginalMoves.Empty();
	}
}

int32 APiece::GetPieceValue() const
{
	return PieceValue;
}

EColor APiece::GetColor() const
{
	return Color;
}

FVector2D APiece::GetVirtualPosition() const
{
	return VirtualPosition;
}

void APiece::SetColor(EColor NewColor)
{
	Color = NewColor;
}

void APiece::SetVirtualPosition(FVector2D PositionToVirtualize)
{
	VirtualPosition = PositionToVirtualize;
}
