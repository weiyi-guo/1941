// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraHeroComponent.h"
#include "DroneMovementComponent.h"
#include "DroneHeroComponent.generated.h"

class UGameFrameworkComponentManager;

DECLARE_LOG_CATEGORY_EXTERN(LogDrone, Log, All);

/**
 * 
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class DRONERACER_YANYIRUNTIME_API UDroneHeroComponent : public ULyraHeroComponent
{
	GENERATED_BODY()

public:

	UDroneHeroComponent(const FObjectInitializer& ObjectInitializer);

	/** Adds mode-specific input config */
	virtual void AddAdditionalInputConfig(const ULyraInputConfig* InputConfig);

	/** Removes a mode-specific input config if it has been added */
	virtual void RemoveAdditionalInputConfig(const ULyraInputConfig* InputConfig);

	/** True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added */
	virtual bool IsReadyToBindInputs() const;

	/** The name of the extension event sent via UGameFrameworkComponentManager when ability inputs are ready to bind */
	static const FName NAME_BindInputsNow;

	/** The name of this component-implemented feature */
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

protected:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	void Input_ThrottleUp(const FInputActionValue& InputActionValue);
	void Input_Roll(const FInputActionValue& InputActionValue);
	void Input_Yaw(const FInputActionValue& InputActionValue);
	void Input_Pitch(const FInputActionValue& InputActionValue);

	void Input_ThrottleUp_Completed(const FInputActionValue& InputActionValue);
	void Input_Roll_Completed(const FInputActionValue& InputActionValue);
	void Input_Yaw_Completed(const FInputActionValue& InputActionValue);
	void Input_Pitch_Completed(const FInputActionValue& InputActionValue);

	void Input_MainWeapon(const FInputActionValue& InputActionValue);
	void Input_SecondaryWeapon(const FInputActionValue& InputActionValue);

	void Input_SwitchCamera(const FInputActionValue& InputActionValue);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

protected:
	// Current Status
	UPROPERTY(BlueprintReadWrite, Category = "Drone")
	EPlaneStatus CurrentPlaneStatus;

private:

	UPROPERTY()
	TObjectPtr<class UDroneMovementComponent> DroneMovementComponent;

	void UpdateMovementComponentInput();
};
