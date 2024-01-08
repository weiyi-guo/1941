// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/DRWidgetInterface.h"
#include "DRUserWidget_GameOver.generated.h"

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API UDRUserWidget_GameOver : public UUserWidget, public IDRWidgetInterface
{
	GENERATED_BODY()
	
public:
	
	virtual bool UpdateWidget_Implementation(const FString& Payload) override;

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCommonTextBlock> Text_BattleResult;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCommonTextBlock> Text_BestRecord;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCommonTextBlock> Text_CurrentRecord;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCommonTextBlock> Text_Player1Name;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCommonTextBlock> Text_Player1Score;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCommonTextBlock> Text_Player2Name;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCommonTextBlock> Text_Player2Score;
};
