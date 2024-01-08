// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "DRUserWidget_GameMenu.generated.h"

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API UDRUserWidget_GameMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UDRUserWidget_GameMenu(const FObjectInitializer& ObjectInitializer);

	void NativeOnInitialized() override;
	
protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCommonTextBlock> Text_BestRecord;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCommonTextBlock> Text_Resolution;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> Btn_StartGame;

	void HandleEscapeAction();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_HandleEscapeAction();

	UFUNCTION(BlueprintCallable)
	void ResolutionUp();

	UFUNCTION(BlueprintCallable)
	void ResolutionDown();

	void GetStandardWindowResolution(int32 Level, FIntPoint& OutResolution);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 ResolutionLevel;

	void UpdateResolution();

	UFUNCTION()
	void OnStartGameBtnClicked();
};
