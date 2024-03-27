// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "ChessGameMode.h"
#include "ChessPlayerController.h"

void UMainHUD::NativeConstruct()
{
	Super::NativeConstruct();

	for (UButton* Button : ButtonArray)
	{
		Button->OnClicked.AddUniqueDynamic(this, &AChessPlayerController::RecreateMove);
	}
}

void UMainHUD::AddButton()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	UButton* NewButton = NewObject<UButton>(this);
	if (NewButton)
	{
		ButtonArray.Add(NewButton);

		ScrollBox->AddChild(NewButton);
	}
}
