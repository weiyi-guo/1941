// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/DRPlaneSet.h"

bool UDRPlaneSet::GetPlaneConfigByName(const FString& PlaneName, FDRPlaneConfig& PlaneConfig)
{
	for (auto& Config : PlaneConfigs)
	{
		if (Config.PlaneName.Equals(PlaneName))
		{
			PlaneConfig = Config;
			return true;
		}
	}

	return false;
}

FDRPlaneConfig UDRPlaneSet::GetNextPlaneConfig(FString CurrentPlaneName)
{
	int32 CurrentPlaneIndex = 0;
	for (; CurrentPlaneIndex < PlaneConfigs.Num(); ++CurrentPlaneIndex)
	{
		if (PlaneConfigs[CurrentPlaneIndex].PlaneName.Equals(CurrentPlaneName))
		{
			break;
		}
	}
	int32 NextPlaneIndex = CurrentPlaneIndex + 1 >= PlaneConfigs.Num() ? 0 : CurrentPlaneIndex + 1;
	return PlaneConfigs[NextPlaneIndex];
}

FDRPlaneConfig UDRPlaneSet::GetPrevPlaneConfig(FString CurrentPlaneName)
{
	int32 CurrentPlaneIndex = 0;
	for (; CurrentPlaneIndex < PlaneConfigs.Num(); ++CurrentPlaneIndex)
	{
		if (PlaneConfigs[CurrentPlaneIndex].PlaneName.Equals(CurrentPlaneName))
		{
			break;
		}
	}
	int32 PrevPlaneIndex = CurrentPlaneIndex - 1 < 0 ? PlaneConfigs.Num() - 1 : CurrentPlaneIndex - 1;
	return PlaneConfigs[PrevPlaneIndex];
}
