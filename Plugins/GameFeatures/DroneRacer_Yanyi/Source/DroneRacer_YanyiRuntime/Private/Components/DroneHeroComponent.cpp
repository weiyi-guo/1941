// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DroneHeroComponent.h"
#include "LyraGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"

#include "EnhancedInputSubsystems.h"

#include "Player/LyraPlayerState.h"
#include "Player/LyraLocalPlayer.h"
#include "Player/LyraPlayerController.h"

#include "Character/LyraPawnExtensionComponent.h"
#include "Character/LyraPawnData.h"

#include "Input/LyraInputComponent.h"
#include "DroneCharacter.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "DRCharacter.h"


DEFINE_LOG_CATEGORY(LogDrone);

const FName UDroneHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UDroneHeroComponent::NAME_ActorFeatureName("DroneHero");

UDroneHeroComponent::UDroneHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UDroneHeroComponent::OnRegister()
{
	Super::OnRegister();
}

bool UDroneHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	return Super::CanChangeInitState(Manager, CurrentState, DesiredState);
}

void UDroneHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	// UE_LOG(LogDrone, Warning, TEXT("UDroneHeroComponent::HandleChangeInitState"));
	
	if (CurrentState == LyraGameplayTags::InitState_DataAvailable && DesiredState == LyraGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ALyraPlayerState* LyraPS = GetPlayerState<ALyraPlayerState>();
		if (!ensure(Pawn && LyraPS))
		{
			return;
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const ULyraPawnData* PawnData = nullptr;

		if (ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<ULyraPawnData>();

			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			// 

			PawnExtComp->InitializeAbilitySystem(LyraPS->GetLyraAbilitySystemComponent(), LyraPS);
		}

		if (ALyraPlayerController* LyraPC = GetController<ALyraPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// 2K TODO
		//if (bIsLocallyControlled && PawnData)
		//{
		//	if (ULyraCameraComponent* CameraComponent = ULyraCameraComponent::FindCameraComponent(Pawn))
		//	{
		//		CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		//	}
		//}
	}
}

void UDroneHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	Super::OnActorInitStateChanged(Params);
}

void UDroneHeroComponent::CheckDefaultInitialization()
{
	Super::CheckDefaultInitialization();
}

void UDroneHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// 2K TODO: safety check
	const APawn* Pawn = GetPawn<APawn>();
	DroneMovementComponent = UDroneMovementComponent::FindDroneMovementComponent(Pawn);
}

void UDroneHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UDroneHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULyraLocalPlayer* LP = Cast<ULyraLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const ULyraPawnData* PawnData = PawnExtComp->GetPawnData<ULyraPawnData>())
		{
			if (const ULyraInputConfig* InputConfig = PawnData->InputConfig)
			{

				ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(PlayerInputComponent);
				if (ensureMsgf(LyraIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to ULyraInputComponent or a subclass of it.")))
				{
					// Add the key mappings that may have been set by the player
					LyraIC->AddInputMappings(InputConfig, Subsystem);

					TArray<uint32> BindHandles;
					LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

					// Hard code all the Input Gameplay Tag
					const FGameplayTag InputTag_ThrottleUp = FGameplayTag::RequestGameplayTag(FName("InputTag.DroneRacer.ThrottleUp"));
					const FGameplayTag InputTag_Roll = FGameplayTag::RequestGameplayTag(FName("InputTag.DroneRacer.Roll"));
					const FGameplayTag InputTag_Yaw = FGameplayTag::RequestGameplayTag(FName("InputTag.DroneRacer.Yaw"));
					const FGameplayTag InputTag_Pitch = FGameplayTag::RequestGameplayTag(FName("InputTag.DroneRacer.Pitch"));
					const FGameplayTag InputTag_MainWeapon = FGameplayTag::RequestGameplayTag(FName("InputTag.DroneRacer.MainWeapon"));
					const FGameplayTag InputTag_SecondaryWeapon = FGameplayTag::RequestGameplayTag(FName("InputTag.DroneRacer.SecondaryWeapon"));
					const FGameplayTag InputTag_SwitchCamera = FGameplayTag::RequestGameplayTag(FName("InputTag.DroneRacer.SwitchCamera"));

					LyraIC->BindNativeAction(InputConfig, InputTag_ThrottleUp, ETriggerEvent::Triggered, this, &ThisClass::Input_ThrottleUp, false);
					LyraIC->BindNativeAction(InputConfig, InputTag_Roll, ETriggerEvent::Triggered, this, &ThisClass::Input_Roll, false);
					LyraIC->BindNativeAction(InputConfig, InputTag_Yaw, ETriggerEvent::Triggered, this, &ThisClass::Input_Yaw, false);
					LyraIC->BindNativeAction(InputConfig, InputTag_Pitch, ETriggerEvent::Triggered, this, &ThisClass::Input_Pitch, false);

					LyraIC->BindNativeAction(InputConfig, InputTag_ThrottleUp, ETriggerEvent::Completed, this, &ThisClass::Input_ThrottleUp_Completed, false);
					LyraIC->BindNativeAction(InputConfig, InputTag_Roll, ETriggerEvent::Completed, this, &ThisClass::Input_Roll_Completed, false);
					LyraIC->BindNativeAction(InputConfig, InputTag_Yaw, ETriggerEvent::Completed, this, &ThisClass::Input_Yaw_Completed, false);
					LyraIC->BindNativeAction(InputConfig, InputTag_Pitch, ETriggerEvent::Completed, this, &ThisClass::Input_Pitch_Completed, false);

					// 2K Bind to Lyra Ability mSystem
					LyraIC->BindNativeAction(InputConfig, InputTag_MainWeapon, ETriggerEvent::Triggered, this, &ThisClass::Input_MainWeapon, false);
					LyraIC->BindNativeAction(InputConfig, InputTag_SecondaryWeapon, ETriggerEvent::Started, this, &ThisClass::Input_SecondaryWeapon, false);
					LyraIC->BindNativeAction(InputConfig, InputTag_SwitchCamera, ETriggerEvent::Started, this, &ThisClass::Input_SwitchCamera, false);
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UDroneHeroComponent::AddAdditionalInputConfig(const ULyraInputConfig* InputConfig)
{
	// 2K Do nothing now
}

void UDroneHeroComponent::RemoveAdditionalInputConfig(const ULyraInputConfig* InputConfig)
{
	// 2K Do nothing now
}

bool UDroneHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UDroneHeroComponent::Input_ThrottleUp(const FInputActionValue& InputActionValue)
{
	// UE_LOG(LogTemp, Warning, TEXT("DroneHeroComponent Input_ThrottleUp Pressed"));
	CurrentPlaneStatus |= EPlaneStatus::ThrottleUP;
	CurrentPlaneStatus &= ~EPlaneStatus::ThrottleDown;

	UpdateMovementComponentInput();
}

void UDroneHeroComponent::Input_Roll(const FInputActionValue& InputActionValue)
{
	float ActionValue = InputActionValue.Get<float>();
	if (ActionValue < 0.0f)
	{
		CurrentPlaneStatus |= EPlaneStatus::RollLeft;
		CurrentPlaneStatus &= ~EPlaneStatus::RollRight;
	}
	else
	{
		CurrentPlaneStatus |= EPlaneStatus::RollRight;
		CurrentPlaneStatus &= ~EPlaneStatus::RollLeft;
	}

	UpdateMovementComponentInput();
}

void UDroneHeroComponent::Input_Yaw(const FInputActionValue& InputActionValue)
{
	float ActionValue = InputActionValue.Get<float>();
	if (ActionValue < 0.0f)
	{
		CurrentPlaneStatus |= EPlaneStatus::YawLeft;
		CurrentPlaneStatus &= ~EPlaneStatus::YawRight;
	}
	else
	{
		CurrentPlaneStatus |= EPlaneStatus::YawRight;
		CurrentPlaneStatus &= ~EPlaneStatus::YawLeft;
	}

	UpdateMovementComponentInput();
}

void UDroneHeroComponent::Input_Pitch(const FInputActionValue& InputActionValue)
{
	float ActionValue = InputActionValue.Get<float>();
	if (ActionValue > 0.0f)
	{
		CurrentPlaneStatus |= EPlaneStatus::PitchUp;
		CurrentPlaneStatus &= ~EPlaneStatus::PitchDown;
	}
	else
	{
		CurrentPlaneStatus |= EPlaneStatus::PitchDown;
		CurrentPlaneStatus &= ~EPlaneStatus::PitchUp;
	}

	UpdateMovementComponentInput();
}

void UDroneHeroComponent::Input_ThrottleUp_Completed(const FInputActionValue& InputActionValue)
{
	CurrentPlaneStatus &= ~EPlaneStatus::ThrottleUP;
	CurrentPlaneStatus &= ~EPlaneStatus::ThrottleDown;
	UpdateMovementComponentInput();
}

void UDroneHeroComponent::Input_Roll_Completed(const FInputActionValue& InputActionValue)
{
	CurrentPlaneStatus &= ~EPlaneStatus::RollRight;
	CurrentPlaneStatus &= ~EPlaneStatus::RollLeft;
	UpdateMovementComponentInput();
}

void UDroneHeroComponent::Input_Yaw_Completed(const FInputActionValue& InputActionValue)
{
	CurrentPlaneStatus &= ~EPlaneStatus::YawLeft;
	CurrentPlaneStatus &= ~EPlaneStatus::YawRight;
	UpdateMovementComponentInput();
}

void UDroneHeroComponent::Input_Pitch_Completed(const FInputActionValue& InputActionValue)
{
	CurrentPlaneStatus &= ~EPlaneStatus::PitchUp;
	CurrentPlaneStatus &= ~EPlaneStatus::PitchDown;
	UpdateMovementComponentInput();
}

void UDroneHeroComponent::Input_MainWeapon(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (ADroneCharacter* DroneCharacter = Cast<ADroneCharacter>(Pawn))
	{
		DroneCharacter->MainWeaponTryOpenFire();
	}
}

void UDroneHeroComponent::Input_SecondaryWeapon(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (ADroneCharacter* DroneCharacter = Cast<ADroneCharacter>(Pawn))
	{
		DroneCharacter->SecondaryWeaponTryOpenFire();
	}
}

void UDroneHeroComponent::Input_SwitchCamera(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (ADRCharacter* DroneCharacter = Cast<ADRCharacter>(Pawn))
	{
		DroneCharacter->SwitchThirdAndFirstCamera();
	}
}

void UDroneHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	// UE_LOG(LogTemp, Warning, TEXT("DroneHeroComponent Input_AbilityInputTag %s Pressed"), *InputTag.ToString());

	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (ULyraAbilitySystemComponent* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent())
			{
				LyraASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UDroneHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	// UE_LOG(LogTemp, Warning, TEXT("DroneHeroComponent Input_AbilityInputTag %s Released"), *InputTag.ToString());

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (ULyraAbilitySystemComponent* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent())
		{
			LyraASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UDroneHeroComponent::UpdateMovementComponentInput()
{
	if (DroneMovementComponent)
	{
		DroneMovementComponent->UpdatePlaneControlInput(CurrentPlaneStatus);
	}
}
