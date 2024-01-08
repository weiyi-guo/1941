// Fill out your copyright notice in the Description page of Project Settings.


#include "DRCharacter.h"
#include "Components/DroneHeroComponent.h"
#include "Components/DroneMovementComponent.h"

// LyraGame
#include "Character/LyraPawnExtensionComponent.h"
#include "Character/LyraHealthComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"

#include <Components/ArrowComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Perception/AIPerceptionStimuliSourceComponent.h>
#include <Components/CapsuleComponent.h>
#include "Components/DRWidgetManagerComponent.h"
#include "DRPlayerState.h"

#include "UI/DRUIMessageDefinition.h"
#include "UI/DRUserWidget_InGameHUD.h"

#include "JsonObjectConverter.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "Attributes/DRCombatSet.h"

#include "DRBlueprintFunctionLibrary.h"
#include <GameFramework/GameStateBase.h>

ADRCharacter::ADRCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDroneMovementComponent>(ACharacter::CharacterMovementComponentName)),
	TeamID(0)
{
	PrimaryActorTick.bCanEverTick = true;

	AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSource"));

	LeftMuzzle = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftMuzzle"));
	LeftMuzzle->SetupAttachment(GetMesh());
	LeftMuzzle->ComponentTags.Add(FName("Muzzle"));

	RightMuzzle = CreateDefaultSubobject<UArrowComponent>(TEXT("RightMuzzle"));
	RightMuzzle->SetupAttachment(GetMesh());
	RightMuzzle->ComponentTags.Add(FName("Muzzle"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArm);
	ThirdPersonCamera->SetAutoActivate(ThirdCameraEnabled);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh());
	FirstPersonCamera->SetAutoActivate(!ThirdCameraEnabled);

	PawnExtComponent = CreateDefaultSubobject<ULyraPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	HealthComponent = CreateDefaultSubobject<ULyraHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	HeroComponent = CreateDefaultSubobject<UDroneHeroComponent>(TEXT("HeroComponent"));

	// always face to the direction of movement
	bUseControllerRotationYaw = false;

	Tags.Add(FName("Player"));

	bReplicates = true;
	bAlwaysRelevant = true;
}

void ADRCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ADRCharacter::BeginPlay()
{
	Super::BeginPlay();

	ToggleMovementAndCollision(false);
}

void ADRCharacter::Restart()
{
	Super::Restart();
}

void ADRCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
}

void ADRCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UDroneMovementComponent* DroneMovementComponent = CastChecked<UDroneMovementComponent>(GetCharacterMovement());
	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
	if(InGameHUD && LyraASC)
	{
		float CurrentHealth = LyraASC->GetNumericAttribute(ULyraHealthSet::GetHealthAttribute());
		int32 RocketNum = LyraASC->GetNumericAttribute(UDRCombatSet::GetRocketNumAttribute());
		InGameHUD->UpdateInGameHUD(CurrentHealth, GetActorLocation().Z, DroneMovementComponent->GetLastUpdateVelocity().Length(), DroneMovementComponent->GetThrottleAmount(), DroneMovementComponent->GetEngineForce(), RocketNum);
	}
}

void ADRCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ADRCharacter::Reset()
{

}

void ADRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ADRCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
}

void ADRCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}

void ADRCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ADRCharacter::GetGenericTeamId() const
{
	return TeamID;
}

FOnLyraTeamIndexChangedDelegate* ADRCharacter::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void ADRCharacter::OnAbilitySystemInitialized()
{
	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
	check(LyraASC);

	// 2K TODO
	LyraASC->SetNumericAttributeBase(ULyraHealthSet::GetMaxHealthAttribute(), 1000.0f);
	LyraASC->SetNumericAttributeBase(UDRCombatSet::GetRocketNumAttribute(), 5);
	// 2K TODO

	HealthComponent->InitializeWithAbilitySystem(LyraASC);

	InitializeGameplayTags();
}

void ADRCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void ADRCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();

	HandleControllerChanged(NewController);
}

void ADRCharacter::UnPossessed()
{
	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();

	HandleControllerChanged(nullptr);
}

void ADRCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();

	HandleControllerChanged(Controller);
}

void ADRCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();
	UDroneMovementComponent* DroneMovementComponent = CastChecked<UDroneMovementComponent>(GetCharacterMovement());
	ADRPlayerState* DRPlayerState = GetPlayerState<ADRPlayerState>();
	FDRPlaneConfig PlaneConfig;
	if(DRPlayerState && DRPlayerState->GetSelectedPlaneConfig(PlaneConfig))
	{ 
		DroneMovementComponent->HandlePlayerStateReplicated(PlaneConfig);

		//FString PlayerName = DRPlayerState->GetPlayerName();
		//UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerState Player %s"), *PlayerName);
	}
}

void ADRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void ADRCharacter::InitializeGameplayTags()
{

}

void ADRCharacter::OnDeathStarted(AActor* OwningActor)
{
	ToggleMovementAndCollision(false);
}

void ADRCharacter::OnDeathFinished(AActor* OwningActor)
{

}

void ADRCharacter::DestroyDueToDeath()
{
	UninitAndDestroy();
}

void ADRCharacter::UninitAndDestroy()
{
	if (ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent())
	{
		if (LyraASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void ADRCharacter::ToggleMovementAndCollision(bool EnableOrNot)
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(EnableOrNot ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(EnableOrNot ? ECR_Block : ECR_Ignore);

	UDroneMovementComponent* DroneMovementComponent = CastChecked<UDroneMovementComponent>(GetCharacterMovement());

	if (!EnableOrNot)
	{
		DroneMovementComponent->StopMovementImmediately();
		DroneMovementComponent->DisableMovement();
	}

	DroneMovementComponent->SetActive(EnableOrNot);

	if (Controller)
	{
		Controller->SetIgnoreMoveInput(!EnableOrNot);
	}

	SetActorHiddenInGame(!EnableOrNot);
}

void ADRCharacter::SwitchThirdAndFirstCamera()
{
	ThirdCameraEnabled = !ThirdCameraEnabled;
	ThirdPersonCamera->SetActive(ThirdCameraEnabled);
	FirstPersonCamera->SetActive(!ThirdCameraEnabled);
}

void ADRCharacter::OnMatchStart_Implementation(const FGenericTeamId& NewTeamID)
{
	SetGenericTeamId(NewTeamID);

	if (WidgetManagerComponent)
	{
		WidgetManagerComponent->RequestHideWidget(FName("WBP_Waiting"));
		InGameHUD = Cast<UDRUserWidget_InGameHUD>(WidgetManagerComponent->RequestShowWidget("WBP_InGameHUD"));

		ADRPlayerState* DRPlayerState = GetPlayerState<ADRPlayerState>();
		if(InGameHUD && DRPlayerState)
		{ 
			InGameHUD->UpdatePlayerInfo(DRPlayerState->GetPlayerName(), TeamID.GetId());
		}
	}

	ToggleMovementAndCollision(true);
}

void ADRCharacter::OnMatchEnd_Implementation(bool WinOrLoss)
{
	if (WidgetManagerComponent)
	{
		WidgetManagerComponent->RequestHideWidget(FName("WBP_InGameHUD"));
		InGameHUD = nullptr;

		WidgetManagerComponent->RequestShowWidget(FName("WBP_GameOver"));

		if (AGameStateBase* DRGameState = GetWorld() ? GetWorld()->GetGameState() : nullptr)
		{
			if (DRGameState->PlayerArray.Num() >= 2)
			{
				APlayerState* Player1 = DRGameState->PlayerArray[0];
				APlayerState* Player2 = DRGameState->PlayerArray[1];
				FDRBattleResult BattleResult(WinOrLoss, Player1->GetPlayerName(), Player2->GetPlayerName(), Player1->GetScore(), Player2->GetScore());

				FString MessagePayload;
				if (FJsonObjectConverter::UStructToJsonObjectString(BattleResult, MessagePayload))
				{
					WidgetManagerComponent->RequestUpdateWidget("WBP_GameOver", MessagePayload);
				}
			}
		}
	}

	ToggleMovementAndCollision(false);
}

void ADRCharacter::HandleControllerChanged(class AController* NewController)
{
	if (WidgetManagerComponent)
	{
		WidgetManagerComponent->RequestHideAllWidgets();
		WidgetManagerComponent = nullptr;
	}

	if (!NewController || !NewController->IsLocalController())
	{
		return;
	}

	WidgetManagerComponent = UDRWidgetManagerComponent::GetComponent(NewController);

	if (WidgetManagerComponent)
	{
		WidgetManagerComponent->RequestShowWidget("WBP_InGameReady");
	}

	if (!WidgetManagerComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("WidgetManagerComponent is Empty in Client!!!"));
	}
}

ULyraAbilitySystemComponent* ADRCharacter::GetLyraAbilitySystemComponent() const
{
	return Cast<ULyraAbilitySystemComponent>(GetAbilitySystemComponent());
}


UAbilitySystemComponent* ADRCharacter::GetAbilitySystemComponent() const
{
	return PawnExtComponent->GetLyraAbilitySystemComponent();
}

void ADRCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent())
	{
		LyraASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ADRCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent())
	{
		return LyraASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ADRCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent())
	{
		return LyraASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ADRCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent())
	{
		return LyraASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}