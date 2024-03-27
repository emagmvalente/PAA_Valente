// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"

void UMainHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainHUD::AddButton()
{
    UOldMovesButtons* NewButton = NewObject<UOldMovesButtons>(this);
    if (NewButton)
    {
        NewButton->OnClicked.AddDynamic(NewButton, &UOldMovesButtons::ButtonOnClickFunction);
        ButtonArray.Add(NewButton);

        UTextBlock* NumberOfMove = NewObject<UTextBlock>(this);
        FString TextValue = FString::FromInt(IntToKeepTrack);
        NumberOfMove->SetText(FText::FromString(TextValue));

        NewButton->AddChild(NumberOfMove);

        ScrollBox->AddChild(NewButton);

        IntToKeepTrack++;
    }
}
