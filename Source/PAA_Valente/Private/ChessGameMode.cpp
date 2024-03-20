// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameMode.h"
#include "WhitePlayer.h"
#include "PieceKing.h"
#include "PieceQueen.h"
#include "PieceRook.h"
#include "PieceBishop.h"
#include "PieceKnight.h"
#include "ChessPlayerController.h"
#include "BlackRandomPlayer.h"
#include "PlayerInterface.h"
#include "EngineUtils.h"

AChessGameMode::AChessGameMode()
{
	DefaultPawnClass = AWhitePlayer::StaticClass();
	PlayerControllerClass = AChessPlayerController::StaticClass();
	FieldSize = 8;
	bIsGameOver = false;
	bIsWhiteOnCheck = false;
	bIsBlackOnCheck = false;
	bIsBlackThinking = false;
	TurnFlag = 0;
}

void AChessGameMode::BeginPlay()
{
	Super::BeginPlay();

	AWhitePlayer* HumanPlayer = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));

	// Random Player
	auto* AI = GetWorld()->SpawnActor<ABlackRandomPlayer>(FVector(), FRotator());

	// MiniMax Player
	//auto* AI = GetWorld()->SpawnActor<ATTT_MinimaxPlayer>(FVector(), FRotator());

	if (CBClass != nullptr)
	{
		CB = GetWorld()->SpawnActor<AChessboard>(CBClass);
		CB->Size = FieldSize;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
	}

	float CameraPosX = ((CB->TileSize * (FieldSize + ((FieldSize - 1) * CB->NormalizedCellPadding) - (FieldSize - 1))) / 2) - (CB->TileSize / 2);
	FVector CameraPos(CameraPosX, CameraPosX, 1000.0f);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

	SetKings();
	HumanPlayer->OnTurn();
}

void AChessGameMode::SetKings()
{
	// Finds WhiteKing
	for (APiece* WhitePiece : CB->WhitePieces)
	{
		if (Cast<APieceKing>(WhitePiece))
		{
			CB->Kings[0] = WhitePiece;
			break;
		}
	}
	// Finds BlackKing
	for (APiece* BlackPiece : CB->BlackPieces)
	{
		if (Cast<APieceKing>(BlackPiece))
		{
			CB->Kings[1] = BlackPiece;
			break;
		}
	}
}

void AChessGameMode::VerifyCheck(APiece* Piece)
{
	ATile** WhiteKingTile = CB->TileMap.Find(FVector2D(CB->Kings[0]->RelativePosition().X, CB->Kings[0]->RelativePosition().Y));
	ATile** BlackKingTile = CB->TileMap.Find(FVector2D(CB->Kings[1]->RelativePosition().X, CB->Kings[1]->RelativePosition().Y));
	
	if (Piece->Color == EColor::B)
	{
		for (APiece* WhitePiece : CB->WhitePieces)
		{
			WhitePiece->PossibleMoves();
			if (WhitePiece->EatablePieces.Contains(*BlackKingTile))
			{
				bIsBlackOnCheck = true;
				break;
			}
			else
			{
				bIsBlackOnCheck = false;
			}
		}
	}

	else if (Piece->Color == EColor::W)
	{
		for (APiece* BlackPiece : CB->BlackPieces)
		{
			BlackPiece->PossibleMoves();
			if (BlackPiece->EatablePieces.Contains(*WhiteKingTile))
			{
				bIsWhiteOnCheck = true;
				break;
			}
			else
			{
				bIsWhiteOnCheck = false;
			}
		}
	}
}

void AChessGameMode::VerifyWin(APiece* Piece)
{
	int32 NumberOfPiecesWithoutLegalMoves = 0;
	TArray<APiece*> AllyPieces;

	if (Piece->Color == EColor::W)
	{
		AllyPieces = CB->WhitePieces;
	}
	else
	{
		AllyPieces = CB->BlackPieces;
	}

	for (APiece* AllyPiece : AllyPieces)
	{
		AllyPiece->PossibleMoves();
		AllyPiece->FilterOnlyLegalMoves();
		if (AllyPiece->Moves.Num() == 0 && AllyPiece->EatablePieces.Num() == 0)
		{
			NumberOfPiecesWithoutLegalMoves++;
		}
	}

	if (NumberOfPiecesWithoutLegalMoves == AllyPieces.Num())
	{
		bIsGameOver = true;
	}
}

