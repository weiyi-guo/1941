// Fill out your copyright notice in the Description page of Project Settings.


#include "DREnemyBase.h"

#include "Components/SphereComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Components/DRHealthComponent.h"

#include "DamageCauserInterface.h"
#include "DroneRacerGameMode.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "Character/LyraHealthComponent.h"

// Sets default values
ADREnemyBase::ADREnemyBase()
	: TeamID(1)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// HealthComponent = CreateDefaultSubobject<UDRHealthComponent>(TEXT("HealthComponent"));
	LyraHealthComponent = CreateDefaultSubobject<ULyraHealthComponent>(TEXT("LyraHealthComponent"));

	AbilitySystemComponent = CreateDefaultSubobject<ULyraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	HealthSet = CreateDefaultSubobject<ULyraHealthSet>(TEXT("HealthSet"));

	bReplicates = true;
}

void ADREnemyBase::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ADREnemyBase::GetGenericTeamId() const
{
	return TeamID;
}

FOnLyraTeamIndexChangedDelegate* ADREnemyBase::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

// Called when the game starts or when spawned
void ADREnemyBase::BeginPlay()
{
	Super::BeginPlay();

	RegisterEnemy();
	
	InitializeEnemy();

	//if (DamageAbility)
	//{
	//	FGameplayAbilitySpec AbilitySpec(DamageAbility);
	//	AbilitySystemComponent->GiveAbility(AbilitySpec);
	//}

	if(DeathAbilityClass && HasAuthority())
	{ 
		ULyraGameplayAbility* AbilityCDO = DeathAbilityClass->GetDefaultObject<ULyraGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, 1);
		AbilitySpec.SourceObject = nullptr;

		const FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
}

void ADREnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereComponentBeginOverlap);

	// HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);

	LyraHealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
	LyraHealthComponent->OnHealthChanged.AddDynamic(this, &ThisClass::OnHealthChanged);
}

void ADREnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void ADREnemyBase::OnExploded()
{
	if (ExplosionEffect)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(ExplosionEffect, SphereComponent, NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);
		NiagaraComp->OnSystemFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
	}
	else
	{
		OnDeathFinished(nullptr);
	}
}

void ADREnemyBase::InitializeEnemy()
{

}

void ADREnemyBase::RegisterEnemy()
{
	ADroneRacerGameMode* DroneRacerGameMode = GetWorld()->GetAuthGameMode<ADroneRacerGameMode>();
	if (DroneRacerGameMode)
	{
		DroneRacerGameMode->OnRegisterEnemy(GetName(), this);
	}
}

void ADREnemyBase::OnSphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UDamageCauserInterface::StaticClass()) && OtherActor->GetInstigator() != this)
	{
		IDamageCauserInterface* DamageCauser = Cast<IDamageCauserInterface>(OtherActor);

		float DamageValue = DamageCauser->Execute_GetDamageAmount(OtherActor);
		// HealthComponent->ApplyDamage(OtherActor->GetInstigator(), DamageValue);
	}
}

void ADREnemyBase::OnActorPerceptionUpdated_Implementation(AActor* Actor, bool WasSuccessfullySensed)
{
	if (WasSuccessfullySensed)
	{
		TargetActors.AddUnique(Actor);

		// UE_LOG(LogTemp, Warning, TEXT("Actor %s AI Perception Captured %s"), *GetName(), *Actor->GetName());
	}
	else
	{
		TargetActors.Remove(Actor);

		// UE_LOG(LogTemp, Warning, TEXT("Actor %s AI Perception Lost %s"), *GetName(), *Actor->GetName());
	}
}

void ADREnemyBase::TraceTargetActors_Implementation()
{

}

void ADREnemyBase::OnHealthChanged(class ULyraHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* InstigatorActor)
{
	if (OldValue > 0.0f && NewValue <= 0.0f)
	{
		//if (HasAuthority())
		//{
		//	ADroneRacerGameMode* DroneRacerGameMode = GetWorld()->GetAuthGameMode<ADroneRacerGameMode>();
		//	DroneRacerGameMode->OnEliminateEnemy(InstigatorActor, this);
		//}

		OnDeathStarted(InstigatorActor);
	}
}

void ADREnemyBase::OnDeathStarted(AActor* Actor)
{
	FString DamageCausorName = Actor ? Actor->GetName() : "UNKNOW";
	UE_LOG(LogTemp, Warning, TEXT("Enemy %s receive damage from %s and start death."), *GetName(), *DamageCausorName);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	OnExploded();
}

void ADREnemyBase::OnDeathFinished(class UNiagaraComponent* NiagaraComponent)
{
	Destroy();
}

