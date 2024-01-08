// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DRSaveGameSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API UDRSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
protected:

	FString CurrentSlotName = "DroneRacerSaveGame";

	UPROPERTY()
	TObjectPtr<class UDRSaveGame> CurrentSaveGame;

public:
	
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void SetSlotName(FString NewSlotName);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();

	void LoadSaveGame(FString InSlotName = "");

	void HandleStartingNewPlayer(class AController* NewPlayer);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	const float GetBestRecordFromSaveGame() const;

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	const int32 GetResolutionLevelFromSaveGame() const;

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void UpdateResolutionLevel(int32 ResolutionLevel);
};
