// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "GameFramework/GameModeBase.h"
#include "AWGameModeBase.generated.h"

class UAWReward;
class UEnvQueryInstanceBlueprintWrapper;
class UEnvQuery;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AAWGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:

	int SpawnMaxNum = 3;
	
protected:
	UPROPERTY(EditDefaultsOnly,Category="AI")
	float SpawnTimeInterval;
	
	FTimerHandle TimerHandle_SpawnBots;

	UPROPERTY(EditDefaultsOnly,Category="AI")
	TSubclassOf<AActor> MinionsClass;
	
	UPROPERTY(EditDefaultsOnly,Category="AI")
	UEnvQuery* SqawnBotQuery;

	UFUNCTION()
	void AfterQuery(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void SpawnBotTimerElasped();
	
	// UFUNCTION()
	// void PlayerDestoryTimeElasped(AActor* Death);
	UFUNCTION()
	void PlayerReSpawnTimeElasped(APlayerController* Controller);

public:

	UFUNCTION()
	void ActorBeenKilled(AActor* Death,AActor* Killer,UAWReward* Reward);
	
	AAWGameModeBase();
	virtual  void StartPlay() override;
};
