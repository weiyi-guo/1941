// Fill out your copyright notice in the Description page of Project Settings.


#include "DRPlayerState.h"
#include "DRSaveGame.h"
#include "Attributes/DRCombatSet.h"
#include "Subsystems/DRSaveGameSubsystem.h"
#include "Components/DRWidgetManagerComponent.h"
#include "UI/DRUserWidget_InGameHUD.h"
#include <Net/UnrealNetwork.h>

ADRPlayerState::ADRPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CreateDefaultSubobject<UDRCombatSet>(TEXT("DRCombatSet"));
}

void ADRPlayerState::SavePlayerState_Implementation(class UDRSaveGame* SaveObject)
{
	if (SaveObject)
	{
		// UE_LOG(LogTemp, Warning, TEXT("ADRPlayerState Save Player State %f"), BestRecordInSeconds);
		SaveObject->BestRecordInSeconds = BestRecordInSeconds;
		SaveObject->SelectedPlaneName = CurrentSelectedPlane;
	}
}

void ADRPlayerState::LoadPlayerState_Implementation(class UDRSaveGame* SaveObject)
{
	if(SaveObject)
	{ 
		// UE_LOG(LogTemp, Warning, TEXT("ADRPlayerState Load Player State %f"), SaveObject->BestRecordInSeconds);
		BestRecordInSeconds = SaveObject->BestRecordInSeconds;
		CurrentSelectedPlane = SaveObject->SelectedPlaneName.IsEmpty() ? GetDefaultPlaneName() : SaveObject->SelectedPlaneName;

		FDRPlaneConfig CurrentPlaneConfig;
		if (PlaneSet && GetSelectedPlaneConfig(CurrentPlaneConfig) && OverrideDefaultPawnData)
		{
			SetPawnData(CurrentPlaneConfig.PawnData);
		}
	}
}

void ADRPlayerState::UpdatePersonalRecord(float TimeInSeconds)
{
	CurrentRecordInSeconds = TimeInSeconds;
	if (FMath::IsNearlyZero(BestRecordInSeconds) || CurrentRecordInSeconds < BestRecordInSeconds)
	{
		BestRecordInSeconds = CurrentRecordInSeconds;
	}
}

void ADRPlayerState::UpdateSelectedPlane(const FString& NewPlaneName)
{
	CurrentSelectedPlane = NewPlaneName;

	// TODO: 2K Yanyi Temp Code
	UDRSaveGameSubsystem* DRSaveGameSubsystem = GetGameInstance()->GetSubsystem<UDRSaveGameSubsystem>();
	DRSaveGameSubsystem->WriteSaveGame();
}

bool ADRPlayerState::GetSelectedPlaneConfig(FDRPlaneConfig& PlaneConfig)
{
	if (PlaneSet)
	{
		for (auto& Config : PlaneSet->PlaneConfigs)
		{
			if (Config.PlaneName.Equals(CurrentSelectedPlane))
			{
				PlaneConfig = Config;
				return true;
			}
		}
	}

	return false;
}

FString ADRPlayerState::GetDefaultPlaneName()
{
	check(PlaneSet);

	check(PlaneSet->PlaneConfigs.Num() > 0);

	return PlaneSet->PlaneConfigs[0].PlaneName;
}

void ADRPlayerState::AddScores(float DeltaScores)
{
	if (HasAuthority())
	{
		SetScore(GetScore() + DeltaScores);
	}

	// TODO BAD CODE
	if (GetPlayerController()->IsLocalController())
	{
		UpdateUI();
	}
}

void ADRPlayerState::SetPawnDataByName(FString NewPlaneName)
{
	CurrentSelectedPlane = NewPlaneName;
	FDRPlaneConfig CurrentPlaneConfig;
	if (PlaneSet && GetSelectedPlaneConfig(CurrentPlaneConfig) && OverrideDefaultPawnData)
	{
		SetPawnData(CurrentPlaneConfig.PawnData);
	}
}

void ADRPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	UpdateUI();
}

void ADRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ADRPlayerState::UpdateUI()
{
	if (APlayerController* PlayerController = GetPlayerController())
	{
		if (UDRWidgetManagerComponent* WidgetManager = UDRWidgetManagerComponent::GetComponent(PlayerController))
		{
			if (UDRUserWidget_InGameHUD* InGameHUD = Cast<UDRUserWidget_InGameHUD>(WidgetManager->GetReferenceOfWidget(FName("WBP_InGameHUD"))))
			{
				InGameHUD->UpdatePlayerScores(GetScore());
			}
		}
	}
}
