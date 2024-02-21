// Fill out your copyright notice in the Description page of Project Settings.


#include "Chessboard.h"
#include "Kismet/GameplayStatics.h"
#include "Piece.h"
#include "PieceBishop.h"
#include "PieceKing.h"
#include "PieceKnight.h"
#include "PiecePawn.h"
#include "PieceQueen.h"
#include "PieceRook.h"
#include "PlayerInterface.h"
#include "EngineUtils.h"

// Sets default values
AChessboard::AChessboard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Size = 8;
	TileSize = 120;
	CellPadding = 0;
}

void AChessboard::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//normalized tilepadding
	NormalizedCellPadding = FMath::RoundToDouble(((TileSize + CellPadding) / TileSize) * 100) / 100;
}

// Called when the game starts or when spawned
void AChessboard::BeginPlay()
{
	Super::BeginPlay();
	GenerateField();
	GeneratePieces();
}

void AChessboard::ResetField()
{
	/*
		for (ATile* Obj : TileArray)
		{
			Obj->SetTileStatus(NOT_ASSIGNED, ETileStatus::EMPTY);
		}

		// send broadcast event to registered objects
		OnResetEvent.Broadcast();

		AGameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
		GameMode->IsGameOver = false;
		GameMode->MoveCounter = 0;
		GameMode->ChoosePlayerAndStartGame();
	*/
}

void AChessboard::GenerateField()
{
	UMaterialInterface* LoadWhiteMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_White"));
	UMaterialInterface* LoadBlackMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_Black"));

	for (int32 x = 0; x < Size; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			FVector Location = AChessboard::GetRelativeLocationByXYPosition(x, y);
			// La spawnactor fallisce se l'oggetto collide con un altro
			ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass, Location, FRotator::ZeroRotator);

			// Setting the color of the tile (b/w)
			if ((x + y) % 2 == 0)
			{
				Obj->ChangeMaterial(LoadBlackMaterial);
			}
			else
			{
				Obj->ChangeMaterial(LoadWhiteMaterial);
			}

			// Setting the occupant color of the tile (b/w)
			if (x <= 1)
			{
				Obj->SetOccupantColor(EOccupantColor::W);
			}
			if (x >= 6)
			{
				Obj->SetOccupantColor(EOccupantColor::B);
			}

			const float TileScale = TileSize / 100;
			Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
			Obj->SetGridPosition(x, y);
			TileArray.Add(Obj);
			TileMap.Add(FVector2D(x, y), Obj);
		}
	}
}

void AChessboard::GeneratePieces()
{

	APiece* Obj = nullptr;

	// Loading materials to create blacks
	UMaterialInterface* LoadBlackBishop = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BBishop"));
	UMaterialInterface* LoadBlackPawn = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BPawn"));
	UMaterialInterface* LoadBlackKing = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BKing"));
	UMaterialInterface* LoadBlackQueen = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BQueen"));
	UMaterialInterface* LoadBlackKnight = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BKnight"));
	UMaterialInterface* LoadBlackRook = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BRook"));

	for (int32 x = 0; x < Size; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			FVector Location = AChessboard::GetRelativeLocationByXYPosition(x, y);
			// SpawnActor fails if the object collides with another one, so set the location higher than a generic tile
			Location.Z = 10.0f;

			// WHITE SIDE

			// BISHOPS
			if ((x == 0 && y == 2) || (x == 0 && y == 5))
			{
				UBlueprint* BishopBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Bishop"));
				Obj = GetWorld()->SpawnActor<APieceBishop>(BishopBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::W;

				
			}

			// PAWNS
			if (x == 1)
			{
				UBlueprint* PawnsBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Pawn"));
				Obj = GetWorld()->SpawnActor<APiecePawn>(PawnsBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::W;

				
			}

			// KING
			if (x == 0 && y == 4)
			{
				UBlueprint* KingBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_King"));
				Obj = GetWorld()->SpawnActor<APieceKing>(KingBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::W;

				
			}

			// QUEEN
			if (x == 0 && y == 3)
			{
				UBlueprint* QueenBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Queen"));
				Obj = GetWorld()->SpawnActor<APieceQueen>(QueenBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::W;

				
			}

			// KNIGHTS
			if ((x == 0 && y == 1) || (x == 0 && y == 6))
			{
				UBlueprint* KnightBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Knight"));
				Obj = GetWorld()->SpawnActor<APieceKnight>(KnightBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::W;

				
			}

			// ROOKS
			if ((x == 0 && y == 0) || (x == 0 && y == 7))
			{
				UBlueprint* RookBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Rook"));
				Obj = GetWorld()->SpawnActor<APieceRook>(RookBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::W;

				
			}


			// BLACK SIDE

			// BISHOPS
			if ((x == 7 && y == 2) || (x == 7 && y == 5))
			{
				UBlueprint* BishopBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Bishop"));
				Obj = GetWorld()->SpawnActor<APieceBishop>(BishopBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::B;

				Obj->ChangeMaterial(LoadBlackBishop);

				
			}

			// PAWNS
			if (x == 6)
			{
				UBlueprint* PawnsBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Pawn"));
				Obj = GetWorld()->SpawnActor<APiecePawn>(PawnsBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::B;

				Obj->ChangeMaterial(LoadBlackPawn);

				
			}

			// KING
			if (x == 7 && y == 4)
			{
				UBlueprint* KingBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_King"));
				Obj = GetWorld()->SpawnActor<APieceKing>(KingBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::B;

				Obj->ChangeMaterial(LoadBlackKing);

				
			}

			// QUEEN
			if (x == 7 && y == 3)
			{
				UBlueprint* QueenBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Queen"));
				Obj = GetWorld()->SpawnActor<APieceQueen>(QueenBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::B;

				Obj->ChangeMaterial(LoadBlackQueen);

				
			}

			// KNIGHTS
			if ((x == 7 && y == 1) || (x == 7 && y == 6))
			{
				UBlueprint* KnightBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Knight"));
				Obj = GetWorld()->SpawnActor<APieceKnight>(KnightBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::B;

				Obj->ChangeMaterial(LoadBlackKnight);

				
			}

			// ROOKS
			if ((x == 7 && y == 0) || (x == 7 && y == 7))
			{
				UBlueprint* RookBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Rook"));
				Obj = GetWorld()->SpawnActor<APieceRook>(RookBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::B;

				Obj->ChangeMaterial(LoadBlackRook);

				
			}

		}
	}
}

FVector2D AChessboard::GetPosition(const FHitResult& Hit)
{
	return Cast<ATile>(Hit.GetActor())->GetGridPosition();
}

TArray<ATile*>& AChessboard::GetTileArray()
{
	return TileArray;
}

FVector AChessboard::GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const
{
	return TileSize * NormalizedCellPadding * FVector(InX, InY, 0);
}

FVector2D AChessboard::GetXYPositionByRelativeLocation(const FVector& Location) const
{
	const double x = Location[0] / (TileSize * NormalizedCellPadding);
	const double y = Location[1] / (TileSize * NormalizedCellPadding);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("x=%f,y=%f"), x, y));
	return FVector2D(x, y);
}
