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
#include "MainHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

AChessGameMode::AChessGameMode()
{
	DefaultPawnClass = AWhitePlayer::StaticClass();
	PlayerControllerClass = AChessPlayerController::StaticClass();
	FieldSize = 8;
	TurnFlag = 0;
	MovesWithoutCaptureOrPawnMove = 0;
}

void AChessGameMode::BeginPlay()
{
	Super::BeginPlay();

	auto* WhitePlayer = GetWorld()->SpawnActor<ABlackRandomPlayer>(FVector(), FRotator());
	auto* BlackPlayer = GetWorld()->SpawnActor<ABlackRandomPlayer>(FVector(), FRotator());

	Players.Add(WhitePlayer);
	Players.Add(BlackPlayer);

	if (CBClass != nullptr)
	{
		CB = GetWorld()->SpawnActor<AChessboard>(CBClass);
		CB->Size = FieldSize;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Chessboard is null"));
	}

	WhitePlayer->SetTeam(EColor::W);
	BlackPlayer->SetTeam(EColor::B);

	float CameraPosX = ((CB->TileSize * FieldSize) / 2) - (CB->TileSize / 2);
	FVector CameraPos(CameraPosX, CameraPosX, 1200.0f);

	auto Camera = GetWorld()->SpawnActor<AWhitePlayer>(FVector(), FRotator());
	Camera->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

	WhitePlayer->OnTurn();
}

// Logic and Utilities
void AChessGameMode::TurnPlayer()
{
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
		Players[TurnFlag]->OnWin();
	}

	// Turn passing
	else
	{
		// Comunicate check
		if (VerifyCheck())
		{
			GameInstance->SetNotificationMessage(TEXT("Check!"));
		}

		// Pass the turn
		TurnFlag = (TurnFlag == 0) ? 1 : 0;
		Players[TurnFlag]->OnTurn();
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

void AChessGameMode::SpawnHumanAndRandom()
{
	AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());

	// If white's random, then it's the menu screen
	if (Cast<ABlackRandomPlayer>(Players[0]))
	{
		// Clear every timer
		auto* AI1 = Cast<ABlackRandomPlayer>(Players[0]);
		GetWorldTimerManager().ClearTimer(*AI1->GetTimerHandle());
		auto* AI2 = Cast<ABlackRandomPlayer>(Players[1]);
		GetWorldTimerManager().ClearTimer(*AI2->GetTimerHandle());

		auto Camera = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));

		Camera->Destroy();
	}
	else
	{
		if (Cast<ABlackRandomPlayer>(Players[1]))
		{
			GetWorldTimerManager().ClearTimer(*Cast<ABlackRandomPlayer>(Players[1])->GetTimerHandle());
		}
		if (Cast<ABlackMinimaxPlayer>(Players[1]))
		{
			GetWorldTimerManager().ClearTimer(*Cast<ABlackMinimaxPlayer>(Players[1])->GetTimerHandle());
		}
	}
	
	Players[0]->DestroyPlayer();	Players[1]->DestroyPlayer();	Players.Empty();

	auto* WhitePlayer = GetWorld()->SpawnActor<AWhitePlayer>(FVector(), FRotator());
	auto* BlackPlayer = GetWorld()->SpawnActor<ABlackRandomPlayer>(FVector(), FRotator());

	Players.Add(WhitePlayer);
	Players.Add(BlackPlayer);

	WhitePlayer->SetTeam(EColor::W);
	BlackPlayer->SetTeam(EColor::B);

	float CameraPosX = ((CB->TileSize * FieldSize) / 2) - (CB->TileSize / 2);
	FVector CameraPos(CameraPosX, CameraPosX, 1200.0f);

	WhitePlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

	CB->ResetField();

	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UMainHUD::StaticClass());
	CPC->MainHUDWidget = Cast<UMainHUD>(FoundWidgets[0]);

	WhitePlayer->OnTurn();
}

void AChessGameMode::SpawnHumanAndMinimax()
{
	AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());

	// If white's random, then it's the menu screen
	if (Cast<ABlackRandomPlayer>(Players[0]))
	{
		auto* AI1 = Cast<ABlackRandomPlayer>(Players[0]);
		GetWorldTimerManager().ClearTimer(*AI1->GetTimerHandle());
		auto* AI2 = Cast<ABlackRandomPlayer>(Players[1]);
		GetWorldTimerManager().ClearTimer(*AI2->GetTimerHandle());

		auto Camera = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));

		Camera->Destroy();
	}
	else
	{
		if (Cast<ABlackRandomPlayer>(Players[1]))
		{
			GetWorldTimerManager().ClearTimer(*Cast<ABlackRandomPlayer>(Players[1])->GetTimerHandle());
		}
		if (Cast<ABlackMinimaxPlayer>(Players[1]))
		{
			GetWorldTimerManager().ClearTimer(*Cast<ABlackMinimaxPlayer>(Players[1])->GetTimerHandle());
		}
	}

	Players[0]->DestroyPlayer();	Players[1]->DestroyPlayer();	Players.Empty();

	auto* WhitePlayer = GetWorld()->SpawnActor<AWhitePlayer>(FVector(), FRotator());
	auto* BlackPlayer = GetWorld()->SpawnActor<ABlackMinimaxPlayer>(FVector(), FRotator());

	Players.Add(WhitePlayer);
	Players.Add(BlackPlayer);

	WhitePlayer->SetTeam(EColor::W);
	BlackPlayer->SetTeam(EColor::B);

	float CameraPosX = ((CB->TileSize * FieldSize) / 2) - (CB->TileSize / 2);
	FVector CameraPos(CameraPosX, CameraPosX, 1200.0f);

	WhitePlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

	CB->ResetField();

	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UMainHUD::StaticClass());
	CPC->MainHUDWidget = Cast<UMainHUD>(FoundWidgets[0]);

	WhitePlayer->OnTurn();
}

// Winning / Draw / Losing

bool AChessGameMode::VerifyCheck()
{
	ATile* EnemyKingTile = (TurnFlag == 0) ? CB->TileMap[CB->Kings[1]->GetVirtualPosition()] : 
											 CB->TileMap[CB->Kings[0]->GetVirtualPosition()];

	TArray<APiece*> AllyPieces = (TurnFlag == 0) ? CB->WhitePieces : CB->BlackPieces;

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
	TArray<APiece*> EnemyPieces = (TurnFlag == 0) ? CB->BlackPieces : CB->WhitePieces;

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
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Draw!"));
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
	FVector2D Location2D = PawnToPromote->GetVirtualPosition();

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
	FVector2D Location2D = PawnToPromote->GetVirtualPosition();

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
	FVector2D Location2D = PawnToPromote->GetVirtualPosition();

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
	FVector2D Location2D = PawnToPromote->GetVirtualPosition();

	// If white then remove the widget and spawn the wanted piece
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
	// If black then spawn the wanted piece
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