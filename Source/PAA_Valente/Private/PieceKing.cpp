// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceKing.h"
#include "PieceRook.h"
#include "ChessPlayerController.h"
#include "MainHUD.h"
#include "EngineUtils.h"

// Sets default values
APieceKing::APieceKing()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// template function that creates a components
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);

	PieceValue = 10;
	Rooks.SetNum(2);
}

// Called when the game starts or when spawned
void APieceKing::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APieceKing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APieceKing::PossibleMoves()
{
	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode->CB->TileMap.Contains(VirtualPosition))
	{
		// Emptying from old moves (if there are any)
		Moves.Empty();

		ATile* StartTile = GameMode->CB->TileMap[VirtualPosition];
		ATile* NextTile = nullptr;

		// For every direction check if the tile is occupied, if not add a possible move
		for (const FVector2D& Direction : Directions)
		{
			FVector2D NextPosition = VirtualPosition + Direction;
			if (GameMode->CB->TileMap.Contains(NextPosition))
			{
				NextTile = GameMode->CB->TileMap[NextPosition];

				if (StartTile->GetOccupantColor() != NextTile->GetOccupantColor())
				{
					Moves.Add(NextTile);
				}
			}
		}
	}

	AddCastlingToPossibleMoves(Rooks[0]);
	AddCastlingToPossibleMoves(Rooks[1]);
}

void APieceKing::AddCastlingToPossibleMoves(APiece* RookToCastleWith)
{
	if (!bWasMoved && RookToCastleWith && !RookToCastleWith->GetWasMoved())
	{
		AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());

		float CordX = VirtualPosition.X;

		TArray<ATile*> TilesToCheck;

		if (RookToCastleWith->GetVirtualPosition().Y == 0.f)
		{
			TilesToCheck = { GameMode->CB->TileMap[FVector2D(CordX, 1)], GameMode->CB->TileMap[FVector2D(CordX, 2)], GameMode->CB->TileMap[FVector2D(CordX, 3)] };
		}
		else
		{
			TilesToCheck = { GameMode->CB->TileMap[FVector2D(CordX, 5)], GameMode->CB->TileMap[FVector2D(CordX, 6)] };
		}

		bool bTilesAreFree = true;
		bool bWouldBeCheck = false;

		for (ATile* Tile : TilesToCheck)
		{
			if (Tile->GetOccupantColor() != EOccupantColor::E)
			{
				bTilesAreFree = false;
				break;
			}
		}

		if (bTilesAreFree)
		{
			TArray<APiece*> EnemyPieces = (Color == EColor::W) ? GameMode->CB->BlackPieces : GameMode->CB->WhitePieces;
			APiece* EnemyKing = (Color == EColor::W) ? GameMode->CB->KingsArray[1] : GameMode->CB->KingsArray[0];

			if (!EnemyKing->GetWasMoved())
			{
				EnemyPieces.Remove(EnemyKing);
			}

			for (APiece* EnemyPiece : EnemyPieces)
			{
				EnemyPiece->PossibleMoves();
				for (ATile* Tile : TilesToCheck)
				{
					if (EnemyPiece->Moves.Contains(Tile))
					{
						bWouldBeCheck = true;
						break;
					}
				}

				if (bWouldBeCheck)
				{
					break;
				}
			}

			if (!bWouldBeCheck)
			{
				Moves.Add(GameMode->CB->TileMap[RookToCastleWith->GetVirtualPosition()]);
			}
		}
	}
}

void APieceKing::PerformCastling(APiece* RookToCastleWith)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	UMainHUD* MainHUD = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController())->MainHUDWidget;

	int32 CordX = VirtualPosition.X;
	EOccupantColor AllyOccupantColor = (Color == EColor::W) ? EOccupantColor::W : EOccupantColor::B;
	ATile* KingDestinationTile = nullptr;
	ATile* RookDestinationTile = nullptr;
	bool bIsLong = false;

	// Castle left
	if (RookToCastleWith->GetVirtualPosition().Y == 0.f)
	{
		KingDestinationTile = GameMode->CB->TileMap[FVector2D(CordX, 2)];
		RookDestinationTile = GameMode->CB->TileMap[FVector2D(CordX, 3)];
		bIsLong = true;
	}
	else if (RookToCastleWith->GetVirtualPosition().Y == 7.f)
	{
		KingDestinationTile = GameMode->CB->TileMap[FVector2D(CordX, 6)];
		RookDestinationTile = GameMode->CB->TileMap[FVector2D(CordX, 5)];
	}

	// Moving the king
	GameMode->CB->TileMap[GetVirtualPosition()]->SetOccupantColor(EOccupantColor::E);
	KingDestinationTile->SetOccupantColor(AllyOccupantColor);

	FVector KingPositioning = GameMode->CB->GetRelativeLocationByXYPosition(KingDestinationTile->GetGridPosition().X, KingDestinationTile->GetGridPosition().Y);
	KingPositioning.Z = 10.0f;
	this->SetActorLocation(KingPositioning);
	this->SetVirtualPosition(KingDestinationTile->GetGridPosition());
	this->SetWasMoved(true);

	// Moving the rook
	GameMode->CB->TileMap[RookToCastleWith->GetVirtualPosition()]->SetOccupantColor(EOccupantColor::E);
	RookDestinationTile->SetOccupantColor(AllyOccupantColor);

	FVector RookPositioning = GameMode->CB->GetRelativeLocationByXYPosition(RookDestinationTile->GetGridPosition().X, RookDestinationTile->GetGridPosition().Y);
	RookPositioning.Z = 10.0f;
	RookToCastleWith->SetActorLocation(RookPositioning);
	RookToCastleWith->SetVirtualPosition(RookDestinationTile->GetGridPosition());
	RookToCastleWith->SetWasMoved(true);

	FString LastMove = GameMode->CB->GenerateStringFromPositions();
	GameMode->CB->HistoryOfMoves.Add(LastMove);

	if (MainHUD)
	{
		MainHUD->AddRookButton(LastMove, bIsLong);
	}

	GameMode->TurnPlayer();
}

APiece* APieceKing::GetLeftRook() const
{
	return Rooks[0];
}

APiece* APieceKing::GetRightRook() const
{
	return Rooks[1];
}
