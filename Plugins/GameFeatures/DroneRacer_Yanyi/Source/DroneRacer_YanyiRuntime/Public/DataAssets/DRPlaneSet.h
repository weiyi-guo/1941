// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DRPlaneSet.generated.h"

USTRUCT(BlueprintType)
struct FDRPlaneConfig
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString PlaneName; // Unique ID

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString PlayerStartTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const class ULyraPawnData> PawnData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxThrottleAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinThrottleAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ThrottleSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PitchSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RollSensitivity;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxRollDegree;
};

USTRUCT(BlueprintType)
struct FDRPlaneConfig_Short
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString PlaneName; // Unique ID

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxThrottleAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinThrottleAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ThrottleSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PitchSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RollSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxRollDegree;

	FDRPlaneConfig_Short()
		: PlaneName(""),
		DisplayName(""),
		MaxSpeed(0.0f),
		MaxThrottleAmount(0.0f),
		MinThrottleAmount(0.0f),
		ThrottleSensitivity(0.0f),
		PitchSensitivity(0.0f),
		RollSensitivity(0.0f),
		MaxRollDegree(0.0f)
	{}

	FDRPlaneConfig_Short(const FDRPlaneConfig& PlaneConfig)
		: PlaneName(PlaneConfig.PlaneName),
		DisplayName(PlaneConfig.DisplayName),
		MaxSpeed(PlaneConfig.MaxSpeed),
		MaxThrottleAmount(PlaneConfig.MaxThrottleAmount),
		MinThrottleAmount(PlaneConfig.MinThrottleAmount),
		ThrottleSensitivity(PlaneConfig.ThrottleSensitivity),
		PitchSensitivity(PlaneConfig.PitchSensitivity),
		RollSensitivity(PlaneConfig.RollSensitivity),
		MaxRollDegree(PlaneConfig.MaxRollDegree)
	{

	}
};

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API UDRPlaneSet : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FDRPlaneConfig> PlaneConfigs;

	UFUNCTION(BlueprintCallable)
	bool GetPlaneConfigByName(const FString& PlaneName, FDRPlaneConfig& PlaneConfig);

	UFUNCTION(BlueprintCallable)
	FDRPlaneConfig GetNextPlaneConfig(FString CurrentPlaneName);

	UFUNCTION(BlueprintCallable)
	FDRPlaneConfig GetPrevPlaneConfig(FString CurrentPlaneName);
};
