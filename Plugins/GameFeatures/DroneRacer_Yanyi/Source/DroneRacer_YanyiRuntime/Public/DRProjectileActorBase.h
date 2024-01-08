// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DRPoolableActorBase.h"
#include "DRProjectileActorBase.generated.h"

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API ADRProjectileActorBase : public ADRPoolableActorBase
{
	GENERATED_BODY()

public:

	ADRProjectileActorBase();

	virtual void PostInitializeComponents() override;

	void OnActive_Implementation(class APawn* NewInstigator, class AActor* NewOwner) override;

	void OnDeActive_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void OnExplode();

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|ProjectileActor", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|ProjectileActor", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|ProjectileActor", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraComponent> ExplosionEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|ProjectileActor", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraComponent> TracerEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|ProjectileActor", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer|ProjectileActor", Meta = (AllowPrivateAccess = "true"))
	float InitialSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer|ProjectileActor", Meta = (AllowPrivateAccess = "true"))
	float MaxLifeTime;

	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer|ProjectileActor", Meta = (AllowPrivateAccess = "true"))
	float DamageAmount;

	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer|ProjectileActor", Meta = (AllowPrivateAccess = "true"))
	TArray<FName> TargetTags; // TODO: filter targets with tags

	FTimerHandle MaxLifeTimerHandle;

	FTimerDelegate MaxLifeTimerDelegate;

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(AActor* TargetActor);

	UFUNCTION()
	void OnExplosionEffectFinished(UNiagaraComponent* ParticleSystem);

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
