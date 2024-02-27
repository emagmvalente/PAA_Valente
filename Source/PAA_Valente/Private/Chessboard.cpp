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

	FString GeneratingString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	GeneratePositionsFromString(GeneratingString);

	// Setting the status of the tiles
	for (int32 i = 0; i < 16; i++)
	{
		TileArray[i]->SetOccupantColor(EOccupantColor::W);
		TileArray[i]->SetTileStatus(ETileStatus::OCCUPIED);
	}
	for (int32 i = 63; i >= 48; i--)
	{
		TileArray[i]->SetOccupantColor(EOccupantColor::B);
		TileArray[i]->SetTileStatus(ETileStatus::OCCUPIED);
	}
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

			const float TileScale = TileSize / 100;
			Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
			Obj->SetGridPosition(x, y);
			TileArray.Add(Obj);
			TileMap.Add(FVector2D(x, y), Obj);
		}
	}
}

FString AChessboard::GenerateStringFromPositions()
{
	FString ResultantString = TEXT("");

	for (int32 Row = 7; Row >= 0; --Row)
	{
		int32 EmptyCount = 0;
		for (int32 Col = 0; Col < 8; ++Col)
		{
			ATile** CurrentTile = TileMap.Find(FVector2D(Row, Col));
			if ((*CurrentTile)->GetTileStatus() == ETileStatus::OCCUPIED && (*CurrentTile)->GetOccupantColor() == EOccupantColor::W)
			{
				for (int32 i = 0; i < WhitePieces.Num(); ++i)
				{
					FVector Location(Row, Col, 10.f);
					APiece* PieceFound = WhitePieces[i];

					if (Cast<APiecePawn>(PieceFound) && PieceFound->RelativePosition() == Location)
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('P');
						break;
					}
					else if (Cast<APieceKing>(PieceFound) && PieceFound->RelativePosition() == Location)
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);

							EmptyCount = 0;
						}
						ResultantString.AppendChar('K');
						break;
					}
					else if (Cast<APieceKnight>(PieceFound) && PieceFound->RelativePosition() == Location)
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('N');
						break;
					}
					else if (Cast<APieceQueen>(PieceFound) && PieceFound->RelativePosition() == Location)
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('Q');
						break;
					}
					else if (Cast<APieceRook>(PieceFound) && PieceFound->RelativePosition() == Location)
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('R');
						break;
					}
					else if (Cast<APieceBishop>(PieceFound) && PieceFound->RelativePosition() == Location)
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('B');
						break;
					}
				}
			}
			else if ((*CurrentTile)->GetTileStatus() == ETileStatus::OCCUPIED && (*CurrentTile)->GetOccupantColor() == EOccupantColor::B)
			{
				for (int32 i = 0; i < BlackPieces.Num(); ++i)
				{
					FVector Location(Row, Col, 10.f);
					APiece* PieceFound = BlackPieces[i];

					if (Cast<APiecePawn>(PieceFound) && PieceFound->RelativePosition() == Location)
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('p');
						break;
					}
					else if (Cast<APieceKing>(PieceFound) && PieceFound->RelativePosition() == Location)
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('k');
						break;
					}
					else if (Cast<APieceKnight>(PieceFound) && PieceFound->RelativePosition() == Location)
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('n');
						break;
					}
					else if (Cast<APieceQueen>(PieceFound) && PieceFound->RelativePosition() == Location)
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('q');
						break;
					}
					else if (Cast<APieceRook>(PieceFound) && PieceFound->RelativePosition() == Location)
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('r');
						break;
					}
					else if (Cast<APieceBishop>(PieceFound) && PieceFound->RelativePosition() == Location)
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('b');
						break;
					}
				}
			}
			else
			{
				++EmptyCount;
			}
		}
		if (EmptyCount > 0)
		{
			ResultantString.AppendInt(EmptyCount);
		}
		if (Row > 0)
		{
			ResultantString.AppendChar('/');
		}
	}

	return ResultantString;
}

