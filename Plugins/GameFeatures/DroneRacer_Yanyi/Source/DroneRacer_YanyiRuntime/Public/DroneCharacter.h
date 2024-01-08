// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DroneCharacter.generated.h"

struct FInputActionValue;

UCLASS()
class DRONERACER_YANYIRUNTIME_API ADroneCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADroneCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character")
	TObjectPtr<class UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character")
	TObjectPtr<class UArrowComponent> LeftMuzzle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character")
	TObjectPtr<class UArrowComponent> RightMuzzle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character")
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character")
	TObjectPtr<class UCameraComponent> ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character")
	TObjectPtr<class UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character")
	TObjectPtr<class UDRHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Weapon")
	float MainWeaponCooldownTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Weapon")
	float SecondaryWeaponCooldownTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Weapon")
	float MainWeaponFireRate = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Weapon")
	float SecondaryWeaponFireRate = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character")
	bool ThirdCameraEnabled = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Weapon")
	float MainWeaponWaitingTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Weapon")
	float SecondaryWeaponWaitingTime;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;

	virtual void PostInitializeComponents() override;

protected:
	// 2K TODO Bind to Lyra Ability Input
	UFUNCTION(BlueprintImplementableEvent)
	void OnMainWeaponFire();

	UFUNCTION(BlueprintImplementableEvent)
	void OnSecondaryWeaponFire();

	UFUNCTION(BlueprintCallable)
	void ToggleMovementAndCollision(bool EnableOrNot);

public:
	bool MainWeaponTryOpenFire();

	bool SecondaryWeaponTryOpenFire();

	void SwitchThirdAndFirstCamera();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnMatchStart();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnMatchEnd(bool WinOrLoss);

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ULyraPawnExtensionComponent> PawnExtComponent;

	UFUNCTION()
	void OnAirCraftHitOthers(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	bool CanMainWeaponOpenFire() const { return MainWeaponWaitingTime <= 0.0f; }

	UFUNCTION(BlueprintCallable)
	bool CanSecondaryWeaponOpenFire() const { return SecondaryWeaponWaitingTime <= 0.0f; }
};
