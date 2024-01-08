// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneRacerGameMode.h"
#include "Subsystems/DRSaveGameSubsystem.h"
#include "DRCharacter.h"
#include "DRPlayerState.h"
#include "GameModes/LyraGameState.h"
#include "Subsystems/DRObjectPoolSubsystem.h"

#include "Character/LyraPawnData.h"
#include "DRPlayerController.h"
#include <GameFramework/PlayerStart.h>
#include <EngineUtils.h>
#include "GameFramework/GameplayMessageSubsystem.h"

#include "NativeGameplayTags.h"
#include "Messages/LyraVerbMessage.h"
#include <Kismet/GameplayStatics.h>
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Lyra_Elimination_Message, "Lyra.Elimination.Message");

void ADroneRacerGameMode::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	FString SpecificPlayerStartName = TEXT("");
	if (NewPlayer->PlayerState)
	{
		ADRPlayerState* DRPlayerState = Cast<ADRPlayerState>(NewPlayer->PlayerState);
		FDRPlaneConfig CurrentPlaneConfig;
		if (DRPlayerState && DRPlayerState->GetSelectedPlaneConfig(CurrentPlaneConfig))
		{
			SpecificPlayerStartName = CurrentPlaneConfig.PlayerStartTag;
		}
	}

	AActor* StartSpot = FindPlayerStart(NewPlayer, SpecificPlayerStartName);

	// If a start spot wasn't found,
	if (StartSpot == nullptr)
	{
		// Check for a previously assigned spot
		if (NewPlayer->StartSpot != nullptr)
		{
			StartSpot = NewPlayer->StartSpot.Get();
			UE_LOG(LogGameMode, Warning, TEXT("RestartPlayer: Player start not found, using last start spot"));
		}
	}

	RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}

const ULyraPawnData* ADroneRacerGameMode::GetPawnDataFromPlayerState_Implementation(const AController* InController) const
{
	// 2K Yanyi: Only override the Default Pawn Data if OverrideDefaultPawnData is checked
	if (InController != nullptr)
	{
		if (const ADRPlayerState* DRPlayerState = InController->GetPlayerState<ADRPlayerState>())
		{
			if (DRPlayerState->OverrideDefaultPawnData)
			{
				if (const ULyraPawnData* PawnData = DRPlayerState->GetPawnData<ULyraPawnData>())
				{
					return PawnData;
				}
			}
		}
	}

	return nullptr;
}

AActor* ADroneRacerGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// return Super::ChoosePlayerStart_Implementation(Player);

	if (AllStartPoints.Num() == 0)
	{
		UWorld* World = GetWorld();

		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			APlayerStart* PlayerStart = *It;

			AllStartPoints.Add(PlayerStart);
		}
	}

	// UE_LOG(LogTemp, Warning, TEXT("ChoosePlayerStart find %d players, and %d start points."), GameState->PlayerArray.Num(), AllStartPoints.Num());

	int32 MaxIndex = AllStartPoints.Num() - 1;
	int32 Index = FMath::Clamp(GameState->PlayerArray.Num() - 1, 0, MaxIndex);

	return AllStartPoints[Index];
}

void ADroneRacerGameMode::OnRegisterEnemy(FString EnemyName, TWeakObjectPtr<AActor> EnemyRef)
{
	// UE_LOG(LogTemp, Warning, TEXT("DroneRacerGameMode OnRegisterEnemy %s."), *EnemyName);
	AllEnemies.Emplace(EnemyName, EnemyRef);
}

void ADroneRacerGameMode::OnEliminateEnemy(AActor* InstigatorPawn, AActor* VictimPawn)
{
	if (VictimPawn && AllEnemies.Contains(VictimPawn->GetName()))
	{
		AllEnemies[VictimPawn->GetName()] = nullptr;
	}

	int32 LiveEnemiesCnt = 0;
	for (const TPair<FString, TWeakObjectPtr<AActor>>& Enemy : AllEnemies)
	{
		if (Enemy.Value.IsValid())
		{
			LiveEnemiesCnt++;
		}
	}

	// the ability avator may be character or player state
	if (ADRCharacter* DRCharacter = Cast<ADRCharacter>(InstigatorPawn))
	{
		if(ADRPlayerState* DRPlayerState = DRCharacter->GetPlayerState<ADRPlayerState>())
		{
			DRPlayerState->AddScores(5.0f);
		}
	}
	else if (ADRPlayerState* DRPlayerState = Cast<ADRPlayerState>(InstigatorPawn))
	{
		DRPlayerState->AddScores(5.0f);
	}

	UE_LOG(LogTemp, Warning, TEXT("OnEliminateEnemy %s, Live Enemies Cnt %d"), *VictimPawn->GetName(), LiveEnemiesCnt);
	if (LiveEnemiesCnt <= 0)
	{
		OnMatchEnd(true);
	}
}

void ADroneRacerGameMode::OnEliminatePlayer(AActor* InstigatorPawn, AActor* Player)
{
	// the ability avator may be character or player state
	if (ADRCharacter* DRCharacter = Cast<ADRCharacter>(InstigatorPawn))
	{
		if (ADRPlayerState* DRPlayerState = DRCharacter->GetPlayerState<ADRPlayerState>())
		{
			DRPlayerState->AddScores(100.0f);
		}
	}
	else if (ADRPlayerState* DRPlayerState = Cast<ADRPlayerState>(InstigatorPawn))
	{
		DRPlayerState->AddScores(100.0f);
	}
	OnMatchEnd(false);
}

void ADroneRacerGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UDRSaveGameSubsystem* DRSaveGameSubsystem = GetGameInstance()->GetSubsystem<UDRSaveGameSubsystem>();
	DRSaveGameSubsystem->LoadSaveGame();

	UDRObjectPoolSubsystem* DRObjectPoolSubsystem = GetWorld()->GetSubsystem<UDRObjectPoolSubsystem>();
	for (TSubclassOf<AActor> ObjectClass : PoolableObjectClass)
	{
		DRObjectPoolSubsystem->RegisterPoolableClass(ObjectClass, 5);
	}

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.RegisterListener(TAG_Lyra_Elimination_Message, this, &ThisClass::OnEliminationMessage);
}

void ADroneRacerGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// UE_LOG(LogTemp, Warning, TEXT("HandleStartingNewPlayer"));

	UDRSaveGameSubsystem* DRSaveGameSubsystem = GetGameInstance()->GetSubsystem<UDRSaveGameSubsystem>();
	if (DRSaveGameSubsystem)
	{
		DRSaveGameSubsystem->HandleStartingNewPlayer(NewPlayer);
	}
}

void ADroneRacerGameMode::OnMatchStart()
{
	ALyraGameState* LyraGS = GetGameState<ALyraGameState>();

	uint8 TeamID = 1;
	for (APlayerState* LyraPlayerState : LyraGS->PlayerArray)
	{
		ADRCharacter* Player = Cast<ADRCharacter>(LyraPlayerState->GetPawn());
		if (Player)
		{
			Player->OnMatchStart(TeamID);
			++TeamID;
			// UE_LOG(LogTemp, Warning, TEXT("Set Team ID %d to player %s"), TeamID, *Player->GetName());
		}
	}
}

// BattleResult: true means all enemy eliminated, false means one player eliminated
void ADroneRacerGameMode::OnMatchEnd(bool BattleResult)
{
	ALyraGameState* LyraGS = GetGameState<ALyraGameState>();

	//if(BattleResult)
	{ 
		ADRPlayerState* WinPlayer = nullptr;
		for (APlayerState* BasePlayerState : LyraGS->PlayerArray)
		{
			if(ADRPlayerState* DRPlayerState = Cast<ADRPlayerState>(BasePlayerState))
			{ 
				if (WinPlayer == nullptr || DRPlayerState->GetScore() > WinPlayer->GetScore())
				{
					WinPlayer = DRPlayerState;
				}
			}
		}

		// Fake: I add 100
		for (APlayerState* BasePlayerState : LyraGS->PlayerArray)
		{
			if (ADRPlayerState* DRPlayerState = Cast<ADRPlayerState>(BasePlayerState))
			{
				ADRCharacter* DRCharacter = DRPlayerState->GetPawn<ADRCharacter>();
				if (!DRCharacter)
				{
					continue;
				}
				if (DRPlayerState == WinPlayer)
				{
					DRCharacter->OnMatchEnd(true);
				}
				else
				{
					DRCharacter->OnMatchEnd(false);
				}
			}
		}
	}

	//if(BattleResult)
	//{ 
	//	UDRSaveGameSubsystem* DRSaveGameSubsystem = GetGameInstance()->GetSubsystem<UDRSaveGameSubsystem>();
	//	DRSaveGameSubsystem->WriteSaveGame();
	//}
}

void ADroneRacerGameMode::OnPlayerReady()
{
	bool AllPlayersReady = true;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ADRPlayerController* PC = Cast<ADRPlayerController>(*Iterator);
		if (PC == nullptr || !PC->GetIsPlayerReady())
		{
			AllPlayersReady = false;
			break;
		}
	}

	if (AllPlayersReady)
	{
		OnMatchStart();
	}
}

APlayerController* ADroneRacerGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	UE_LOG(LogTemp, Warning, TEXT("DroneRacerGameMode Login with Options %s."), *Options);
	if (UGameplayStatics::HasOption(Options, TEXT("PlaneName")))
	{
		FString PlaneName = UGameplayStatics::ParseOption(Options, TEXT("PlaneName"));

		if (ADRPlayerController* DRPlayerController = Cast<ADRPlayerController>(NewPlayerController))
		{
			DRPlayerController->SetClientSelectedPlane(PlaneName);
		}
	}

	return NewPlayerController;
}

void ADroneRacerGameMode::OnEliminationMessage(FGameplayTag Channel, const FLyraVerbMessage& Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("Received OnEliminationMessage"));

	AActor* InstigatorPawn = Cast<AActor>(Payload.Instigator);
	AActor* VictimPawn = Cast<AActor>(Payload.Target);

	if (VictimPawn && VictimPawn->ActorHasTag("Enemy"))
	{
		OnEliminateEnemy(InstigatorPawn, VictimPawn);
	}
	else if (VictimPawn && VictimPawn->ActorHasTag("Player"))
	{
		OnEliminatePlayer(InstigatorPawn, VictimPawn);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnEliminationMessage Unknow Type"));
	}
}
