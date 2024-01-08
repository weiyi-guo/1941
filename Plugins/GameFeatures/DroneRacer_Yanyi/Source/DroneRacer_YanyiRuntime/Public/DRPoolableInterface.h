// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DRPoolableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDRPoolableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DRONERACER_YANYIRUNTIME_API IDRPoolableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DroneRacer|PoolableInterface")
	void OnActive(class APawn* NewInstigator, class AActor* NewOwner);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DroneRacer|PoolableInterface")
	void InitializeTransform(const FVector& Location, const FRotator& Rotation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DroneRacer|PoolableInterface")
	void OnDeActive();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DroneRacer|PoolableInterface")
	bool IsBussy();
};
