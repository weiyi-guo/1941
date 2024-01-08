// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DREnemyBase.h"
#include "DRStationaryEnemy.generated.h"

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API ADRStationaryEnemy : public ADREnemyBase
{
	GENERATED_BODY()
	
public:
	
	virtual void OnActorPerceptionUpdated_Implementation(AActor* Actor, bool WasSuccessfullySensed) override;

protected:

	virtual void InitializeEnemy() override;

	virtual void TraceTargetActors_Implementation() override;

	FTimerHandle TraceTargetTimerHandle;

	FTimerDelegate TraceTargetTimerDelegate;
};
