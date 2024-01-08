// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DroneMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"

#include "Net/UnrealNetwork.h"
#include "DRBlueprintFunctionLibrary.h"

UDroneMovementComponent::UDroneMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bOrientRotationToMovement = true;
	MaxFlySpeed = 5000.0f;
	RotationRate = FRotator(360.0f, 360.0f, 0.0f);

	SetNetworkMoveDataContainer(DroneNetworkMoveDataContainer);

	// bReplicates = true;
}

void UDroneMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UpdateCameraEffect();

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// TODO Fake Skeleton Mesh Rotation Based on Roll, Yaw and Pitch
	if (!CharacterOwner->IsLocallyControlled())
	{ 
		UpdateSkeletonMeshPos();
	}
}

FRotator UDroneMovementComponent::ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const
{
	if (LastUpdateVelocity.SizeSquared() > UE_KINDA_SMALL_NUMBER)
	{
		FRotator TargetRotation = LastUpdateVelocity.GetSafeNormal().Rotation();
		// UE_LOG(LogTemp, Warning, TEXT("ComputeOrientToMovementRotation %f %f %f"), TargetRotation.Pitch, TargetRotation.Yaw, TargetRotation.Roll);
		return LastUpdateVelocity.GetSafeNormal().Rotation();
	}

	// Don't change rotation.
	return CurrentRotation;
}

void UDroneMovementComponent::UpdatePlaneControlInput(const EPlaneStatus NewControlInput)
{
	CurrentPlaneStatus = NewControlInput;
}

float UDroneMovementComponent::GetMaxEngineForce() const
{
	return MaxThrottleAmount * ThrottleToForce;
}

bool UDroneMovementComponent::ShouldRemainVertical() const
{
	return IsMovingOnGround();
}

void UDroneMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDroneMovementComponent::HandlePlayerStateReplicated(const FDRPlaneConfig& PlaneConfig)
{
	MaxThrottleAmount = PlaneConfig.MaxThrottleAmount;
	MinThrottleAmount = PlaneConfig.MinThrottleAmount;
	ThrottleSensitivity = PlaneConfig.ThrottleSensitivity;
	PitchSensitivity = PlaneConfig.PitchSensitivity;
	RollSensitivity = PlaneConfig.RollSensitivity;
	MaxRollDegree = PlaneConfig.MaxRollDegree;

	MaxFlySpeed = PlaneConfig.MaxSpeed;
}

void UDroneMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UDroneMovementComponent, RollAmount, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UDroneMovementComponent, PitchAmount, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UDroneMovementComponent, YawAmount, COND_SkipOwner);
}

// TODO: cause the mesh's roll, yaw and pitch is fake in local, we need send message to server and then transfer to all clients
void UDroneMovementComponent::PlaneControlInfo_ClientSend_Implementation(float In_RollAmount, float In_PitchAmount, float In_YawAmount)
{
	RollAmount = In_RollAmount;
	PitchAmount = In_PitchAmount;
	YawAmount = In_YawAmount;
}

void UDroneMovementComponent::UpdateThrottleAmount(float DeltaTime)
{
	if (uint8(CurrentPlaneStatus & EPlaneStatus::ThrottleUP) || AutoDrive)
	{
		ThrottleAmount += ThrottleSensitivity * DeltaTime;
	}
	else
	{
		ThrottleAmount -= ThrottleSensitivity * DeltaTime; //FMath::Lerp(0.0f, ThrottleAmount, 0.95f);
	}
	ThrottleAmount = FMath::Clamp(ThrottleAmount, MinThrottleAmount, MaxThrottleAmount);
}

void UDroneMovementComponent::UpdatePitchAmount(float DeltaTime)
{
	if (!IsFlying() && LastUpdateVelocity.Length() < MinimumTakeOffSpeed)
	{
		return;
	}

	if (uint8(CurrentPlaneStatus & EPlaneStatus::PitchUp) || uint8(CurrentPlaneStatus & EPlaneStatus::PitchDown))
	{
		PitchAmount += PitchSensitivity * DeltaTime * (uint8(CurrentPlaneStatus & EPlaneStatus::PitchUp) ? 1 : -1);
	}
	else
	{
		PitchAmount = FMath::Lerp(0.0f, PitchAmount, 0.99f);
	}

	PitchAmount = FMath::Clamp(PitchAmount, -10.0f, 10.0f);
}

void UDroneMovementComponent::UpdateRollAmount(float DeltaTime)
{
	float RealRollSensitivity = IsFalling() ? RollSensitivity * 10.0f : RollSensitivity;
	if (uint8(CurrentPlaneStatus & EPlaneStatus::RollLeft) || uint8(CurrentPlaneStatus & EPlaneStatus::RollRight) 
	|| uint8(CurrentPlaneStatus & EPlaneStatus::YawLeft) || uint8(CurrentPlaneStatus & EPlaneStatus::YawRight) || IsFalling())
	{
		RollAmount += RealRollSensitivity * DeltaTime * (uint8(CurrentPlaneStatus & EPlaneStatus::RollRight) || uint8(CurrentPlaneStatus & EPlaneStatus::YawRight) ? 1 : -1);
	}
	else
	{
		RollAmount = FMath::Lerp(0.0f, RollAmount, 0.95f);
	}

	if (IsFlying())
	{
		RollAmount = FMath::Clamp(RollAmount, -MaxRollDegree, MaxRollDegree);
	}
}

