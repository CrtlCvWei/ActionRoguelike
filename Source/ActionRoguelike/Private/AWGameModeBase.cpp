// Fill out your copyright notice in the Description page of Project Settings.


#include "AWGameModeBase.h"

#include "AWAttributeComp.h"
#include "AwCharacter.h"
#include "AWPlayerState.h"
#include "AWReward.h"
#include "EngineUtils.h"
#include "K2Node_SpawnActorFromClass.h"
#include "AI/AWAICharacter.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

AAWGameModeBase::AAWGameModeBase()
{
	SpawnTimeInterval = 5.f;
	SpawnMaxNum = 3;
	PlayerStateClass = AAWPlayerState::StaticClass();
}

void AAWGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnBots, this, &AAWGameModeBase::SpawnBotTimerElasped,
	                                       SpawnTimeInterval, true);
}

void AAWGameModeBase::AfterQuery(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (!QueryInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("QueryInstance is null!"));
		return;
	}
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("QueryStatus is not Successful!"));
		return;
	}

	TArray<FVector> Location;
	QueryInstance->GetQueryResultsAsLocations(Location);
	if (Location.Num() > 0)
	{
		GetWorld()->SpawnActor<AActor>(MinionsClass, Location[0], FRotator::ZeroRotator);
	}
}

void AAWGameModeBase::SpawnBotTimerElasped()
{
	int32 CurrentAliveAI = 0;
	for (TActorIterator<AAWAICharacter> AIIt(GetWorld()); AIIt; ++AIIt)
	{
		AAWAICharacter* Bot = *AIIt;
		UAWAttributeComp* Attribute = Cast<UAWAttributeComp>(Bot->GetComponentByClass(UAWAttributeComp::StaticClass()));
		if (Attribute && Attribute->isAlive())
		{
			CurrentAliveAI++;
		}
	}
	if (CurrentAliveAI >= SpawnMaxNum)
	{
		return;
	}

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this, SqawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AAWGameModeBase::AfterQuery);
	}
}

void AAWGameModeBase::PlayerReSpawnTimeElasped(APlayerController* Controller)
{
	if (ensure(Controller))
	{
		APawn* Old = Controller->GetPawn();
		Controller->UnPossess();
		RestartPlayer(Controller);
		Old->Destroy();
	}
}

// void AAWGameModeBase::PlayerDestoryTimeElasped(AActor* Death)
// {
// 	if (ensureAlways(Death))
// 	{
// 		Death->Destroy();
// 	}
// }

void AAWGameModeBase::ActorBeenKilled(AActor* Death, AActor* Killer, UAWReward* Reward)
{
	AAwCharacter* Player = Cast<AAwCharacter>(Death);
	if (Player)
	{
		// if the player die... And the Reward is None
		FTimerHandle ReSpawnTimerHandle;
		FTimerDelegate Delegate_1;
		APlayerController* PC = Cast<APlayerController>(Player->GetController());

		if (ensure(PC))
		{
			Delegate_1.BindUFunction(this, "PlayerReSpawnTimeElasped", PC);
			GetWorld()->GetTimerManager().SetTimer(ReSpawnTimerHandle, Delegate_1, 3.f, false);
		}
	}
	else
	{
		AAWAICharacter* AIPlayer = Cast<AAWAICharacter>(Death);
		if (AIPlayer)
		{
			// if the AI die...  Rewards the killer
			AAwCharacter* KillerPlayer = Cast<AAwCharacter>(Killer);
			if (ensure(Reward))
			{
				AAWPlayerState* State = KillerPlayer->GetPlayerState<AAWPlayerState>();
				if (ensure(State))
				{
					// not Credits Now~
					// State->AddCredits()

					State->AddScores(Reward->GetScore());
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("A Actor %s was killed by %s "), *GetNameSafe(Death), *GetNameSafe(Killer));
}
