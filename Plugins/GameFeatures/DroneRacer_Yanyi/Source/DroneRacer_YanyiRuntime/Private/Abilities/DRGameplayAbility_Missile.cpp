// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/DRGameplayAbility_Missile.h"
#include "DRCharacter.h"
#include "Attributes/DRCombatSet.h"

UDRGameplayAbility_Missile::UDRGameplayAbility_Missile(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{

}

bool UDRGameplayAbility_Missile::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const ADRCharacter* DRCharacter = Cast<ADRCharacter>(ActorInfo->AvatarActor.Get());
	if (!DRCharacter || !DRCharacter->CanSecondaryWeaponOpenFire())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (const UDRCombatSet* CombatSet = ActorInfo->AbilitySystemComponent->GetSet<UDRCombatSet>())
	{
		if (CombatSet->GetRocketNum() <= 0.0f)
		{
			return false;
		}
	}

	return true;
}

void UDRGameplayAbility_Missile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo->AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("DRGameplayAbility_Missile::ActivateAbility Failed to get the AbilitySystemComponent"));
		return;
	}

	if (const UDRCombatSet* CombatSet = ActorInfo->AbilitySystemComponent->GetSet<UDRCombatSet>())
	{
		float CurrentRocketNum = CombatSet->GetRocketNum();
		CurrentRocketNum--;

		ActorInfo->AbilitySystemComponent->SetNumericAttributeBase(UDRCombatSet::GetRocketNumAttribute(), CurrentRocketNum);
	}
}

void UDRGameplayAbility_Missile::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
