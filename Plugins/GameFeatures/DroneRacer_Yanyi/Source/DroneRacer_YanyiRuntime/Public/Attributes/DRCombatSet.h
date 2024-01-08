// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "DRCombatSet.generated.h"

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API UDRCombatSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:

	UDRCombatSet();

	ATTRIBUTE_ACCESSORS(UDRCombatSet, RocketNum);
	ATTRIBUTE_ACCESSORS(UDRCombatSet, MaxRocketNum);

protected:

	UFUNCTION()
	void OnRep_RocketNum(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxRocketNum(const FGameplayAttributeData& OldValue);

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RocketNum, Category = "DroneRacer|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData RocketNum;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxRocketNum, Category = "DroneRacer|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxRocketNum;
	
};
