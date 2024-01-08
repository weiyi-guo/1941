// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/DRSaveGameSubsystem.h"
#include "DRSaveGame.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/GameStateBase.h>
#include "DRPlayerState.h"

void UDRSaveGameSubsystem::SetSlotName(FString NewSlotName)
{
	if (NewSlotName.Len() > 0)
	{
		CurrentSlotName = NewSlotName;
	}
}

void UDRSaveGameSubsystem::WriteSaveGame()
{
	// TODO 2K: change to DRGameState
	AGameStateBase* DRGameState = GetWorld()->GetGameState();
	if (!DRGameState)
	{
		return;
	}

	// TODO: we only have one local player
	for (int32 i = 0; i < DRGameState->PlayerArray.Num(); ++i)
	{
		ADRPlayerState* DRPlayerState = Cast<ADRPlayerState>(DRGameState->PlayerArray[i]);
		DRPlayerState->SavePlayerState(CurrentSaveGame);
	}

	if (!UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Try to WriteSaveGame Failed"));
	}
}

void UDRSaveGameSubsystem::LoadSaveGame(FString InSlotName)
{
	SetSlotName(InSlotName);

	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
	{ 
		CurrentSaveGame = Cast<UDRSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));

		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));
	}

	if (!CurrentSaveGame)
	{
		CurrentSaveGame = Cast<UDRSaveGame>(UGameplayStatics::CreateSaveGameObject(UDRSaveGame::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("Created New SaveGame Data."));
	}
}

void UDRSaveGameSubsystem::HandleStartingNewPlayer(class AController* NewPlayer)
{
	// UE_LOG(LogTemp, Warning, TEXT("DRSaveGameSubsystem HandleStartingNewPlayer"));

	ADRPlayerState* DRPlayerState = NewPlayer->GetPlayerState<ADRPlayerState>();
	if(DRPlayerState)
	{ 
		DRPlayerState->LoadPlayerState(CurrentSaveGame);
	}
}

void UDRSaveGameSubsystem::UpdateResolutionLevel(int32 ResolutionLevel)
{
	if (CurrentSaveGame)
	{
		CurrentSaveGame->ResolutionLevel = ResolutionLevel;
	}
}

const int32 UDRSaveGameSubsystem::GetResolutionLevelFromSaveGame() const
{
	if (CurrentSaveGame)
	{
		return CurrentSaveGame->ResolutionLevel;
	}
	return 0;
}

const float UDRSaveGameSubsystem::GetBestRecordFromSaveGame() const
{
	if (CurrentSaveGame)
	{
		return CurrentSaveGame->BestRecordInSeconds;
	}
	return 0.0f;
}
