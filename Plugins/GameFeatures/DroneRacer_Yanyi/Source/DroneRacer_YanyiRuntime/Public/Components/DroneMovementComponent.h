// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DataAssets/DRPlaneSet.h"
#include "Network/DroneMovementReplication.h"
#include "DroneMovementComponent.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlaneStatus : uint8
{
	None			= (0x0)		UMETA(Hidden),
	ThrottleUP		= (1 << 0),
	ThrottleDown	= (1 << 1),
	PitchUp			= (1 << 2),
	PitchDown		= (1 << 3),
	RollLeft		= (1 << 4),
	RollRight		= (1 << 5),
	YawLeft			= (1 << 6),
	YawRight		= (1 << 7)

};
ENUM_CLASS_FLAGS(EPlaneStatus)

class DRONERACER_YANYIRUNTIME_API FSavedMove_Drone : public FSavedMove_Character
{

public:
	FVector EngineForce;

	virtual ~FSavedMove_Drone() {}

	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;

	virtual void Clear() override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
};

class DRONERACER_YANYIRUNTIME_API FNetworkPredictionData_Client_Drone : public FNetworkPredictionData_Client_Character
{
public:
	FVector EngineForce;

	FNetworkPredictionData_Client_Drone(const UCharacterMovementComponent& ClientMovement);
	virtual FSavedMovePtr AllocateNewMove() override;

	virtual ~FNetworkPredictionData_Client_Drone(){}
};

class DRONERACER_YANYIRUNTIME_API FNetworkPredictionData_Server_Drone : public FNetworkPredictionData_Server_Character
{
public:
	FVector EngineForce;

	FNetworkPredictionData_Server_Drone(const UCharacterMovementComponent& ServerMovement);
	virtual ~FNetworkPredictionData_Server_Drone(){}
};

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API UDroneMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:

	UDroneMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "DroneRacer|MovementComponent")
	static UDroneMovementComponent* FindDroneMovementComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UDroneMovementComponent>() : nullptr); }

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual FRotator ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const;

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|MovementComponent")
	void UpdatePlaneControlInput(const EPlaneStatus NewControlInput);

	UPROPERTY(BlueprintReadOnly, Category = "DroneRacer|Engine")
	float LastUpdatedEngineForce;

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Engine")
	float GetThrottleAmount() const { return ThrottleAmount; }

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Engine")
	float GetPitchAmount() const { return PitchAmount; }

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Engine")
	float GetRollAmount() const { return RollAmount; }

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Engine")
	float GetYawAmount() const { return YawAmount; }

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Engine")
	float GetEngineForce() const { return LastUpdatedEngineForce; }

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Engine")
	FVector GetEngineForceVector() const { return EngineForce; }

	UFUNCTION(BlueprintCallable, Category = "DroneRacer|Engine")
	float GetMaxEngineForce() const;

	virtual bool ShouldRemainVertical() const;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void HandlePlayerStateReplicated(const FDRPlaneConfig& PlaneConfig);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	// Configuration
	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer|Throttle")
	bool AutoDrive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Throttle")
	float MaxThrottleAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Throttle")
	float MinThrottleAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Throttle")
	float ThrottleToForce;

	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer|Throttle")
	float ThrottleSensitivity;

	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer|Pitch")
	float PitchSensitivity;

	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer|Roll")
	float RollSensitivity;
	
	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer|Roll")
	float MaxRollDegree;

	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer|Yaw")
	float YawSensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DroneRacer|Engine")
	float MinimumTakeOffSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer|Camera")
	TSubclassOf<class UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditAnywhere, Category = "Balance")
	UCurveFloat* CameraFOVCurveAsset;

	// Current Status
	UPROPERTY(BlueprintReadWrite, Category = "Drone")
	EPlaneStatus CurrentPlaneStatus;

	UPROPERTY(BlueprintReadWrite, Category = "DroneRacer|Throttle")
	float ThrottleAmount;

	UPROPERTY(BlueprintReadWrite, Category = "DroneRacer|Pitch", Replicated)
	float PitchAmount;

	UPROPERTY(BlueprintReadWrite, Category = "DroneRacer|Roll", Replicated)
	float RollAmount;

	UPROPERTY(BlueprintReadWrite, Category = "DroneRacer|Yaw", Replicated)
	float YawAmount;

	UPROPERTY(BlueprintReadWrite, Category = "DroneRacer|Engine")
	FVector EngineForce = FVector::Zero();

	UFUNCTION(Server, Unreliable)
	void PlaneControlInfo_ClientSend(float In_RollAmount, float In_PitchAmount, float In_YawAmount);

private:

	void UpdateThrottleAmount(float DeltaTime);

	void UpdatePitchAmount(float DeltaTime);

	void UpdateRollAmount(float DeltaTime);

	void UpdateYawAmount(float DeltaTime);

	void CalculateEngineForce();

	float ConvertThrottleToForce();

	void UpdateCameraEffect();

	void UpdateSkeletonMeshPos();

protected:

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void SimulatedTick(float DeltaSeconds) override;

public:

	virtual void ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration);

	virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds) override;

	virtual void ApplyAccumulatedForces(float DeltaSeconds) override;

	virtual void ClearAccumulatedForces() override;

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual class FNetworkPredictionData_Server* GetPredictionData_Server() const override;

	virtual void ServerMove_PerformMovement(const FCharacterNetworkMoveData& MoveData) override;

private:
	FDroneNetworkMoveDataContainer DroneNetworkMoveDataContainer;
};
