// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameMode.h"
#include "PieceKing.h"
#include "PieceQueen.h"
#include "PieceRook.h"
#include "PieceBishop.h"
#include "PieceKnight.h"
#include "PiecePawn.h"
#include "ChessPlayerController.h"
#include "WhitePlayer.h"
#include "BlackRandomPlayer.h"
#include "EngineUtils.h"
#include "ChessPlayerController.h"
#include "MainHUD.h"

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
	MovesWithoutCaptureOrPawnMove = 0;
}

void AChessGameMode::BeginPlay()
{
	Super::BeginPlay();

	AWhitePlayer* HumanPlayer = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));

	// Random Player
	auto* AI = GetWorld()->SpawnActor<ABlackRandomPlayer>(FVector(), FRotator());

	// MiniMax Player
	//auto* AI = GetWorld()->SpawnActor<ABlackMinimaxPlayer>(FVector(), FRotator());

	if (CBClass != nullptr)
	{
		CB = GetWorld()->SpawnActor<AChessboard>(CBClass);
		CB->Size = FieldSize;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
	}

	float CameraPosX = ((CB->TileSize * FieldSize) / 2) - (CB->TileSize / 2);
	FVector CameraPos(CameraPosX, CameraPosX, 1000.0f);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

	SetKings();
	HumanPlayer->OnTurn();
}

// Logic and Utilities
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

void AChessGameMode::TurnPlayer()
{
	AWhitePlayer* HumanPlayer = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));
	ABlackRandomPlayer* AIPlayer = Cast<ABlackRandomPlayer>(*TActorIterator<ABlackRandomPlayer>(GetWorld()));

	VerifyCheck();
	VerifyDraw();

	if (TurnFlag == 0)
	{
		TurnFlag++;
		if (!bIsGameOver)
		{
			AIPlayer->OnTurn();
		}
		else if (bIsGameOver && bIsBlackOnCheck)
		{
			HumanPlayer->OnWin();
		}
		else if (bIsGameOver && !bIsBlackOnCheck)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Draw!"));
		}
	}
	else if (TurnFlag == 1)
	{
		TurnFlag--;
		if (!bIsGameOver)
		{
			HumanPlayer->OnTurn();
		}
		else if (bIsGameOver && bIsWhiteOnCheck)
		{
			AIPlayer->OnWin();
		}
		else if (bIsGameOver && !bIsWhiteOnCheck)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Draw!"));
		}
	}
}

// Winning / Draw / Losing

// ANCORA MIGLIORABILE ELIMINANDO LE VARIABILI BOOLEANE DI CHECK E SPEZZANDO VERIFYCHECK IN DUE METODI BOOLEANI
void AChessGameMode::VerifyCheck()
{
	AWhitePlayer* HumanPlayer = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));
	ABlackRandomPlayer* AIPlayer = Cast<ABlackRandomPlayer>(*TActorIterator<ABlackRandomPlayer>(GetWorld()));
	ATile** WhiteKingTile = CB->TileMap.Find(FVector2D(CB->Kings[0]->RelativePosition().X, CB->Kings[0]->RelativePosition().Y));
	ATile** BlackKingTile = CB->TileMap.Find(FVector2D(CB->Kings[1]->RelativePosition().X, CB->Kings[1]->RelativePosition().Y));
	int32 NumberOfPiecesWithoutLegalMoves = 0;

	// White puts black on check
	for (APiece* WhitePiece : CB->WhitePieces)
	{
		WhitePiece->PossibleMoves();

		// Check detection
		if (WhitePiece->EatablePiecesPosition.Contains(*BlackKingTile))
		{
			bIsBlackOnCheck = true;
			break;
		}
		else
		{
			bIsBlackOnCheck = false;
		}

		// Checkmate / Stalemate detection
		for (APiece* BlackPiece : CB->BlackPieces)
		{
			BlackPiece->PossibleMoves();
			BlackPiece->FilterOnlyLegalMoves();
			if (BlackPiece->Moves.Num() == 0 && BlackPiece->EatablePiecesPosition.Num() == 0)
			{
				NumberOfPiecesWithoutLegalMoves++;
			}
		}
		if (NumberOfPiecesWithoutLegalMoves == CB->BlackPieces.Num())
		{
			bIsGameOver = true;
		}
	}

	// Black puts white on check
	if (!bIsBlackOnCheck)
	{
		for (APiece* BlackPiece : CB->BlackPieces)
		{
			BlackPiece->PossibleMoves();

			// Check detection
			if (BlackPiece->EatablePiecesPosition.Contains(*WhiteKingTile))
			{
				bIsWhiteOnCheck = true;
				break;
			}
			else
			{
				bIsWhiteOnCheck = false;
			}

			// Checkmate / Stalemate detection
			for (APiece* WhitePiece : CB->WhitePieces)
			{
				WhitePiece->PossibleMoves();
				WhitePiece->FilterOnlyLegalMoves();
				if (WhitePiece->Moves.Num() == 0 && WhitePiece->EatablePiecesPosition.Num() == 0)
				{
					NumberOfPiecesWithoutLegalMoves++;
				}
			}
			if (NumberOfPiecesWithoutLegalMoves == CB->WhitePieces.Num())
			{
				bIsGameOver = true;
			}
		}
	}
}

void AChessGameMode::VerifyDraw()
{
	// TODO: Dead Positions
	// Stalemate is evaluated in VerifyWin
	if (CheckThreeOccurrences() || KingvsKing() || FiftyMovesRule())
	{
		bIsGameOver = true;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Draw!"));
	}
}

bool AChessGameMode::CheckThreeOccurrences()
{
	TMap<FString, int32> CountMap;

	// Conta quante volte ogni elemento appare nell'array
	for (FString Occurency : CB->HistoryOfMoves) {
		if (!CountMap.Contains(Occurency)) {
			CountMap.Add(Occurency, 1);
		}
		else {
			CountMap[Occurency]++;
		}
	}

	// Verifica se uno qualsiasi degli elementi appare tre volte
	for (const auto& Pair : CountMap) {
		if (Pair.Value >= 3) {
			return true;
		}
	}

	return false;
}

bool AChessGameMode::KingvsKing()
{
	if (CB->WhitePieces.Num() == 1 && CB->BlackPieces.Num() == 1) {
		return true;
	}
	return false;
}

bool AChessGameMode::FiftyMovesRule()
{
	// If the number reaches 50, then it's a draw situation
	if (MovesWithoutCaptureOrPawnMove >= 50)
	{
		return true;
	}

	// Declarations
	int32 NumberOfPiecesInPreviousMove = 0;
	int32 NumberOfPiecesNow = 0;
	bool bWasMovedAPawn = false;
	FString ActualMove = CB->HistoryOfMoves.Last();
	FString PreviousMove = FString("");
	if (CB->HistoryOfMoves.Num() > 1)
	{
		PreviousMove = CB->HistoryOfMoves[CB->HistoryOfMoves.Num() - 2];
	}

	// Capture check
	for (int32 i = 0; i < ActualMove.Len(); i++)
	{
		TCHAR PieceInActualMove = ActualMove[i];
		if (FChar::IsAlpha(PieceInActualMove))
		{
			NumberOfPiecesNow++;
		}
	}
	if (PreviousMove != FString(""))
	{
		for (int32 i = 0; i < PreviousMove.Len(); i++)
		{
			TCHAR PieceInPreviousMove = PreviousMove[i];
			if (FChar::IsAlpha(PieceInPreviousMove))
			{
				NumberOfPiecesInPreviousMove++;
			}
		}
	}

	// Pawn movement check
	for (APiece* WhitePawn : CB->WhitePieces)
	{
		if (Cast<APiecePawn>(WhitePawn) && Cast<APiecePawn>(WhitePawn)->TurnsWithoutMoving > 0)
		{
			bWasMovedAPawn = true;
			break;
		}
	}
	if (!bWasMovedAPawn)
	{
		for (APiece* BlackPawn : CB->BlackPieces)
		{
			if (Cast<APiecePawn>(BlackPawn) && Cast<APiecePawn>(BlackPawn)->TurnsWithoutMoving > 0)
			{
				bWasMovedAPawn = true;
				break;
			}
		}
	}

	if ((NumberOfPiecesNow == NumberOfPiecesInPreviousMove) && !bWasMovedAPawn)
	{
		MovesWithoutCaptureOrPawnMove++;
	}
	else
	{
		MovesWithoutCaptureOrPawnMove = 0;
	}

	return false;
}

// Pawn Promotion
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