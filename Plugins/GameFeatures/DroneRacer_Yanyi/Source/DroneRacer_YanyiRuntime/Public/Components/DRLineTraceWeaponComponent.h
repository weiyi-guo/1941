// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DRLineTraceWeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DRONERACER_YANYIRUNTIME_API UDRLineTraceWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDRLineTraceWeaponComponent();

private:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Line Trace Weapon", Meta = (AllowPrivateAccess = "true"))
	float MaxTraceDistance = 10000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Line Trace Weapon", Meta = (AllowPrivateAccess = "true"))
	double TimeLastFired = 0.0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Line Trace Weapon", Meta = (AllowPrivateAccess = "true"))
	float WeaponFireRate = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Line Trace Weapon", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraSystem> BulletTracerEffect;

public:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintPure, Category = "DroneRacer|Line Trace Weapon")
	static UDRLineTraceWeaponComponent* FindLineTraceWeaponComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UDRLineTraceWeaponComponent>() : nullptr); }

	UFUNCTION(BlueprintPure, Category = "DroneRacer|Line Trace Weapon")
	bool CanOpenFire() const;

	bool GetMuzzlePositions(OUT TArray<FVector>& MuzzlePositions) const;

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Line Trace Weapon")
	FVector GetTargetingDirection() const;

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Line Trace Weapon")
	void OnFire();

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Line Trace Weapon")
	float GetMaxTraceDistance() const { return MaxTraceDistance; }

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Line Trace Weapon")
	float GetFireRate() const { return WeaponFireRate; }

private:

	UPROPERTY()
	TArray<class UActorComponent*> Muzzles;

	UPROPERTY()
	TMap<FString, class UNiagaraComponent*> TracerComponents;

	bool IsUserTrigger = false;
};
