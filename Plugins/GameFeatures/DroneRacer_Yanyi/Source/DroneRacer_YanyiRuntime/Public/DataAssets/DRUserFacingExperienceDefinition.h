// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/LyraUserFacingExperienceDefinition.h"
#include "DRUserFacingExperienceDefinition.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DRONERACER_YANYIRUNTIME_API UDRUserFacingExperienceDefinition : public ULyraUserFacingExperienceDefinition
{
	GENERATED_BODY()
	
public:
	virtual UCommonSession_HostSessionRequest* CreateHostingRequest() const override;
};
