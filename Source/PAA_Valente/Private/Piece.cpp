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

FVector APiece::RelativePosition() const
{
	return FVector(Relative2DPosition().X, Relative2DPosition().Y, 10.f);
}

FVector2D APiece::Relative2DPosition() const
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	return GameMode->CB->GetXYPositionByRelativeLocation(GetActorLocation());
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

	for (ATile* Move : Moves)
	{
		Move->ChangeMaterial(LoadYellowMaterial);
	}
	for (ATile* EatablePiecePosition : EatablePiecesPosition)
	{
		EatablePiecePosition->ChangeMaterial(LoadRedMaterial);
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
	TArray<ATile*> MovesAndEatablePieces = Moves;
	MovesAndEatablePieces.Append(EatablePiecesPosition);
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
		Move->SetOccupantColor(AllyColor);
		bool bIsMoveSafe = true;

		// Checking if any enemy piece can threaten the move
		for (APiece* EnemyPiece : EnemyPieces)
		{
			// Checking if the piece calculated isn't the threatening piece, if yes then skip its moves to simulate a capture
			if (Move->GetGridPosition() != EnemyPiece->Relative2DPosition())
			{
				EnemyPiece->PossibleMoves();
				// Checking if the piece is a king, if yes then any move is equal to moving the king tile, 
				// so don't consider "the king tile" but consinder "the move"
				if (Cast<APieceKing>(this))
				{
					if (EnemyPiece->EatablePiecesPosition.Contains(Move))
					{
						bIsMoveSafe = false;
						break;
					}
				}
				// Else consider "king tile"
				else
				{
					if (EnemyPiece->EatablePiecesPosition.Contains(*KingTile))
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
			else if (EatablePiecesPosition.Contains(Move))
			{
				EatablePiecesPosition.Remove(Move);
			}
		}

		// Restoring the original occupant color of the tile
		Move->SetOccupantColor(ActualOccupantColor);
	}

	// Restoring the original occupant color of the start tile
	(*StartTile)->SetOccupantColor(AllyColor);
}

int32 APiece::GetPieceValue() const
{
	return PieceValue;
}

EColor APiece::GetColor() const
{
	return Color;
}

void APiece::SetColor(EColor NewColor)
{
	Color = NewColor;
}
