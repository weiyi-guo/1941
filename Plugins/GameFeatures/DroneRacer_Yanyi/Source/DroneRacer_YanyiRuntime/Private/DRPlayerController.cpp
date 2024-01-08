// Fill out your copyright notice in the Description page of Project Settings.


#include "DRPlayerController.h"
#include "Components/DRWidgetManagerComponent.h"
#include "DroneRacerGameMode.h"

#include "Net/UnrealNetwork.h"
#include "DRPlayerState.h"

void ADRPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ADRPlayerController::OnClientChangeReadyStatus_Implementation(bool IsReady)
{
	if (HasAuthority())
	{
		IsPlayerReady = IsReady;

		AGameModeBase* const GameMode = GetWorld()->GetAuthGameMode();
		if (ADroneRacerGameMode* DroneRacerGameMode = Cast<ADroneRacerGameMode>(GameMode))
		{
			DroneRacerGameMode->OnPlayerReady();
		}
	}
}

void ADRPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADRPlayerController, IsPlayerReady);
}

void ADRPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
}

void ADRPlayerController::SetClientSelectedPlane(FString& NewPlaneName)
{
	ClientSelectedPlaneName = NewPlaneName;
	if (GetNetMode() != NM_Client)
	{
		if (ADRPlayerState* DRPlayerStat = GetPlayerState<ADRPlayerState>())
		{
			DRPlayerStat->SetPawnDataByName(ClientSelectedPlaneName);
		}
	}
}

void ADRPlayerController::ClientTravelInternal_Implementation(const FString& URL, ETravelType TravelType, bool bSeamless, FGuid MapPackageGuid)
{
	if(ADRPlayerState* DRPlayerStat = GetPlayerState<ADRPlayerState>())
	{ 
		FString Address = FString::Printf(TEXT("%s?PlaneName=%s"), *URL, *DRPlayerStat->GetSelectedPlaneName());
		Super::ClientTravelInternal_Implementation(Address, TravelType, bSeamless, MapPackageGuid);
	}
	else
	{
		Super::ClientTravelInternal_Implementation(URL, TravelType, bSeamless, MapPackageGuid);
	}
}
