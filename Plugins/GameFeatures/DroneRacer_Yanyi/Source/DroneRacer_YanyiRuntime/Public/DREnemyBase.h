// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/LyraCharacter.h"
#include "DRPerceptiveActorInterface.h"
#include "DREnemyBase.generated.h"

UCLASS()
class DRONERACER_YANYIRUNTIME_API ADREnemyBase : public ACharacter, public IDRPerceptiveActorInterface, public ILyraTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADREnemyBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Enemy", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroneRacer|Enemy", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraSystem> ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroneRacer|Enemy", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ULyraHealthComponent> LyraHealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Enemy", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroneRacer|Enemy", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ULyraGameplayAbility> DamageAbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroneRacer|Enemy", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ULyraGameplayAbility> DeathAbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroneRacer|Enemy", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroneRacer|Enemy", Meta = (AllowPrivateAccess = "true"))
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroneRacer|Enemy", Meta = (AllowPrivateAccess = "true"))
	float TargetRandomRange = 100.0f;;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroneRacer|Enemy", Meta = (AllowPrivateAccess = "true"))
	float DamageAmount = 10.0f;

	virtual void OnActorPerceptionUpdated_Implementation(AActor* Actor, bool WasSuccessfullySensed);

	//~ILyraTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnLyraTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of ILyraTeamAgentInterface interface

protected:

	UPROPERTY(VisibleAnywhere, Category = "DroneRacer|Enemy")
	TObjectPtr<class ULyraAbilitySystemComponent> AbilitySystemComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	virtual void OnExploded();

	virtual void InitializeEnemy();

	virtual void RegisterEnemy();

	UFUNCTION()
	void OnSphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	TArray<TWeakObjectPtr<AActor>> TargetActors;

	UFUNCTION(BlueprintNativeEvent)
	void TraceTargetActors();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AttackTargetActor(const FVector& TargetActorLocation);

	UFUNCTION(BlueprintImplementableEvent)
	FVector GetMuzzleLocation();

	UFUNCTION()
	void OnHealthChanged(class ULyraHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* InstigatorActor);

	UFUNCTION()
	void OnDeathStarted(AActor* Actor);

	UFUNCTION()
	void OnDeathFinished(class UNiagaraComponent* NiagaraComponent);

#if !UE_BUILD_SHIPPING
	int32 BulletsAmount = 0;

	int32 SucceededHitAmount = 0;
#endif

public:	
	
	virtual void PostInitializeComponents() override;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Enemy", Meta = (AllowPrivateAccess = "true"))
	FGenericTeamId TeamID;

	UPROPERTY()
	FOnLyraTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	TObjectPtr<class UAttributeSet> HealthSet;

};