void UDroneMovementComponent::UpdateYawAmount(float DeltaTime)
{
	float DeltaYaw = 0.0f;
	if (uint8(CurrentPlaneStatus & EPlaneStatus::YawLeft) || uint8(CurrentPlaneStatus & EPlaneStatus::YawRight))
	{
		YawAmount += YawSensitivity * DeltaTime * (uint8(CurrentPlaneStatus & EPlaneStatus::YawRight) ? 1 : -1);
	}
	else
	{
		YawAmount = FMath::Lerp(0.0f, YawAmount, 0.95f);
	}

	YawAmount = FMath::Clamp(YawAmount, -10.0f, 10.0f);
}

void UDroneMovementComponent::CalculateEngineForce()
{
	if (!PawnOwner)
	{
		return;
	}

	// EngineForce = FVector::Zero();
	FVector EngineForwardForce = FVector::Zero();
	FVector EnginePitchForce = FVector::Zero();
	FVector EngineYawForce = FVector::Zero();

	float ForwardSpeed = LastUpdateVelocity.Length();

	if(ForwardSpeed < GetMaxSpeed())
	{ 
		EngineForwardForce = ThrottleAmount * ConvertThrottleToForce() * PawnOwner->GetActorForwardVector();// *(1.0f - ForwardSpeed / GetMaxSpeed());
	}

	if(LastUpdateVelocity.Length() >= MinimumTakeOffSpeed)
	{ 
		EnginePitchForce = PitchAmount * ForwardSpeed * PawnOwner->GetActorUpVector() * 50.0f;
	}

	// Pitch Compensation
	FVector PitchCompensation = LastUpdateVelocity.Z * PawnOwner->GetActorUpVector() * -600.0f;

	EngineYawForce = YawAmount * ForwardSpeed * PawnOwner->GetActorRightVector() * 15.0f;

	EngineForce = EngineForwardForce + EnginePitchForce + EngineYawForce + PitchCompensation;

	// AddForce(EngineForce);

	LastUpdatedEngineForce = EngineForce.Length();

	// 2K TODO
	PlaneControlInfo_ClientSend(RollAmount, PitchAmount, YawAmount);

	UpdateSkeletonMeshPos();
}

float UDroneMovementComponent::ConvertThrottleToForce()
{
	if (IsFlying())
	{
		return ThrottleToForce;
	}

	return 10000.0f;
}

void UDroneMovementComponent::UpdateCameraEffect()
{
	if (!PawnOwner)
	{
		return;
	}

	APlayerController* PlayerControler = Cast<APlayerController>(PawnOwner->GetController());
	if (!PlayerControler)
	{
		return;
	}

	if (CameraShakeClass && uint8(CurrentPlaneStatus & EPlaneStatus::PitchUp) && PlayerControler->IsLocalController())
	{
		PlayerControler->PlayerCameraManager->StartCameraShake(CameraShakeClass, 1.0f, ECameraShakePlaySpace::CameraLocal, FRotator::ZeroRotator);
		// UE_LOG(LogTemp, Warning, TEXT("StartCameraShake"));
	}

	check(CameraFOVCurveAsset)
	{
		float CurveSample = (ThrottleAmount - MinThrottleAmount) / (MaxThrottleAmount - MinThrottleAmount);
		float ExtraFOV = CameraFOVCurveAsset->GetFloatValue(CurveSample) * 30.0f;
		const float TargetFOV = 90.0f + ExtraFOV;
		PlayerControler->PlayerCameraManager->SetFOV(TargetFOV);
	}
}

void UDroneMovementComponent::UpdateSkeletonMeshPos()
{
	if (USkeletalMeshComponent* Mesh = CharacterOwner->GetMesh())
	{
		FRotator CurrentRotator = Mesh->GetRelativeRotation();
		CurrentRotator.Roll = RollAmount;
		CurrentRotator.Pitch = PitchAmount;
		CurrentRotator.Yaw = YawAmount;

		Mesh->SetRelativeRotation(CurrentRotator);
	}
}

void UDroneMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UDroneMovementComponent::SimulatedTick(float DeltaSeconds)
{
	Super::SimulatedTick(DeltaSeconds);

	// UpdateSkeletonMeshPos();
}

void UDroneMovementComponent::ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration)
{
	Super::ReplicateMoveToServer(DeltaTime, NewAcceleration);

	EngineForce = FVector::Zero();
}

