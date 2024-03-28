// Fill out your copyright notice in the Description page of Project Settings.


#include "OldMovesButtons.h"

UOldMovesButtons::UOldMovesButtons()
{
	AssociatedString = FString("");
}

void UOldMovesButtons::ButtonOnClickFunction()
{
	if (AssociatedString != FString("") && !GameMode->bIsBlackThinking)
	{
		if (CPC->SelectedPieceToMove)
		{
			CPC->SelectedPieceToMove->DecolorPossibleMoves();
		}
		GameMode->CB->GeneratePositionsFromString(AssociatedString);
		GameMode->CB->SetTilesOwners();
		GameMode->SetKings();
	}
	else if (GameMode->bIsBlackThinking)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Shhh! Black is thinking... Replay later."));
	}
}
