// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DRUserWidget_GameMenu.h"
#include "Subsystems/DRSaveGameSubsystem.h"
#include "CommonTextBlock.h"
#include "Components/Button.h"
#include "Input/CommonUIInputTypes.h"
#include <Kismet/KismetMathLibrary.h>
#include "NativeGameplayTags.h"
#include "Performance/LyraPerformanceSettings.h"
#include "GameFramework/GameUserSettings.h"

#include "Kismet/GameplayStatics.h"
#include "Components/DRWidgetManagerComponent.h"

// #include "Input/CommonUIInputSettings.h"

#define LOCTEXT_NAMESPACE "USERWIDGET_GAMEMENU"

UDRUserWidget_GameMenu::UDRUserWidget_GameMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UDRUserWidget_GameMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UDRSaveGameSubsystem* DRSaveGameSubsystem = GetGameInstance()->GetSubsystem<UDRSaveGameSubsystem>())
	{
		float BestRecordInSeconds = DRSaveGameSubsystem->GetBestRecordFromSaveGame();

		FTimespan BestRecord = UKismetMathLibrary::FromSeconds(BestRecordInSeconds);

		Text_BestRecord->SetText(FText::Format(LOCTEXT("TEXT_BESTRECORD", "{0}:{1}:{2}"), BestRecord.GetMinutes(), BestRecord.GetSeconds(), BestRecord.GetFractionMilli()));

		ResolutionLevel = DRSaveGameSubsystem->GetResolutionLevelFromSaveGame();

		UpdateResolution();
	}

	const FGameplayTag TAG_UI_ACTION_ESCAPE = FGameplayTag::RequestGameplayTag(FName("UI.Action.Escape"));
	RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_ACTION_ESCAPE), false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction)));

	if (Btn_StartGame)
	{
		Btn_StartGame->OnClicked.AddDynamic(this, &ThisClass::OnStartGameBtnClicked);
	}
}

void UDRUserWidget_GameMenu::HandleEscapeAction()
{
	// UE_LOG(LogTemp, Warning, TEXT("UserWidget_GameMenu HandleEscapeAction"));

	BP_HandleEscapeAction();
}

void UDRUserWidget_GameMenu::ResolutionUp()
{
	ResolutionLevel++;
	ResolutionLevel = FMath::Clamp(ResolutionLevel, 0, 3);// 2K TODO Yanyi Bao
	UpdateResolution();
}

void UDRUserWidget_GameMenu::ResolutionDown()
{
	ResolutionLevel--;
	ResolutionLevel = FMath::Clamp(ResolutionLevel, 0, 3);// 2K TODO Yanyi Bao
	UpdateResolution();
}

void UDRUserWidget_GameMenu::GetStandardWindowResolution(int32 Level, FIntPoint& OutResolution)
{
	static TArray<FIntPoint> StandardResolutions;
	if (StandardResolutions.Num() == 0)
	{
		{
			new(StandardResolutions) FIntPoint(640, 360); // nHD
			new(StandardResolutions) FIntPoint(960, 540); // qHD
			new(StandardResolutions) FIntPoint(1280, 720); // HD
			new(StandardResolutions) FIntPoint(1920, 1080); // FHD
		}
	}

	// Sort the list by total resolution size
	StandardResolutions.Sort([](const FIntPoint& A, const FIntPoint& B) { return (A.X * A.Y) < (B.X * B.Y); });

	OutResolution = Level < StandardResolutions.Num() ? StandardResolutions[Level] : StandardResolutions[0];
}

void UDRUserWidget_GameMenu::UpdateResolution()
{
	FIntPoint OutResolution;
	GetStandardWindowResolution(ResolutionLevel, OutResolution);
	Text_Resolution->SetText(FText::Format(LOCTEXT("TEXT_RESOLUTION", "{0}x{1}"), OutResolution.X, OutResolution.Y));

	if (GEngine)
	{
		GEngine->GetGameUserSettings()->SetScreenResolution(OutResolution);
	}
}

void UDRUserWidget_GameMenu::OnStartGameBtnClicked()
{
	APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (UDRWidgetManagerComponent* WidgetManager = UDRWidgetManagerComponent::GetComponent(LocalPlayerController))
	{
		WidgetManager->RequestHideWidget(FName("WBP_GameMenu"));
		WidgetManager->RequestShowWidget(FName("WBP_PlaneInfo"));
	}
}

#undef LOCTEXT_NAMESPACE