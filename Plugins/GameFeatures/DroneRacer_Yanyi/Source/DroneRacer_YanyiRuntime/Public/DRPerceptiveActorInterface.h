// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DRPerceptiveActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDRPerceptiveActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DRONERACER_YANYIRUNTIME_API IDRPerceptiveActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PerceptiveActorInterface")
	void OnActorPerceptionUpdated(AActor* Actor, bool WasSuccessfullySensed);
};
