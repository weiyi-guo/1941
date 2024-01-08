// Fill out your copyright notice in the Description page of Project Settings.


#include "DREnemyController.h"
#include "Perception/AIPerceptionComponent.h"
#include "DRPerceptiveActorInterface.h"

ADREnemyController::ADREnemyController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	AAIController::SetGenericTeamId(FGenericTeamId(1));
}

void ADREnemyController::BeginPlay()
{
	Super::BeginPlay();
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ADREnemyController::OnActorPerceptionUpdated);
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ADREnemyController::OnPerceptionUpdated);
}

void ADREnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ADREnemyController::OnActorPerceptionUpdated);
}

void ADREnemyController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ADREnemyController::OnActorPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 2k TODO
	// For know, we only care about Player
	if (!Actor->ActorHasTag("Player"))
	{
		return;
	}

	// UE_LOG(LogTemp, Warning, TEXT("OnActorPerceptionUpdated %s."), *Actor->GetName());

	if (GetPawn() && GetPawn()->GetClass()->ImplementsInterface(UDRPerceptiveActorInterface::StaticClass()))
	{
		IDRPerceptiveActorInterface* PerceptiveActorInterface = Cast<IDRPerceptiveActorInterface>(GetPawn());

		PerceptiveActorInterface->Execute_OnActorPerceptionUpdated(GetPawn(), Actor, Stimulus.WasSuccessfullySensed());
	}
}

void ADREnemyController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{

}
