// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DRSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API UDRSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category = "DroneRacer|SaveGame")
	float BestRecordInSeconds;

	UPROPERTY(VisibleAnywhere, Category = "DroneRacer|SaveGame")
	FString SelectedPlaneName;

	UPROPERTY(VisibleAnywhere, Category = "DroneRacer|SaveGame")
	int32 ResolutionLevel;
};
