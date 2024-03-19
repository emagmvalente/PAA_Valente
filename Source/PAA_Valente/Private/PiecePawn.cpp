// Fill out your copyright notice in the Description page of Project Settings.


#include "PiecePawn.h"
#include "PieceQueen.h"
#include "PieceRook.h"
#include "PieceBishop.h"
#include "PieceKnight.h"
#include "ChessPlayerController.h"
#include "EngineUtils.h"

// Sets default values
APiecePawn::APiecePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// template function that creates a components
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);
}

// Called when the game starts or when spawned
void APiecePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void APiecePawn::PromoteToQueen()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	UBlueprint* QueenBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Queen"));
	FVector Location = GetActorLocation();

	GameMode->CB->WhitePieces.Remove(this);
	this->Destroy();

	if (PawnPromotionWidgetInstance)
	{
		PawnPromotionWidgetInstance->RemoveFromViewport();
	}
	
	APiece* Obj = GetWorld()->SpawnActor<APieceQueen>(QueenBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
	if (Color == EColor::W)
	{
		Obj->Color = EColor::W;
		GameMode->CB->WhitePieces.Add(Obj);
	}
	else if (Color == EColor::B)
	{
		UMaterialInterface* LoadBlackQueen = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BQueen"));
		Obj->ChangeMaterial(LoadBlackQueen);
		Obj->Color = EColor::B;
		GameMode->CB->BlackPieces.Add(Obj);
	}

	GameMode->CB->WhitePieces.Add(Obj);
	GameMode->TurnPlayer();
}

void APiecePawn::PromoteToRook()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	UBlueprint* RookBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Rook"));
	FVector Location = GetActorLocation();

	GameMode->CB->WhitePieces.Remove(this);
	this->Destroy();

	if (PawnPromotionWidgetInstance)
	{
		PawnPromotionWidgetInstance->RemoveFromViewport();
	}

	APiece* Obj = GetWorld()->SpawnActor<APieceRook>(RookBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
	if (Color == EColor::W)
	{
		Obj->Color = EColor::W;
		GameMode->CB->WhitePieces.Add(Obj);
	}
	else if (Color == EColor::B)
	{
		UMaterialInterface* LoadBlackRook = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BRook"));
		Obj->ChangeMaterial(LoadBlackRook);
		Obj->Color = EColor::B;
		GameMode->CB->BlackPieces.Add(Obj);
	}

	GameMode->CB->WhitePieces.Add(Obj);
	GameMode->TurnPlayer();
}

void APiecePawn::PromoteToBishop()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	UBlueprint* BishopBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Bishop"));
	FVector Location = GetActorLocation();

	GameMode->CB->WhitePieces.Remove(this);
	this->Destroy();

	if (PawnPromotionWidgetInstance)
	{
		PawnPromotionWidgetInstance->RemoveFromViewport();
	}

	APiece* Obj = GetWorld()->SpawnActor<APieceBishop>(BishopBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
	if (Color == EColor::W)
	{
		Obj->Color = EColor::W;
		GameMode->CB->WhitePieces.Add(Obj);
	}
	else if (Color == EColor::B)
	{
		UMaterialInterface* LoadBlackBishop = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BBishop"));
		Obj->ChangeMaterial(LoadBlackBishop);
		Obj->Color = EColor::B;
		GameMode->CB->BlackPieces.Add(Obj);
	}

	GameMode->CB->WhitePieces.Add(Obj);
	GameMode->TurnPlayer();
}

void APiecePawn::PromoteToKnight()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	UBlueprint* KnightBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Knight"));
	FVector Location = GetActorLocation();

	GameMode->CB->WhitePieces.Remove(this);
	this->Destroy();

	if (PawnPromotionWidgetInstance)
	{
		PawnPromotionWidgetInstance->RemoveFromViewport();
	}

	APiece* Obj = GetWorld()->SpawnActor<APieceKnight>(KnightBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);

	if (Color == EColor::W)
	{
		Obj->Color = EColor::W;
		GameMode->CB->WhitePieces.Add(Obj);
	}
	else if (Color == EColor::B)
	{
		UMaterialInterface* LoadBlackKnight = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BKnight"));
		Obj->ChangeMaterial(LoadBlackKnight);
		Obj->Color = EColor::B;
		GameMode->CB->BlackPieces.Add(Obj);
	}

	GameMode->TurnPlayer();
}

void APiecePawn::Promote()
{
	PawnPromotionWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), PawnPromotionWidgetClass);

	if (Color == EColor::W && RelativePosition().X == 7)
	{
		if (PawnPromotionWidgetInstance)
		{
			PawnPromotionWidgetInstance->AddToViewport();
		}
	}

	else if (Color == EColor::B && RelativePosition().X == 0)
	{
		int32 RandIdx0 = FMath::Rand() % 4;

		switch (RandIdx0)
		{
			case 0:
				PromoteToQueen();
				break;
			case 1:
				PromoteToRook();
				break;
			case 2:
				PromoteToBishop();
				break;
			case 3:
				PromoteToKnight();
				break;
			default: 
				return;
		}
	}
}

// Called every frame
void APiecePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APiecePawn::PossibleMoves()
{
	Moves.Empty();
	EatablePieces.Empty();

	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector ActorLocation = RelativePosition();
	FVector2D TileLocation(ActorLocation.X, ActorLocation.Y);
	ATile** NextTile = GameMode->CB->TileMap.Find(TileLocation);

	// If the pawn is black, then invert his movements
	if (Color == EColor::B)
	{
		Directions = { FVector2D(-1, 0), FVector2D(-1, -1), FVector2D(-1, 1) };
	}

	for (const FVector2D& Direction : Directions)
	{
		FVector2D NextPosition = TileLocation + Direction;
		NextTile = GameMode->CB->TileMap.Find(NextPosition);

		if (NextTile != nullptr)
		{
			if (Direction == Directions[0] && (*NextTile)->GetOccupantColor() == EOccupantColor::E)
			{
				Moves.Add((*NextTile));

				NextPosition += Direction;
				NextTile = GameMode->CB->TileMap.Find(NextPosition);

				if (NextTile != nullptr && Direction == Directions[0] && bFirstMove && (*NextTile)->GetOccupantColor() == EOccupantColor::E)
				{
					Moves.Add(*NextTile);
				}

				continue;
			}
			else if (!IsSameColorAsTileOccupant(*NextTile) && (*NextTile)->GetOccupantColor() != EOccupantColor::E && Direction != Directions[0])
			{
				EatablePieces.Add(*NextTile);
				continue;
			}
		}
		else
		{
			continue;
		}
	}
}
