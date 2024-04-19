// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "WhitePlayer.h"
#include "EngineUtils.h"

void UMainHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainHUD::AddButton(FString AssociatedString, APiece* PieceMoved, bool bItWasACapture, FVector2D NewPosition, FVector2D OldPosition)
{
    UOldMovesButtons* NewButton = NewObject<UOldMovesButtons>(this);
    if (NewButton)
    {
        NewButton->OnClicked.AddDynamic(NewButton, &UOldMovesButtons::ButtonOnClickFunction);
        NewButton->GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
        NewButton->HumanPlayer = Cast<AWhitePlayer>(*TActorIterator<AWhitePlayer>(GetWorld()));
        NewButton->GameInstance = Cast<UChessGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        NewButton->SetAssociatedString(NewButton->GameMode->CB->HistoryOfMoves.Last());
        NewButton->CreateText(PieceMoved, bItWasACapture, PieceMoved->GetVirtualPosition(), OldPosition);
        NewButton->SetAssociatedString(AssociatedString);

        ButtonArray.Add(NewButton);

        ScrollBox->AddChild(NewButton);
    }
}

void UMainHUD::DestroyButtons()
{
    if (ScrollBox)
    {
        ScrollBox->ClearChildren();
    }
}