void UDroneMovementComponent::ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds)
{
	UpdateThrottleAmount(DeltaSeconds);

	UpdatePitchAmount(DeltaSeconds);

	UpdateYawAmount(DeltaSeconds);

	UpdateRollAmount(DeltaSeconds);

	CalculateEngineForce();

	AddForce(EngineForce);

	Super::ControlledCharacterMove(InputVector, DeltaSeconds);
}

void UDroneMovementComponent::ApplyAccumulatedForces(float DeltaSeconds)
{
	// UE_LOG(LogTemp, Warning, TEXT("%s Apply EngineForce %f %f %f"), *PawnOwner->GetName(), PendingForceToApply.X, PendingForceToApply.Y, PendingForceToApply.Z);
	Super::ApplyAccumulatedForces(DeltaSeconds);
}

void UDroneMovementComponent::ClearAccumulatedForces()
{
	// UE_LOG(LogTemp, Warning, TEXT("%s Clear EngineForce %f %f %f"), *PawnOwner->GetName(), PendingForceToApply.X, PendingForceToApply.Y, PendingForceToApply.Z);
	Super::ClearAccumulatedForces();
}

class FNetworkPredictionData_Client* UDroneMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UDroneMovementComponent* MutableThis = const_cast<UDroneMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Drone(*this);
	}

	return ClientPredictionData;
}

class FNetworkPredictionData_Server* UDroneMovementComponent::GetPredictionData_Server() const
{
	if (ServerPredictionData == nullptr)
	{
		UDroneMovementComponent* MutableThis = const_cast<UDroneMovementComponent*>(this);
		MutableThis->ServerPredictionData = new FNetworkPredictionData_Server_Drone(*this);
	}

	return ServerPredictionData;
}

void UDroneMovementComponent::ServerMove_PerformMovement(const FCharacterNetworkMoveData& MoveData)
{
	const FDroneNetworkMoveData* DroneNetworkMoveData = static_cast<const FDroneNetworkMoveData*>(&MoveData);
	if (DroneNetworkMoveData)
	{
		EngineForce = DroneNetworkMoveData->EngineForce;
		// UE_LOG(LogTemp, Warning, TEXT("%s EngineForce %f %f %f"), *PawnOwner->GetName(), EngineForce.X, EngineForce.Y, EngineForce.Z);
		AddForce(EngineForce);
	}
	
	Super::ServerMove_PerformMovement(MoveData);
}

void FSavedMove_Drone::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	FNetworkPredictionData_Client_Drone* ClientDroneData = static_cast<FNetworkPredictionData_Client_Drone*>(&ClientData);

	if (ClientDroneData)
	{
		UDroneMovementComponent* DroneMovementComponent = Cast<UDroneMovementComponent>(Character->GetCharacterMovement());
		ClientDroneData->EngineForce = DroneMovementComponent->GetEngineForceVector();
		EngineForce = DroneMovementComponent->GetEngineForceVector();
	}

	// UE_LOG(LogTemp, Warning, TEXT("%s SetMoveFor %f %f %f"), *PawnOwner->GetName(), EngineForce.X, EngineForce.Y, EngineForce.Z);
}

void FSavedMove_Drone::Clear()
{
	FSavedMove_Character::Clear();

	EngineForce = FVector::ZeroVector;
}

bool FSavedMove_Drone::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	bool CanCombine = FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
	FSavedMove_Drone* NewDroneMove = static_cast<FSavedMove_Drone*>(NewMove.Get());

	if (!CanCombine)
	{
		return CanCombine;
	}

	if (NewDroneMove->EngineForce.IsZero())
	{
		if (!EngineForce.IsZero())
		{
			return false;
		}
	}

	return true;
}

FNetworkPredictionData_Client_Drone::FNetworkPredictionData_Client_Drone(const UCharacterMovementComponent& ClientMovement)
	: FNetworkPredictionData_Client_Character(ClientMovement)
{
	const UDroneMovementComponent* DroneMovementComponent = static_cast<const UDroneMovementComponent*>(&ClientMovement);

	if (DroneMovementComponent)
	{
		EngineForce = DroneMovementComponent->GetEngineForceVector();

		// UE_LOG(LogTemp, Warning, TEXT("FNetworkPredictionData_Client_Drone %f %f %f"), EngineForce.X, EngineForce.Y, EngineForce.Z);
	}
}

FSavedMovePtr FNetworkPredictionData_Client_Drone::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Drone());
}

FNetworkPredictionData_Server_Drone::FNetworkPredictionData_Server_Drone(const UCharacterMovementComponent& ServerMovement)
	: FNetworkPredictionData_Server_Character(ServerMovement)
{
	const UDroneMovementComponent* DroneMovementComponent = static_cast<const UDroneMovementComponent*>(&ServerMovement);

	if (DroneMovementComponent)
	{
		EngineForce = DroneMovementComponent->GetEngineForceVector();
	}
}
