// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DREnemyBase.h"
#include "DRMovableEnemy.generated.h"

UCLASS()
class DRONERACER_YANYIRUNTIME_API ADRMovableEnemy : public ADREnemyBase
{
	GENERATED_BODY()

public:


protected:

	virtual void InitializeEnemy() override;

	virtual void TraceTargetActors_Implementation() override;

};
