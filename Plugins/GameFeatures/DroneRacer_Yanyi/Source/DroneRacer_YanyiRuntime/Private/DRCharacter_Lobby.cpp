// Fill out your copyright notice in the Description page of Project Settings.


#include "DRCharacter_Lobby.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include <Camera/CameraComponent.h>
#include "DRPlayerState.h"
#include "Components/DRWidgetManagerComponent.h"
#include "JsonObjectConverter.h"

ADRCharacter_Lobby::ADRCharacter_Lobby(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(RootComponent);
}

void ADRCharacter_Lobby::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController)
	{
		WidgetManagerComponent = UDRWidgetManagerComponent::GetComponent(NewController);
		if (WidgetManagerComponent)
		{
			// WidgetManagerComponent->RequestShowWidget(FName("WBP_PlaneInfo"));
			// WidgetManagerComponent->RequestShowWidget(FName("WBP_GameMenu"));
		}
	}

	if (ADRPlayerState* DRPlayerState = GetPlayerState<ADRPlayerState>())
	{
		CurrentViewingPlane = DRPlayerState->GetSelectedPlaneName();

		FDRPlaneConfig PlaneConfig;
		DRPlayerState->GetSelectedPlaneConfig(PlaneConfig);
		UpdateUI(PlaneConfig);
	}
}

void ADRCharacter_Lobby::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllActors);

	for (auto TargetActor : AllActors)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(TargetActor);
		ViewPointForPlanes.Add(PlayerStart->PlayerStartTag, PlayerStart);
	}

	check(PlaneSet);
}

void ADRCharacter_Lobby::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(NeedMoving)
	{ 
		FVector DistanceToTarget = TargetViewPosition - GetActorLocation();
		if (DistanceToTarget.Length() <= 50.0f)
		{
			NeedMoving = false;
		}
		else
		{
			FVector NewLocation = FMath::VInterpTo(GetActorLocation(), TargetViewPosition, DeltaSeconds, 20.0f);
			SetActorLocation(NewLocation);
		}
	}
}

void ADRCharacter_Lobby::ViewNextPlane()
{
	if (ADRPlayerState* DRPlayerState = GetPlayerState<ADRPlayerState>())
	{
		FDRPlaneConfig NextPlaneConfig = PlaneSet->GetNextPlaneConfig(CurrentViewingPlane);
		if (ViewPointForPlanes.Contains(FName(NextPlaneConfig.PlayerStartTag)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Moving from %s to %s."), *CurrentViewingPlane, *NextPlaneConfig.PlaneName);
			CurrentViewingPlane = NextPlaneConfig.PlaneName;
			TargetViewPosition = ViewPointForPlanes[FName(NextPlaneConfig.PlayerStartTag)]->GetActorLocation();
			NeedMoving = true;

			UpdateUI(NextPlaneConfig);
		}
	}
}

void ADRCharacter_Lobby::ViewPrevPlane()
{
	if (ADRPlayerState* DRPlayerState = GetPlayerState<ADRPlayerState>())
	{
		FDRPlaneConfig PrevPlaneConfig = PlaneSet->GetPrevPlaneConfig(CurrentViewingPlane);
		if (ViewPointForPlanes.Contains(FName(PrevPlaneConfig.PlayerStartTag)))
		{
			CurrentViewingPlane = PrevPlaneConfig.PlaneName;
			TargetViewPosition = ViewPointForPlanes[FName(PrevPlaneConfig.PlayerStartTag)]->GetActorLocation();
			NeedMoving = true;

			UpdateUI(PrevPlaneConfig);
		}
	}
}

void ADRCharacter_Lobby::UpdateUI(const FDRPlaneConfig& PlaneConfig)
{
	FDRPlaneConfig_Short PlaneConfig_Short(PlaneConfig);
	FString MessagePayload;

	if (WidgetManagerComponent && FJsonObjectConverter::UStructToJsonObjectString(PlaneConfig_Short, MessagePayload))
	{
		WidgetManagerComponent->RequestUpdateWidget(FName("WBP_PlaneInfo"), MessagePayload);
	}
}
