// Fill out your copyright notice in the Description page of Project Settings.


#include "DRStationaryEnemy.h"
#include "DRBlueprintFunctionLibrary.h"
#include "GameplayEffect.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"

void ADRStationaryEnemy::OnActorPerceptionUpdated_Implementation(AActor* Actor, bool WasSuccessfullySensed)
{
	Super::OnActorPerceptionUpdated_Implementation(Actor, WasSuccessfullySensed);

	if (TargetActors.Num() > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TraceTargetTimerHandle, TraceTargetTimerDelegate, 1.0f / FireRate, true);
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("%s Stop Attacking"), *GetName());
		GetWorld()->GetTimerManager().ClearTimer(TraceTargetTimerHandle);
	}
}

void ADRStationaryEnemy::InitializeEnemy()
{
	TraceTargetTimerDelegate.BindUFunction(this, "TraceTargetActors");
}

void ADRStationaryEnemy::TraceTargetActors_Implementation()
{
	for (TWeakObjectPtr<AActor> TargetActor : TargetActors)
	{
		if (TargetActor.IsValid())
		{
			FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0.0f, TargetRandomRange);

			FVector RayCastStart = GetMuzzleLocation();
			FVector RayCastEnd = TargetActor->GetActorLocation() + RandomOffset;
			FHitResult RayCastResult;

			FCollisionQueryParams CollisionParams;

			FVector AdjustedLocation = RayCastEnd;

#if !UE_BUILD_SHIPPING
			BulletsAmount++;
#endif

			if (GetWorld()->LineTraceSingleByChannel(RayCastResult, RayCastStart, RayCastEnd, ECC_Pawn, CollisionParams))
			{
				// AdjustedLocation = RayCastResult.ImpactPoint;

				// UDRBlueprintFunctionLibrary::ApplyDamage(this, RayCastResult.GetActor(), DamageAmount);

				// TODO: 2K Yanyi Fake Apply Damage
				if(RayCastResult.GetActor()->ActorHasTag("Player"))
				{ 
					if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(RayCastResult.GetActor()))
					{
						//float CurrentHealth = ASC->GetNumericAttribute(ULyraHealthSet::GetHealthAttribute());
						//ASC->SetNumericAttributeBase(ULyraHealthSet::GetHealthAttribute(), CurrentHealth - DamageAmount);
						UGameplayEffect* GameplayEffect = DamageEffectClass->GetDefaultObject<UGameplayEffect>();
						FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
						ASC->ApplyGameplayEffectToSelf(GameplayEffect, 0.0f, EffectContext);
					}
				}

#if !UE_BUILD_SHIPPING
				SucceededHitAmount++;
#endif
			}

			// UE_LOG(LogTemp, Warning, TEXT("%s Start Attack %s at position(%f, %f, %f)"), *GetName(), *TargetActor->GetName(), AdjustedLocation.X, AdjustedLocation.Y, AdjustedLocation.Z);
			AttackTargetActor(AdjustedLocation);

		}
	}
}
