// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/DRUserFacingExperienceDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DRUserFacingExperienceDefinition)

UCommonSession_HostSessionRequest* UDRUserFacingExperienceDefinition::CreateHostingRequest() const
{
	UCommonSession_HostSessionRequest* Result = Super::CreateHostingRequest();

	return Result;
}