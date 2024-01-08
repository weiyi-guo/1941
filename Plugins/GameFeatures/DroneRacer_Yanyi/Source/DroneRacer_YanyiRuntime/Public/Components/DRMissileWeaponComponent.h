// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DRMissileWeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DRONERACER_YANYIRUNTIME_API UDRMissileWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDRMissileWeaponComponent();

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Missile Weapon", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ADRPoolableActorBase> MissileActorClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Missilee Weapon", Meta = (AllowPrivateAccess = "true"))
	double TimeLastFired = 0.0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Missile Weapon", Meta = (AllowPrivateAccess = "true"))
	float WeaponFireRate = 5.0f;

public:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "DroneRacer|Missile Weapon")
	bool CanOpenFire() const;

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Missile Weapon")
	void OnFire();
};
