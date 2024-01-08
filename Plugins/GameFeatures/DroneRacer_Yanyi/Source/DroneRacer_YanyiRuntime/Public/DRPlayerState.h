// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/LyraPlayerState.h"
#include "DataAssets/DRPlaneSet.h"
#include "DRPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API ADRPlayerState : public ALyraPlayerState
{
	GENERATED_BODY()

public:
	ADRPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Lobby")
	bool OverrideDefaultPawnData = false;

protected:
	UPROPERTY(VisibleAnywhere)
	float BestRecordInSeconds;

	UPROPERTY(VisibleAnywhere)
	float CurrentRecordInSeconds;

	UPROPERTY(VisibleAnywhere)
	FString CurrentSelectedPlane;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Lobby", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDRPlaneSet> PlaneSet;

public:

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(class UDRSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(class UDRSaveGame* SaveObject);

	UFUNCTION(BlueprintCallable)
	void UpdatePersonalRecord(float TimeInSeconds);

	UFUNCTION(BlueprintCallable)
	float GetBestRecord() const { return BestRecordInSeconds; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentRecord() const { return CurrentRecordInSeconds; }

	UFUNCTION(BlueprintCallable)
	void UpdateSelectedPlane(const FString& NewPlaneName);

	UFUNCTION(BlueprintCallable)
	FString GetSelectedPlaneName() { return CurrentSelectedPlane; }

	UFUNCTION(BlueprintCallable)
	bool GetSelectedPlaneConfig(FDRPlaneConfig& PlaneConfig);

	UFUNCTION(BlueprintCallable)
	FString GetDefaultPlaneName();

	UFUNCTION(BlueprintCallable)
	void AddScores(float DeltaScores);

	UFUNCTION(BlueprintCallable)
	void SetPawnDataByName(FString NewPlaneName);

protected:

	virtual void OnRep_Score() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	
	void UpdateUI();
};
