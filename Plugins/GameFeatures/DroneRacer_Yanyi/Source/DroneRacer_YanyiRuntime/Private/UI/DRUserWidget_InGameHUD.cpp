// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DRUserWidget_InGameHUD.h"
#include "CommonTextBlock.h"


#define LOCTEXT_NAMESPACE "USERWIDGET_INGAMEHUD"

void UDRUserWidget_InGameHUD::NativeConstruct()
{
	//UE_LOG(LogTemp, Warning, TEXT("UserWidget_InGameHUD::NativeConstruct"));
	Super::NativeConstruct();
}

bool UDRUserWidget_InGameHUD::Initialize()
{
	//UE_LOG(LogTemp, Warning, TEXT("UserWidget_InGameHUD::Initialize"));
	return Super::Initialize();
}

bool UDRUserWidget_InGameHUD::UpdateWidget_Implementation(const FString& Payload)
{
	return false;
}

void UDRUserWidget_InGameHUD::UpdatePlayerScores(float Scores)
{
	Text_Scores->SetText(FText::Format(LOCTEXT("TEXT_SCORES", "Score: {0}"), Scores));
}

void UDRUserWidget_InGameHUD::UpdatePlayerInfo(FString PlayerName, uint8 TeamID)
{
	UE_LOG(LogTemp, Warning, TEXT("UpdatePlayerInfo => %s, %d"), *PlayerName, TeamID);
	Text_PlayerName->SetText(FText::Format(LOCTEXT("TEXT_PLAYERNAME", "Player: {0}"), FText::FromString(PlayerName)));

	Text_TeamID->SetText(FText::Format(LOCTEXT("TEXT_TEAMID", "Team: {0}"), TeamID));
}

void UDRUserWidget_InGameHUD::UpdateInGameHUD(float HP, float Altitude, float Speed, float ThrottleAmount, float EngineForce, int32 RocketNum)
{
	Text_HP->SetText(FText::Format(LOCTEXT("TEXT_HP", "HP: {0}"), HP));
	Text_Altitude->SetText(FText::Format(LOCTEXT("TEXT_ALTITUDE", "Altitude: {0}"), Altitude));
	Text_Speed->SetText(FText::Format(LOCTEXT("TEXT_SPEED", "Speed: {0}"), Speed));
	Text_ThrottleAmount->SetText(FText::Format(LOCTEXT("TEXT_THROTTLEAMOUNT", "Throttle: {0}"), ThrottleAmount));
	Text_EngineForce->SetText(FText::Format(LOCTEXT("TEXT_ENGINEFORCE", "EngineForce: {0}"), EngineForce));
	Text_RocketNum->SetText(FText::Format(LOCTEXT("TEXT_ROCKETNUM", "Rocket: {0}"), RocketNum));
}

#undef LOCTEXT_NAMESPACE