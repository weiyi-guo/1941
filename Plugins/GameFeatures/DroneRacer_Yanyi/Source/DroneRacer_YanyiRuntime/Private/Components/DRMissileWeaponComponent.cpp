// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DRMissileWeaponComponent.h"
#include "Subsystems/DRObjectPoolSubsystem.h"
#include "DRPoolableActorBase.h"

// Sets default values for this component's properties
UDRMissileWeaponComponent::UDRMissileWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void UDRMissileWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UDRMissileWeaponComponent::CanOpenFire() const
{
	if (WeaponFireRate <= 0.0f)
	{
		return false;
	}
	UWorld* World = GetWorld();
	check(World);
	double TimeSeconds = World->GetTimeSeconds();
	return (TimeSeconds - TimeLastFired) >= 1.0 / WeaponFireRate;
}

void UDRMissileWeaponComponent::OnFire()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	check(World);
	TimeLastFired = World->GetTimeSeconds();

	if (!MissileActorClass)
	{
		return;
	}

	if (UDRObjectPoolSubsystem* ObjectPoolSubsystem = GetWorld()->GetSubsystem<UDRObjectPoolSubsystem>())
	{
		if (ADRPoolableActorBase* MissileActor = Cast<ADRPoolableActorBase>(ObjectPoolSubsystem->GetObjectOfType(MissileActorClass)))
		{
			if(APawn* NewInstigator = Cast<APawn>(GetOwner()))
			{ 
				MissileActor->InitializeTransform(NewInstigator->GetActorLocation(), NewInstigator->GetActorRotation());
				MissileActor->OnActive(NewInstigator, NewInstigator);
			}
		}
	}
}
