// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DRUserWidget_PlaneInfo.h"
#include "DataAssets/DRPlaneSet.h"
#include "JsonObjectConverter.h"
#include "CommonTextBlock.h"
#include "Components/Button.h"
#include "DRPlayerState.h"
#include "CommonButtonBase.h"
#include "DRCharacter_Lobby.h"

#define LOCTEXT_NAMESPACE "USERWIDGET_PLANEINFO"

void UDRUserWidget_PlaneInfo::NativeConstruct()
{
	Super::NativeConstruct();

	if (ADRPlayerState* DRPlayerState = GetOwningPlayerState<ADRPlayerState>())
	{
		// TODO
		FDRPlaneConfig PlaneConfigTemp;
		if (DRPlayerState->GetSelectedPlaneConfig(PlaneConfigTemp))
		{
			PlaneConfig = FDRPlaneConfig_Short(PlaneConfigTemp);
			Text_PlaneName->SetText(FText::FromString(PlaneConfig.DisplayName));
			Text_MaxSpeed->SetText(FText::Format(LOCTEXT("TEXT_MAXSPEED", "Max Speed: {0}"), PlaneConfig.MaxSpeed));
			Text_MaxThrottle->SetText(FText::Format(LOCTEXT("TEXT_MAXTHROTTLE", "Max Throttle: {0}"), PlaneConfig.MaxThrottleAmount));
			Text_ThrottleSensitivity->SetText(FText::Format(LOCTEXT("TEXT_THROTTLESENSITIVITY", "Throttle Sensitivity: {0}"), PlaneConfig.ThrottleSensitivity));
			Text_PitchSensitivity->SetText(FText::Format(LOCTEXT("TEXT_PITCHSENSITIVITY", "Pitch Sensitivity: {0}"), PlaneConfig.PitchSensitivity));
			Text_RollSensitivity->SetText(FText::Format(LOCTEXT("TEXT_ROLLSENSITIVITY", "Roll Sensitivity: {0}"), PlaneConfig.RollSensitivity));
		}
	}
}

bool UDRUserWidget_PlaneInfo::Initialize()
{
	bool Result = Super::Initialize();

	//if (Btn_ChanhePlane)
	//{
	//	Btn_ChanhePlane1->OnClicked.AddDynamic(this, &ThisClass::OnChangePlaneBtnClicked);
	//}

	Btn_PrevPlane->OnClicked().AddUFunction(this, "OnPrevPlaneBtnClicked");
	Btn_NextPlane->OnClicked().AddUFunction(this, "OnNextPlaneBtnClicked");

	return Result;
}

void UDRUserWidget_PlaneInfo::OnChangePlaneBtnClicked()
{
	// UE_LOG(LogTemp, Warning, TEXT("OnChanhePlaneBtnClicked => %s"), *PlaneConfig.PlaneName);

	if (ADRPlayerState* DRPlayerState = GetOwningPlayerState<ADRPlayerState>())
	{
		DRPlayerState->UpdateSelectedPlane(PlaneConfig.PlaneName);
	}
}

void UDRUserWidget_PlaneInfo::OnPrevPlaneBtnClicked()
{
	// UE_LOG(LogTemp, Warning, TEXT("OnPrevPlaneBtnClicked"));
	if (ADRCharacter_Lobby* Character_Lobby = Cast<ADRCharacter_Lobby>(GetOwningPlayerPawn()))
	{
		Character_Lobby->ViewPrevPlane();
	}
}

void UDRUserWidget_PlaneInfo::OnNextPlaneBtnClicked()
{
	// UE_LOG(LogTemp, Warning, TEXT("OnNextPlaneBtnClicked"));
	if (ADRCharacter_Lobby* Character_Lobby = Cast<ADRCharacter_Lobby>(GetOwningPlayerPawn()))
	{
		Character_Lobby->ViewNextPlane();
	}
}

bool UDRUserWidget_PlaneInfo::UpdateWidget_Implementation(const FString& Payload)
{
	// FDRPlaneConfig_Short PlaneConfig;
	if(FJsonObjectConverter::JsonObjectStringToUStruct(Payload, &PlaneConfig))
	{
		Text_PlaneName->SetText(FText::FromString(PlaneConfig.DisplayName));
		Text_MaxSpeed->SetText(FText::Format(LOCTEXT("TEXT_MAXSPEED", "Max Speed: {0}"), PlaneConfig.MaxSpeed));
		Text_MaxThrottle->SetText(FText::Format(LOCTEXT("TEXT_MAXTHROTTLE", "Max Throttle: {0}"), PlaneConfig.MaxThrottleAmount));
		Text_ThrottleSensitivity->SetText(FText::Format(LOCTEXT("TEXT_THROTTLESENSITIVITY", "Throttle Sensitivity: {0}"), PlaneConfig.ThrottleSensitivity));
		Text_PitchSensitivity->SetText(FText::Format(LOCTEXT("TEXT_PITCHSENSITIVITY", "Pitch Sensitivity: {0}"), PlaneConfig.PitchSensitivity));
		Text_RollSensitivity->SetText(FText::Format(LOCTEXT("TEXT_ROLLSENSITIVITY", "Roll Sensitivity: {0}"), PlaneConfig.RollSensitivity));
		return true;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE