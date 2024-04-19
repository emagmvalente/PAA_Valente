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
#include "BlackMinimaxPlayer.h"
#include "EngineUtils.h"
#include "ChessPlayerController.h"
#include "MainHUD.h"

AChessGameMode::AChessGameMode()
{
	DefaultPawnClass = AWhitePlayer::StaticClass();
	PlayerControllerClass = AChessPlayerController::StaticClass();
	FieldSize = 8;
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
		UE_LOG(LogTemp, Error, TEXT("Chessboard is null"));
	}

	float CameraPosX = ((CB->TileSize * FieldSize) / 2) - (CB->TileSize / 2);
	FVector CameraPos(CameraPosX, CameraPosX, 1200.0f);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

	HumanPlayer->OnTurn();
}

// Logic and Utilities
void AChessGameMode::TurnPlayer()
{
	AWhitePlayer* HumanPlayer = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));
	ABlackRandomPlayer* AIPlayer = Cast<ABlackRandomPlayer>(*TActorIterator<ABlackRandomPlayer>(GetWorld()));
	//ABlackMinimaxPlayer* AIPlayer = Cast<ABlackMinimaxPlayer>(*TActorIterator<ABlackMinimaxPlayer>(GetWorld()));

	UChessGameInstance* GameInstance = Cast<UChessGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	GameInstance->SetNotificationMessage(TEXT(""));

	// Draw case
	if (VerifyDraw())
	{
		GameInstance->SetTurnMessage(TEXT("Draw!"));
	}

	// Win case
	else if (VerifyCheck() && VerifyCheckmate())
	{
		GameInstance->SetNotificationMessage(TEXT("Checkmate!"));
		if (TurnFlag == 0)
		{
			HumanPlayer->OnWin();
		}
		else if (TurnFlag == 1)
		{
			AIPlayer->OnWin();
		}
	}

	// Turn passing
	else
	{
		// Comunicate check
		if (VerifyCheck())
		{
			GameInstance->SetNotificationMessage(TEXT("Check!"));
		}

		// Anyway, pass the turn
		if (TurnFlag == 0)
		{
			TurnFlag++;
			AIPlayer->OnTurn();
		}
		else if (TurnFlag == 1)
		{
			TurnFlag--;
			HumanPlayer->OnTurn();
		}
	}
}

void AChessGameMode::ResetVariablesForRematch()
{
	TurnFlag = 0;
	MovesWithoutCaptureOrPawnMove = 0;
}

int32 AChessGameMode::GetTurnFlag() const
{
	return TurnFlag;
}

// Winning / Draw / Losing

bool AChessGameMode::VerifyCheck()
{
	ATile* EnemyKingTile = nullptr;
	TArray<APiece*> AllyPieces;

	if (TurnFlag == 0)
	{
		EnemyKingTile = *CB->TileMap.Find(CB->Kings[1]->GetVirtualPosition());
		AllyPieces = CB->WhitePieces;
	}
	else if (TurnFlag == 1)
	{
		EnemyKingTile = *CB->TileMap.Find(CB->Kings[0]->GetVirtualPosition());;
		AllyPieces = CB->BlackPieces;
	}

	for (APiece* AllyPiece : AllyPieces)
	{
		AllyPiece->PossibleMoves();
		AllyPiece->FilterOnlyLegalMoves();

		// Check detection
		if (AllyPiece->Moves.Contains(EnemyKingTile))
		{
			return true;
		}
	}

	return false;
}

bool AChessGameMode::VerifyCheckmate()
{
	TArray<APiece*> EnemyPieces;

	if (TurnFlag == 0)
	{
		EnemyPieces = CB->BlackPieces;
	}
	else if (TurnFlag == 1)
	{
		EnemyPieces = CB->WhitePieces;
	}

	for (APiece* EnemyPiece : EnemyPieces)
	{
		EnemyPiece->PossibleMoves();
		EnemyPiece->FilterOnlyLegalMoves();
		if (EnemyPiece->Moves.Num() > 0)
		{
			return false;
		}
	}

	return true;
}

bool AChessGameMode::VerifyDraw()
{
	UChessGameInstance* GameInstance = Cast<UChessGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	// TODO: Dead Positions
	if (CheckThreeOccurrences() || KingvsKing() || /*FiftyMovesRule() ||*/ Stalemate())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Draw!"));
		GameInstance->SetTurnMessage(TEXT("Draw!"));
		return true;
	}
	return false;
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
	/*
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
		if (Cast<APiecePawn>(WhitePawn) && Cast<APiecePawn>(WhitePawn)->GetTurnsWithoutMoving() > 0)
		{
			bWasMovedAPawn = true;
			break;
		}
	}
	if (!bWasMovedAPawn)
	{
		for (APiece* BlackPawn : CB->BlackPieces)
		{
			if (Cast<APiecePawn>(BlackPawn) && Cast<APiecePawn>(BlackPawn)->GetTurnsWithoutMoving() > 0)
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
	*/

	return false;
}

