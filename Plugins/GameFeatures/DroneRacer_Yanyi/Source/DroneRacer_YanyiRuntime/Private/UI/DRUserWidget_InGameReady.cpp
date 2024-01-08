// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DRUserWidget_InGameReady.h"
#include "Components/Button.h"

//#include "Kismet/GameplayStatics.h"
#include "Components/DRWidgetManagerComponent.h"
#include "DRPlayerController.h"

//#include "DroneRacerGameMode.h"

bool UDRUserWidget_InGameReady::UpdateWidget_Implementation(const FString& Payload)
{
	return false;
}

void UDRUserWidget_InGameReady::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UDRUserWidget_InGameReady::Initialize()
{
	bool Result = Super::Initialize();

	if (Btn_StartGame)
	{
		Btn_StartGame->OnClicked.AddDynamic(this, &ThisClass::OnStartGameBtnClicked);
	}

	return Result;
}

void UDRUserWidget_InGameReady::OnStartGameBtnClicked()
{
	APlayerController* LocalPlayerController = GetOwningPlayer();
	if (UDRWidgetManagerComponent* WidgetManager = UDRWidgetManagerComponent::GetComponent(LocalPlayerController))
	{
		WidgetManager->RequestHideWidget(FName("WBP_InGameReady"));
		WidgetManager->RequestShowWidget(FName("WBP_Waiting"));
	}

	if (ADRPlayerController* DRPlayerController = Cast<ADRPlayerController>(LocalPlayerController))
	{
		DRPlayerController->OnClientChangeReadyStatus(true);
	}
}