void AChessboard::GeneratePositionsFromString(FString& String)
{
	// Emptying old pieces (if there are any) to recreate the chosen move
	for (int32 i = 0; i < WhitePieces.Num(); i++)
	{
		WhitePieces[i]->Destroy();
		BlackPieces[i]->Destroy();
	}
	for (int32 i = 0; i < TileArray.Num(); i++)
	{
		TileArray[i]->SetOccupantColor(EOccupantColor::E);
		TileArray[i]->SetTileStatus(ETileStatus::EMPTY);
	}
	WhitePieces.Empty();
	BlackPieces.Empty();
	
	// Skin declarations
	UMaterialInterface* LoadBlackBishop = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BBishop"));
	UMaterialInterface* LoadBlackPawn = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BPawn"));
	UMaterialInterface* LoadBlackKing = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BKing"));
	UMaterialInterface* LoadBlackQueen = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BQueen"));
	UMaterialInterface* LoadBlackKnight = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BKnight"));
	UMaterialInterface* LoadBlackRook = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BRook"));

	// Blueprint declarations
	UBlueprint* PawnsBlueprint;
	UBlueprint* BishopBlueprint;
	UBlueprint* KingBlueprint;
	UBlueprint* QueenBlueprint;
	UBlueprint* KnightBlueprint;
	UBlueprint* RookBlueprint;

	// Rows and Columns indexes
	int32 Row = 7;
	int32 Col = 0;

	// Loop through each character in the FEN string
	for (int32 i = 0; i < String.Len(); ++i)
	{
		TCHAR Char = String[i];

		// Slash case
		if (Char == '/')
		{
			--Row; // Move to the next row
			Col = 0; // Reset the column
		}
		// Digit case
		else if (FChar::IsDigit(Char))
		{
			if (Char - '0' > 8)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Invalid String."));
				break;
			}
			Col += Char - '0'; // Increment the column by the number of empty squares
		}
		// Letter case
		else
		{
			APiece* Obj = nullptr;

			FVector Location = AChessboard::GetRelativeLocationByXYPosition(Row, Col);
			Location.Z = 10.f;

			// Determine the piece type based on the character
			switch (Char)
			{
			// WHITE PIECES
			case 'P':
				PawnsBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Pawn"));
				Obj = GetWorld()->SpawnActor<APiecePawn>(PawnsBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::W;

				WhitePieces.Add(Obj);
				break;

			case 'N':
				KnightBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Knight"));
				Obj = GetWorld()->SpawnActor<APieceKnight>(KnightBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::W;

				WhitePieces.Add(Obj);
				break;

			case 'B':
				BishopBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Bishop"));
				Obj = GetWorld()->SpawnActor<APieceBishop>(BishopBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::W;

				WhitePieces.Add(Obj);
				break;

			case 'R':
				RookBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Rook"));
				Obj = GetWorld()->SpawnActor<APieceRook>(RookBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::W;

				WhitePieces.Add(Obj);
				break;

			case 'Q':
				QueenBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Queen"));
				Obj = GetWorld()->SpawnActor<APieceQueen>(QueenBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::W;

				WhitePieces.Add(Obj);
				break;

			case 'K':
				KingBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_King"));
				Obj = GetWorld()->SpawnActor<APieceKing>(KingBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::W;

				WhitePieces.Add(Obj);
				break;

			
			// BLACK PIECES
			case 'p':
				PawnsBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Pawn"));
				Obj = GetWorld()->SpawnActor<APiecePawn>(PawnsBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::B;
				
				Obj->ChangeMaterial(LoadBlackPawn);

				BlackPieces.Add(Obj);
				break;

			case 'n':
				KnightBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Knight"));
				Obj = GetWorld()->SpawnActor<APieceKnight>(KnightBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::B;

				Obj->ChangeMaterial(LoadBlackKnight);

				BlackPieces.Add(Obj);
				break;

			case 'b':
				BishopBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Bishop"));
				Obj = GetWorld()->SpawnActor<APieceBishop>(BishopBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::B;

				Obj->ChangeMaterial(LoadBlackBishop);

				BlackPieces.Add(Obj);
				break;

			case 'r':
				RookBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Rook"));
				Obj = GetWorld()->SpawnActor<APieceRook>(RookBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::B;

				Obj->ChangeMaterial(LoadBlackRook);

				BlackPieces.Add(Obj);
				break;

			case 'q':
				QueenBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Queen"));
				Obj = GetWorld()->SpawnActor<APieceQueen>(QueenBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::B;

				Obj->ChangeMaterial(LoadBlackQueen);

				BlackPieces.Add(Obj);
				break;

			case 'k':
				KingBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_King"));
				Obj = GetWorld()->SpawnActor<APieceKing>(KingBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->Color = EColor::B;

				Obj->ChangeMaterial(LoadBlackKing);

				BlackPieces.Add(Obj);
				break;

			default:
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Invalid String."));
				break;
			}

			++Col; // Move to the next column
		}
	}
}

FVector2D AChessboard::GetPosition(const FHitResult& Hit)
{
	return Cast<ATile>(Hit.GetActor())->GetGridPosition();
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

TArray<ATile*> AChessboard::GetTileArray() const
{
	return TileArray;
}
