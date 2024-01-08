// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DRLineTraceWeaponComponent.h"
#include <Components/ArrowComponent.h>
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "DRBlueprintFunctionLibrary.h"

// Sets default values for this component's properties
UDRLineTraceWeaponComponent::UDRLineTraceWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void UDRLineTraceWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	Muzzles = GetOwner()->GetComponentsByTag(UArrowComponent::StaticClass(), FName("Muzzle"));

	//for (UActorComponent* Muzzle : Muzzles)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Find Muzzle %s."), *Muzzle->GetName());
	//}
}

void UDRLineTraceWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool UDRLineTraceWeaponComponent::CanOpenFire() const
{	
	// TODO
	// return true;

	if (WeaponFireRate <= 0.0f)
	{
		return false;
	}
	UWorld* World = GetWorld();
	check(World);
	double TimeSeconds = World->GetTimeSeconds();
	return (TimeSeconds - TimeLastFired) >= 1.0 / WeaponFireRate;
}

bool UDRLineTraceWeaponComponent::GetMuzzlePositions(OUT TArray<FVector>& MuzzlePositions) const
{
	MuzzlePositions.Empty();

	for(UActorComponent* Muzzle : Muzzles)
	{
		if (UArrowComponent* ArrowComponent = Cast<UArrowComponent>(Muzzle))
		{
			MuzzlePositions.Add(ArrowComponent->GetComponentLocation());
		}
	}

	return MuzzlePositions.Num() > 0;
}

FVector UDRLineTraceWeaponComponent::GetTargetingDirection() const
{
	return GetOwner()->GetActorForwardVector();
}

void UDRLineTraceWeaponComponent::OnFire()
{
	FString NetRole = UDRBlueprintFunctionLibrary::ConvertNetRoleEnumToString(GetOwner()->GetLocalRole());
	FString NetMode = UDRBlueprintFunctionLibrary::ConvertNetModeEnumToString(GetOwner()->GetNetMode());

	// UE_LOG(LogTemp, Warning, TEXT("LineTraceWeaponComponent OnFire %s %s"), *NetRole, *NetMode);

	UWorld * World = GetWorld();
	check(World);
	TimeLastFired = World->GetTimeSeconds();

	if (!BulletTracerEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("BulletTracerEffect is nullptr!"));
		return;
	}

	IsUserTrigger = !IsUserTrigger;
	for (UActorComponent* Muzzle : Muzzles)
	{
		FString MuzzleName = Muzzle->GetName();
		UArrowComponent* ArrowComponent = Cast<UArrowComponent>(Muzzle);

		if (!TracerComponents.Contains(MuzzleName) || !IsValid(TracerComponents[MuzzleName]))
		{
			UNiagaraComponent* TracerComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(BulletTracerEffect, ArrowComponent, FName("None"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true, true, ENCPoolMethod::None, true);

			if(!TracerComponents.Contains(MuzzleName))
			{ 
				TracerComponents.Add(MuzzleName, TracerComponent);
			}
			else
			{
				TracerComponents[MuzzleName] = TracerComponent;
			}
		}

		if (!IsValid(TracerComponents[MuzzleName]))
		{
			continue;
		}

		TracerComponents[MuzzleName]->SetVariableBool("User.Trigger", IsUserTrigger);
		TArray<FVector> ImpactPositions;
		FVector ImpactPosition = ArrowComponent->GetComponentLocation() + GetTargetingDirection() * MaxTraceDistance;
		ImpactPositions.Add(ImpactPosition);

		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TracerComponents[MuzzleName], "User.ImpactPositions", ImpactPositions);
	}
}
