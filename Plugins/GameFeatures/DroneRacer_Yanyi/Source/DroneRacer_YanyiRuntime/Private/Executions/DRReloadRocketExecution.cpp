// Fill out your copyright notice in the Description page of Project Settings.


#include "Executions/DRReloadRocketExecution.h"
#include "Attributes/DRCombatSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DRReloadRocketExecution)

struct FRocketNumStatics
{
	FGameplayEffectAttributeCaptureDefinition MaxRocketNumDef;

	FRocketNumStatics()
	{
		MaxRocketNumDef = FGameplayEffectAttributeCaptureDefinition(UDRCombatSet::GetMaxRocketNumAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FRocketNumStatics& RocketNumStatics()
{
	static FRocketNumStatics Statics;
	return Statics;
}

UDRReloadRocketExecution::UDRReloadRocketExecution()
{
	RelevantAttributesToCapture.Add(RocketNumStatics().MaxRocketNumDef);
}

void UDRReloadRocketExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float MaxRocketNum = 0.0f;
	bool Result = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(RocketNumStatics().MaxRocketNumDef, EvaluateParameters, MaxRocketNum);

	if (Result)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttemptCalculateCapturedAttributeMagnitude Success."));
	}

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UDRCombatSet::GetRocketNumAttribute(), EGameplayModOp::Override, MaxRocketNum));
#endif // #if WITH_SERVER_CODE
}
