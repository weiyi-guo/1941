// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/DRWidgetInterface.h"
#include "DRUserWidget_InGameReady.generated.h"

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API UDRUserWidget_InGameReady : public UUserWidget, public IDRWidgetInterface
{
	GENERATED_BODY()

public:

	virtual bool UpdateWidget_Implementation(const FString& Payload) override;

protected:

	virtual void NativeConstruct() override;

	virtual bool Initialize() override;
	
protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> Btn_StartGame;

	UFUNCTION()
	void OnStartGameBtnClicked();
};