void AChessGameMode::VerifyTie(APiece* Piece)
{
	// 
	bool bCondition1 = CB->Kings[0]->Moves.IsEmpty();
	bool bCondition2 = CB->Kings[0]->EatablePieces.IsEmpty();
	bool bCondition3 = true;

	bool bTie = false;

	for (APiece* BlackPiece : CB->BlackPieces)
	{
		if (BlackPiece->EatablePieces.Contains(CB->Kings[0]))
		{
			bCondition3 = false;
			break;
		}
	}

	if (bCondition1 && bCondition2 && bCondition3)
	{
		bTie = true;
		// TODO: Implement Tie Message
	}
}

void AChessGameMode::TurnPlayer()
{
	AWhitePlayer* HumanPlayer = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));
	ABlackRandomPlayer* AIPlayer = Cast<ABlackRandomPlayer>(*TActorIterator<ABlackRandomPlayer>(GetWorld()));

	if (TurnFlag == 0)
	{
		TurnFlag++;
		VerifyCheck(CB->Kings[1]);
		VerifyWin(CB->Kings[1]);
		if (!bIsGameOver)
		{
			AIPlayer->OnTurn();
		}
		else
		{
			HumanPlayer->OnWin();
		}
	}
	else if (TurnFlag == 1)
	{
		TurnFlag--;
		VerifyCheck(CB->Kings[0]);
		VerifyWin(CB->Kings[0]);
		if (!bIsGameOver)
		{
			HumanPlayer->OnTurn();
		}
		else
		{
			AIPlayer->OnWin();
		}
	}
}

void AChessGameMode::PromoteToQueen()
{
	UBlueprint* QueenBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Queen"));
	APiece* Obj = nullptr;

	FVector Location = PawnToPromote->GetActorLocation();

	if (PawnToPromote->Color == EColor::W)
	{
		PawnPromotionWidgetInstance->RemoveFromParent();

		CB->WhitePieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceQueen>(QueenBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->Color = EColor::W;
		CB->WhitePieces.Add(Obj);
	}
	else if (PawnToPromote->Color == EColor::B)
	{
		UMaterialInterface* LoadBlackQueen = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BQueen"));

		CB->BlackPieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceQueen>(QueenBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->ChangeMaterial(LoadBlackQueen);
		Obj->Color = EColor::B;
		CB->BlackPieces.Add(Obj);
	}

	TurnPlayer();
}

void AChessGameMode::PromoteToRook()
{
	UBlueprint* RookBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Rook"));
	APiece* Obj = nullptr;

	FVector Location = PawnToPromote->GetActorLocation();

	if (PawnToPromote->Color == EColor::W)
	{
		PawnPromotionWidgetInstance->RemoveFromParent();

		CB->WhitePieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceRook>(RookBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->Color = EColor::W;
		CB->WhitePieces.Add(Obj);
	}
	else if (PawnToPromote->Color == EColor::B)
	{
		UMaterialInterface* LoadBlackRook = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BRook"));

		CB->BlackPieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceRook>(RookBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->ChangeMaterial(LoadBlackRook);
		Obj->Color = EColor::B;
		CB->BlackPieces.Add(Obj);
	}

	TurnPlayer();
}

void AChessGameMode::PromoteToBishop()
{
	UBlueprint* BishopBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Bishop"));
	APiece* Obj = nullptr;

	FVector Location = PawnToPromote->GetActorLocation();

	if (PawnToPromote->Color == EColor::W)
	{
		PawnPromotionWidgetInstance->RemoveFromParent();

		CB->WhitePieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceBishop>(BishopBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->Color = EColor::W;
		CB->WhitePieces.Add(Obj);
	}
	else if (PawnToPromote->Color == EColor::B)
	{
		UMaterialInterface* LoadBlackBishop = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BBishop"));

		CB->BlackPieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceBishop>(BishopBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->ChangeMaterial(LoadBlackBishop);
		Obj->Color = EColor::B;
		CB->BlackPieces.Add(Obj);
	}

	TurnPlayer();
}

void AChessGameMode::PromoteToKnight()
{
	UBlueprint* KnightBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Knight"));
	APiece* Obj = nullptr;

	FVector Location = PawnToPromote->GetActorLocation();

	if (PawnToPromote->Color == EColor::W)
	{
		PawnPromotionWidgetInstance->RemoveFromParent();

		CB->WhitePieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceKnight>(KnightBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->Color = EColor::W;
		CB->WhitePieces.Add(Obj);
	}
	else if (PawnToPromote->Color == EColor::B)
	{
		UMaterialInterface* LoadBlackKnight = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BKnight"));

		CB->BlackPieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceKnight>(KnightBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->ChangeMaterial(LoadBlackKnight);
		Obj->Color = EColor::B;
		CB->BlackPieces.Add(Obj);
	}

	TurnPlayer();
}