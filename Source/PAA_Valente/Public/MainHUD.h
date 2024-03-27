// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "MainHUD.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedButton, class UButton*, ButtonClicked);

/**
 * 
 */
UCLASS()
class PAA_VALENTE_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddButton();

	FOnClickedButton OnClickedDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ResetButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ResetText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* ScrollBox;

	TArray<UButton*> ButtonArray;

	virtual void NativeConstruct() override;
};
