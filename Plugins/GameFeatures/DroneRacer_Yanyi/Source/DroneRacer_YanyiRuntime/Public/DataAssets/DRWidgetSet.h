// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DRWidgetSet.generated.h"

UENUM(BlueprintType)
enum class EDRInputMode : uint8 {
	EDR_None			UMETA(DisplayName = "None"),
	EDR_GameOnly		UMETA(DisplayName = "InputModeGameOnly"),
	EDR_GameAndUI		UMETA(DisplayName = "InputModeGameAndUI"),
	EDR_UIOnly			UMETA(DisplayName = "InputModeUIOnly"),
};


USTRUCT(BlueprintType)
struct FDRWidgetConfig
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName WidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UUserWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EDRInputMode OverrideInputMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool ShowMouseCursor = false;
};



/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API UDRWidgetSet : public UDataAsset
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer|WidgetSet")
	TArray<FDRWidgetConfig> WidgetConfigs;

public:
	
	UFUNCTION(BlueprintCallable)
	TSubclassOf<class UUserWidget> FindWidgetClassByName(const FName& WidgetName);

	UFUNCTION(BlueprintCallable)
	bool FindWidgetConfigByName(const FName& WidgetName, FDRWidgetConfig& WidgetConfig);
};