bool AChessGameMode::Stalemate()
{
	if (!VerifyCheck())
	{
		return VerifyCheckmate();
	}
	return false;
}

// Pawn Promotion
void AChessGameMode::PromoteToQueen()
{
	UBlueprint* QueenBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Queen"));
	APiece* Obj = nullptr;

	FVector Location = PawnToPromote->GetActorLocation();
	FVector2D Location2D = CB->GetXYPositionByRelativeLocation(Location);

	if (PawnToPromote->GetColor() == EColor::W)
	{
		PawnPromotionWidgetInstance->RemoveFromParent();

		CB->WhitePieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceQueen>(QueenBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->SetColor(EColor::W);
		Obj->SetVirtualPosition(Location2D);
		CB->WhitePieces.Add(Obj);
	}
	else if (PawnToPromote->GetColor() == EColor::B)
	{
		UMaterialInterface* LoadBlackQueen = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BQueen"));

		CB->BlackPieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceQueen>(QueenBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->ChangeMaterial(LoadBlackQueen);
		Obj->SetColor(EColor::B);
		Obj->SetVirtualPosition(Location2D);
		CB->BlackPieces.Add(Obj);
	}

	TurnPlayer();
}

void AChessGameMode::PromoteToRook()
{
	UBlueprint* RookBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Rook"));
	APiece* Obj = nullptr;

	FVector Location = PawnToPromote->GetActorLocation();
	FVector2D Location2D = CB->GetXYPositionByRelativeLocation(Location);

	if (PawnToPromote->GetColor() == EColor::W)
	{
		PawnPromotionWidgetInstance->RemoveFromParent();

		CB->WhitePieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceRook>(RookBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->SetColor(EColor::W);
		Obj->SetVirtualPosition(Location2D);
		CB->WhitePieces.Add(Obj);
	}
	else if (PawnToPromote->GetColor() == EColor::B)
	{
		UMaterialInterface* LoadBlackRook = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BRook"));

		CB->BlackPieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceRook>(RookBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->ChangeMaterial(LoadBlackRook);
		Obj->SetColor(EColor::B);
		Obj->SetVirtualPosition(Location2D);
		CB->BlackPieces.Add(Obj);
	}

	TurnPlayer();
}

void AChessGameMode::PromoteToBishop()
{
	UBlueprint* BishopBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Bishop"));
	APiece* Obj = nullptr;

	FVector Location = PawnToPromote->GetActorLocation();
	FVector2D Location2D = CB->GetXYPositionByRelativeLocation(Location);

	if (PawnToPromote->GetColor() == EColor::W)
	{
		PawnPromotionWidgetInstance->RemoveFromParent();

		CB->WhitePieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceBishop>(BishopBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->SetColor(EColor::W);
		Obj->SetVirtualPosition(Location2D);
		CB->WhitePieces.Add(Obj);
	}
	else if (PawnToPromote->GetColor() == EColor::B)
	{
		UMaterialInterface* LoadBlackBishop = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BBishop"));

		CB->BlackPieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceBishop>(BishopBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->ChangeMaterial(LoadBlackBishop);
		Obj->SetColor(EColor::B);
		Obj->SetVirtualPosition(Location2D);
		CB->BlackPieces.Add(Obj);
	}

	TurnPlayer();
}

void AChessGameMode::PromoteToKnight()
{
	UBlueprint* KnightBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Knight"));
	APiece* Obj = nullptr;

	FVector Location = PawnToPromote->GetActorLocation();
	FVector2D Location2D = CB->GetXYPositionByRelativeLocation(Location);

	if (PawnToPromote->GetColor() == EColor::W)
	{
		PawnPromotionWidgetInstance->RemoveFromParent();

		CB->WhitePieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceKnight>(KnightBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->SetColor(EColor::W);
		Obj->SetVirtualPosition(Location2D);
		CB->WhitePieces.Add(Obj);
	}
	else if (PawnToPromote->GetColor() == EColor::B)
	{
		UMaterialInterface* LoadBlackKnight = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BKnight"));

		CB->BlackPieces.Remove(PawnToPromote);
		PawnToPromote->Destroy();

		Obj = GetWorld()->SpawnActor<APieceKnight>(KnightBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
		Obj->ChangeMaterial(LoadBlackKnight);
		Obj->SetColor(EColor::B);
		Obj->SetVirtualPosition(Location2D);
		CB->BlackPieces.Add(Obj);
	}

	TurnPlayer();
}