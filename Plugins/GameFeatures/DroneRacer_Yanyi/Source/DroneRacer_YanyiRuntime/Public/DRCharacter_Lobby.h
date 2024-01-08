// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraCharacter.h"
#include "DataAssets/DRPlaneSet.h"
#include "DRCharacter_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API ADRCharacter_Lobby : public AModularCharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FirstPersonCamera;

public:

	ADRCharacter_Lobby(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PossessedBy(AController* NewController) override;
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Lobby")
	void ViewNextPlane();

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Lobby")
	void ViewPrevPlane();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Lobby", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDRPlaneSet> PlaneSet;

	UPROPERTY(BlueprintReadOnly, Category = "DroneRacer|Lobby", Meta = (AllowPrivateAccess = "true"))
	TMap<FName, TObjectPtr<class APlayerStart>> ViewPointForPlanes;

	FVector TargetViewPosition;

	bool NeedMoving = false;

	FString CurrentViewingPlane;

	UPROPERTY()
	TObjectPtr<class UDRWidgetManagerComponent> WidgetManagerComponent;

	void UpdateUI(const FDRPlaneConfig& PlaneConfig);
};
