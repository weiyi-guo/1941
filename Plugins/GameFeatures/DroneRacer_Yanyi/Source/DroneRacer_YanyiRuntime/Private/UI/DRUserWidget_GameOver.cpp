// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DRUserWidget_GameOver.h"
#include "UI/DRUIMessageDefinition.h"
#include "CommonTextBlock.h"
#include "JsonObjectConverter.h"
#include <Kismet/KismetMathLibrary.h>

#define LOCTEXT_NAMESPACE "USERWIDGET_GAMEOVER"

bool UDRUserWidget_GameOver::UpdateWidget_Implementation(const FString& Payload)
{
	FDRBattleResult BattleResult;
	if (FJsonObjectConverter::JsonObjectStringToUStruct(Payload, &BattleResult))
	{
		Text_BattleResult->SetText(BattleResult.WinOrLoss ? FText::FromString("WIN") : FText::FromString("LOSE"));

		/*
		FTimespan BestRecord = UKismetMathLibrary::FromSeconds(BattleResult.BestRecordInSeconds);
		FTimespan CurrentRecord = UKismetMathLibrary::FromSeconds(BattleResult.CurrentRecordInSeconds);

		Text_BestRecord->SetText(FText::Format(LOCTEXT("TEXT_BESTRECORD", "{0}:{1}:{2}"), BestRecord.GetMinutes(), BestRecord.GetSeconds(), BestRecord.GetFractionMilli()));

		Text_CurrentRecord->SetText(FText::Format(LOCTEXT("TEXT_CURRENTRECORD", "{0}:{1}:{2}"), CurrentRecord.GetMinutes(), BestRecord.GetSeconds(), BestRecord.GetFractionMilli()));
		*/

		Text_Player1Name->SetText(FText::FromString(BattleResult.Player1Name));

		Text_Player2Name->SetText(FText::FromString(BattleResult.Player2Name));
		
		Text_Player1Score->SetText(FText::Format(LOCTEXT("TEXT_PLAYER1SCORE", "{0}"), BattleResult.Player1Score));

		Text_Player2Score->SetText(FText::Format(LOCTEXT("TEXT_PLAYER2SCORE", "{0}"), BattleResult.Player2Score));
	}
	return false;
}

#undef LOCTEXT_NAMESPACE