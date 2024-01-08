// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraCharacter.h"
#include "DRCharacter.generated.h"

/**
 * ADRCharacter: Author Yanyi_2K
 * Actually this is an extend version of LyraCharacter
 * But there exists some different with the LyraCharacter so I do not want to subclass the LyraCharacter
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by DroneRacer Game Feature."))
class DRONERACER_YANYIRUNTIME_API ADRCharacter : public AModularCharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface, public ILyraTeamAgentInterface
{
	GENERATED_BODY()
	
public:

	ADRCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Character")
	class ULyraAbilitySystemComponent* GetLyraAbilitySystemComponent() const;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;

	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	//~AActor interface
	virtual void PreInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Reset() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	//~End of AActor interface

	//~APawn interface
	virtual void NotifyControllerChanged() override;
	//~End of APawn interface

	//~ILyraTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnLyraTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of ILyraTeamAgentInterface interface

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ULyraPawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDroneHeroComponent> HeroComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ULyraHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UArrowComponent> LeftMuzzle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UArrowComponent> RightMuzzle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> ThirdPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DroneRacer|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Weapon", Meta = (AllowPrivateAccess = "true"))
	float MainWeaponWaitingTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Weapon", Meta = (AllowPrivateAccess = "true"))
	float SecondaryWeaponWaitingTime;

	bool ThirdCameraEnabled = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Weapon", Meta = (AllowPrivateAccess = "true"))
	FGenericTeamId TeamID;

	UPROPERTY()
	FOnLyraTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	class UDRWidgetManagerComponent* WidgetManagerComponent;

	UPROPERTY()
	class UDRUserWidget_InGameHUD* InGameHUD;

protected:

	virtual void OnAbilitySystemInitialized();

	virtual void OnAbilitySystemUninitialized();

	virtual void Restart() override;

	virtual void PawnClientRestart() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	virtual void OnRep_Controller() override;

	virtual void OnRep_PlayerState() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitializeGameplayTags();

	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	void DestroyDueToDeath();

	void UninitAndDestroy();

	UFUNCTION(BlueprintCallable)
	void ToggleMovementAndCollision(bool EnableOrNot);

public:

	UFUNCTION(BlueprintCallable)
	bool CanMainWeaponOpenFire() const { return MainWeaponWaitingTime <= 0.0f; }

	UFUNCTION(BlueprintCallable)
	bool CanSecondaryWeaponOpenFire() const { return SecondaryWeaponWaitingTime <= 0.0f; }

	UFUNCTION(BlueprintCallable)
	void SwitchThirdAndFirstCamera();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void OnMatchStart(const FGenericTeamId& NewTeamID);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void OnMatchEnd(bool WinOrLoss);

private:
	
	void HandleControllerChanged(class AController* NewController);
};
