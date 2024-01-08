// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/DRGameplayAbility_LineTrace.h"
#include "Components/DRLineTraceWeaponComponent.h"
#include "AbilitySystem/LyraGameplayAbilityTargetData_SingleTargetHit.h"

UDRGameplayAbility_LineTrace::UDRGameplayAbility_LineTrace(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{

}

bool UDRGameplayAbility_LineTrace::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
	if (bResult && AvatarActor)
	{
		UDRLineTraceWeaponComponent* LineTraceWeaponComponent = UDRLineTraceWeaponComponent::FindLineTraceWeaponComponent(AvatarActor);
		bResult = LineTraceWeaponComponent->CanOpenFire();
	}

	return bResult;

}

void UDRGameplayAbility_LineTrace::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(CurrentActorInfo);

	AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
	check(AvatarActor);

	TArray<FHitResult> HitResults;
	PerformLocalTargeting(HitResults);

	FGameplayAbilityTargetDataHandle TargetData;
	if (HitResults.Num() > 0)
	{
		const int32 CartridgeID = FMath::Rand();
		for (const FHitResult& FoundHit : HitResults)
		{
			FLyraGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FLyraGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = FoundHit;
			NewTargetData->CartridgeID = CartridgeID;

			TargetData.Add(NewTargetData);
		}

		OnLineTraceAbilityTargetDataReady(TargetData);
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UDRGameplayAbility_LineTrace::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDRGameplayAbility_LineTrace::PerformLocalTargeting(OUT TArray<FHitResult>& OutHits)
{
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	if (AvatarPawn)
	{
		UDRLineTraceWeaponComponent* LineTraceWeaponComponent = UDRLineTraceWeaponComponent::FindLineTraceWeaponComponent(AvatarPawn);

		TArray<FVector> StartPoints;

		if (LineTraceWeaponComponent && LineTraceWeaponComponent->GetMuzzlePositions(StartPoints))
		{
			FVector TraceDirection = LineTraceWeaponComponent->GetTargetingDirection();
			float MaxTraceDistance = LineTraceWeaponComponent->GetMaxTraceDistance();

			LineTraceWeaponComponent->OnFire(); // TODO should update when activate ability

			FCollisionQueryParams TraceParams;
			for (const FVector& StartPoint : StartPoints)
			{
				FVector EndPoint = StartPoint + TraceDirection * MaxTraceDistance;

				FHitResult HitResult;
				if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_WorldDynamic, TraceParams))
				{
					OutHits.Add(HitResult);
					// UE_LOG(LogTemp, Warning, TEXT("GameplayAbility_LineTrace Hit Actor %s."), *(HitResult.GetActor()->GetName()));
				}

				// DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Red, false, 60.0f, 0, 2.0f);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No LineTraceWeaponComponent or Muzzle Positions Found!"));
		}
	}
}
