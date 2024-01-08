// Fill out your copyright notice in the Description page of Project Settings.


#include "DRProjectileActorBase.h"
#include <Components/CapsuleComponent.h>
#include "NiagaraComponent.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "Components/DRHealthComponent.h"
#include "Subsystems/DRObjectPoolSubsystem.h"

#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

ADRProjectileActorBase::ADRProjectileActorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CapsuleComponent->SetNotifyRigidBodyCollision(true);
	SetRootComponent(CapsuleComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetupAttachment(RootComponent);

	ExplosionEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionEffect"));
	ExplosionEffect->SetupAttachment(RootComponent);
	ExplosionEffect->bAutoActivate = false;

	TracerEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TracerEffect"));
	TracerEffect->SetupAttachment(RootComponent);
	TracerEffect->bAutoActivate = false;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bAutoActivate = false;

	bReplicates = true;
}

void ADRProjectileActorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CapsuleComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnComponentHit);
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);

	if (ExplosionEffect && ExplosionEffect->GetAsset())
	{
		ExplosionEffect->OnSystemFinished.AddDynamic(this, &ThisClass::OnExplosionEffectFinished);
	}
}

void ADRProjectileActorBase::OnExplode()
{
	if (ExplosionEffect && ExplosionEffect->GetAsset())
	{
		ExplosionEffect->Activate();
	}
	else
	{
		OnExplosionEffectFinished(nullptr);
	}

	StaticMeshComponent->SetHiddenInGame(true); // can't wait explosion finished to hide the static mesh
	TracerEffect->Deactivate();
}

void ADRProjectileActorBase::ApplyDamage(AActor* TargetActor)
{
	UDRHealthComponent* DRHealthComponent = UDRHealthComponent::FindHealthComponent(TargetActor);
	if (DRHealthComponent)
	{
		DRHealthComponent->ApplyDamage(GetInstigator(), DamageAmount);
	}

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
	{
		float CurrentHealth = ASC->GetNumericAttribute(ULyraHealthSet::GetHealthAttribute());
		ASC->SetNumericAttributeBase(ULyraHealthSet::GetHealthAttribute(), CurrentHealth - DamageAmount);
	}
}

void ADRProjectileActorBase::OnExplosionEffectFinished(UNiagaraComponent* ParticleSystem)
{
	GetWorld()->GetSubsystem<UDRObjectPoolSubsystem>()->ReturnToPool(this);
}

void ADRProjectileActorBase::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetInstigator() || OtherActor == GetOwner())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("%s OnComponentHit %s."), *GetName(), *OtherActor->GetName());

	OnExplode();

	ApplyDamage(OtherActor);
}

void ADRProjectileActorBase::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString InstigatorName = GetInstigator() ? GetInstigator()->GetName() : "NULL";
	FString OwnerName = GetOwner() ? GetOwner()->GetName() : "NULL";

	if (OtherActor == GetInstigator() || OtherActor == GetOwner())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("%s OnComponentBeginOverlap %s, Instigator %s, Owner %s."), *GetName(), *OtherActor->GetName(), *InstigatorName, *OwnerName);

	OnExplode();

	ApplyDamage(OtherActor);
}

void ADRProjectileActorBase::OnActive_Implementation(class APawn* NewInstigator, class AActor* NewOwner)
{
	Super::OnActive_Implementation(NewInstigator, NewOwner);

	ProjectileMovementComponent->SetUpdatedComponent(GetRootComponent());
	ProjectileMovementComponent->Velocity = InitialSpeed * GetActorForwardVector();
	ProjectileMovementComponent->Activate();

	if (MaxLifeTime > 0.0f) // 0.0 means infinite
	{
		MaxLifeTimerDelegate.BindUFunction(this, FName("OnExplode"));
		GetWorld()->GetTimerManager().SetTimer(MaxLifeTimerHandle, MaxLifeTime, false);
	}

	if (TracerEffect && TracerEffect->GetAsset())
	{
		TracerEffect->Activate();
	}

	StaticMeshComponent->SetHiddenInGame(false);
}

void ADRProjectileActorBase::OnDeActive_Implementation()
{
	Super::OnDeActive_Implementation();

	GetWorld()->GetTimerManager().ClearTimer(MaxLifeTimerHandle);

	ProjectileMovementComponent->Deactivate();
	ProjectileMovementComponent->StopMovementImmediately();
}