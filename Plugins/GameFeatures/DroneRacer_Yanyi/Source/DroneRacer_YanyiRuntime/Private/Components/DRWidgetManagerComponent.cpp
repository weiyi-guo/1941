// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DRWidgetManagerComponent.h"
#include "Blueprint/UserWidget.h"
#include "DataAssets/DRWidgetSet.h"
#include "UI/DRWidgetInterface.h"


UDRWidgetManagerComponent::UDRWidgetManagerComponent(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	bAutoRegister = true;
	bAutoActivate = true;
}

UDRWidgetManagerComponent* UDRWidgetManagerComponent::GetComponent(AController* Controller)
{
	if (Controller)
	{
		return Controller->FindComponentByClass<UDRWidgetManagerComponent>();
	}

	return nullptr;
}

UUserWidget* UDRWidgetManagerComponent::RequestShowWidget(const FName& WidgetName)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());

	if (!PlayerController->IsLocalController())
	{
		return false;
	}

	if (LoadedWidget.Contains(WidgetName))
	{
		LoadedWidget[WidgetName]->AddToPlayerScreen();
		return LoadedWidget[WidgetName];
	}
	else
	{
		FDRWidgetConfig WidgetConfig;
		if (PlayerController && WidgetSet && WidgetSet->FindWidgetConfigByName(WidgetName, WidgetConfig))
		{
			TSubclassOf<class UUserWidget> WidgetClass = WidgetConfig.WidgetClass;

			UUserWidget* NewUserWidget = CreateWidget<UUserWidget>(PlayerController, WidgetClass, WidgetName);
			if (!NewUserWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("CreateWidget %s failed for player %s"), *WidgetName.ToString(), *PlayerController->GetName());
				return false;
			}
			LoadedWidget.Add(WidgetName, NewUserWidget);
			NewUserWidget->AddToPlayerScreen();

			// Stupid code
			switch (WidgetConfig.OverrideInputMode)
			{
				case EDRInputMode::EDR_GameOnly:
					PlayerController->SetInputMode(FInputModeGameOnly());
					break;
				case EDRInputMode::EDR_GameAndUI:
					PlayerController->SetInputMode(FInputModeGameAndUI());
					break;
				case EDRInputMode::EDR_UIOnly:
					PlayerController->SetInputMode(FInputModeUIOnly());
					break;
				default:
					break;
			}
			
			PlayerController->SetShowMouseCursor(WidgetConfig.ShowMouseCursor);
			return LoadedWidget[WidgetName];
		}
	}

	return nullptr;
}

bool UDRWidgetManagerComponent::RequestHideWidget(const FName& WidgetName)
{
	if (LoadedWidget.Contains(WidgetName))
	{
		LoadedWidget[WidgetName]->RemoveFromParent();

		APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
		if (PlayerController)
		{
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->SetShowMouseCursor(false);
		}
		return true;
	}

	return false;
}

bool UDRWidgetManagerComponent::RequestUpdateWidget(const FName& WidgetName, FString& Payload)
{
	if (LoadedWidget.Contains(WidgetName))
	{
		UUserWidget* TargetWidget = LoadedWidget[WidgetName];
		if (TargetWidget->Implements<UDRWidgetInterface>())
		{
			IDRWidgetInterface::Execute_UpdateWidget(TargetWidget, Payload);
			return true;
		}
	}

	return false;
}

bool UDRWidgetManagerComponent::RequestHideAllWidgets()
{
	for (const TPair<FName, TObjectPtr<class UUserWidget>>& WidgetInfo : LoadedWidget)
	{
		if (WidgetInfo.Value)
		{
			WidgetInfo.Value->RemoveFromParent();
		}
	}

	return true;
}

UUserWidget* UDRWidgetManagerComponent::GetReferenceOfWidget(const FName& WidgetName)
{
	if (LoadedWidget.Contains(WidgetName))
	{
		return LoadedWidget[WidgetName];
	}

	return nullptr;
}
