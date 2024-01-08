// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/DRWidgetSet.h"

TSubclassOf<class UUserWidget> UDRWidgetSet::FindWidgetClassByName(const FName& WidgetName)
{
	for (const FDRWidgetConfig& WidgetConfig : WidgetConfigs)
	{
		if (WidgetConfig.WidgetName.IsEqual(WidgetName))
		{
			return WidgetConfig.WidgetClass;
		}
	}

	return nullptr;
}

bool UDRWidgetSet::FindWidgetConfigByName(const FName& WidgetName, FDRWidgetConfig& WidgetConfig)
{
	for (const FDRWidgetConfig& Config : WidgetConfigs)
	{
		if (Config.WidgetName.IsEqual(WidgetName))
		{
			WidgetConfig = Config;
			return true;
		}
	}

	return false;
}
