// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DRPoolableInterface.h"
#include "DRPoolableActorBase.generated.h"

UCLASS()
class DRONERACER_YANYIRUNTIME_API ADRPoolableActorBase : public AActor, public IDRPoolableInterface
{
	GENERATED_BODY()

protected:

	bool VisibleInGame;
	
public:	
	// Sets default values for this actor's properties
	ADRPoolableActorBase();

	virtual void Destroyed() override;

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	void OnActive_Implementation(class APawn* NewInstigator, class AActor* NewOwner) override;

	void InitializeTransform_Implementation(const FVector& Location, const FRotator& Rotation) override;

	void OnDeActive_Implementation() override;

	bool IsBussy_Implementation() override;

};
