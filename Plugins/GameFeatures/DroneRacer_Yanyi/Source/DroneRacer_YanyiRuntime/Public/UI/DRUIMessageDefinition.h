#pragma once
#include "CoreMinimal.h"
#include "DRUIMessageDefinition.generated.h"

USTRUCT(BlueprintType)
struct FDRBattleResult
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool WinOrLoss;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//float BestRecordInSeconds;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//float CurrentRecordInSeconds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Player1Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Player2Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Player1Score;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Player2Score;

	FDRBattleResult()
		: WinOrLoss(false),
		//BestRecordInSeconds(0.0f),
		//CurrentRecordInSeconds(0.0f),
		Player1Name(""),
		Player2Name(""),
		Player1Score(0.0f),
		Player2Score(0.0f)
	{}

	// FDRBattleResult(bool _WinOrLoss, float _BestRecordInSeconds, float _CurrentRecordInSeconds, FString& _Player1Name, FString& _Player2Name, float _Player1Score, float _Player2Score)
	FDRBattleResult(bool _WinOrLoss, const FString& _Player1Name, const FString& _Player2Name, float _Player1Score, float _Player2Score)
		: WinOrLoss(_WinOrLoss),
		//BestRecordInSeconds(_BestRecordInSeconds),
		//CurrentRecordInSeconds(_CurrentRecordInSeconds),
		Player1Name(_Player1Name),
		Player2Name(_Player2Name),
		Player1Score(_Player1Score),
		Player2Score(_Player2Score)
	{}
};