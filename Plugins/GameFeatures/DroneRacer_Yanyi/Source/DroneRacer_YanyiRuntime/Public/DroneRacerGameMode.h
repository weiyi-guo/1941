// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/LyraGameMode.h"
#include "DroneRacerGameMode.generated.h"

class ULyraPawnData;
class AController;
struct FGameplayTag;
struct FLyraVerbMessage;

/**
 * 
 */
UCLASS()
class DRONERACER_YANYIRUNTIME_API ADroneRacerGameMode : public ALyraGameMode
{
	GENERATED_BODY()

public:
	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual const ULyraPawnData* GetPawnDataFromPlayerState_Implementation(const AController* InController) const override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	void OnRegisterEnemy(FString EnemyName, TWeakObjectPtr<AActor> EnemyRef);

	UFUNCTION(BlueprintCallable)
	void OnEliminateEnemy(AActor* InstigatorPawn, AActor* VictimPawn);

	UFUNCTION(BlueprintCallable)
	void OnEliminatePlayer(AActor* InstigatorPawn, AActor* Player);

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable)
	void OnMatchStart();

	UFUNCTION(BlueprintCallable)
	void OnMatchEnd(bool BattleResult);

	UPROPERTY(EditDefaultsOnly, Category = "DroneRacer")
	TArray<TSubclassOf<AActor>> PoolableObjectClass;
	
	void OnPlayerReady();

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	
protected:
	TMap<FString, TWeakObjectPtr<AActor>> AllEnemies;

	UPROPERTY()
	TObjectPtr<class UDataAsset> PlaneSet;

	TArray<class APlayerStart*> AllStartPoints;

	void OnEliminationMessage(FGameplayTag Channel, const FLyraVerbMessage& Payload);
};
