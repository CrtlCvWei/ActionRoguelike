// Fill out your copyright notice in the Description page of Project Settings.


#include "AWGameModeBase.h"

#include "..\Public\MyGAS/AWAttributeComp.h"
#include "AwCharacter.h"
#include "AWPlayerState.h"
#include "AWReward.h"
#include "EngineUtils.h"
#include "AI/AWAICharacter.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"

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
	FVector Up(0.f,0.f,90.f);
	if (Location.Num() > 0)
	{
		GetWorld()->SpawnActor<AActor>(MinionsClass, Location[0]+Up, FRotator::ZeroRotator);
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
		this, SqawnBotQuery, this, EEnvQueryRunMode::RandomBest25Pct, nullptr);
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
		auto PS =  Controller->GetPlayerState<AAWPlayerState>();
		if (PS)
		{
			auto Attribute = PS->GetPlayerAttribute();
			if (Attribute)
			{
				Attribute->SetHealth(Attribute->GetMaxHealth(), nullptr);
			}
		}
	}
}

void AAWGameModeBase::ActorBeenKilled(AActor* Death, AActor* Killer, UAWReward* Reward)
{
	AAwCharacter* Player = Cast<AAwCharacter>(Death);
	if (Player)
	{
		// if the player die... And the Reward is None

		APlayerController* PC = Cast<APlayerController>(Player->GetController());

		if (ensure(PC) && !GetWorld()->GetTimerManager().IsTimerActive(ReSpawnTimerHandle))
		{
			Delegate.BindUFunction(this, "PlayerReSpawnTimeElasped", PC);
			GetWorld()->GetTimerManager().SetTimer(ReSpawnTimerHandle, Delegate, 3.f, false);
		}
	}
	else
	{
		AAWAICharacter* AIPlayer = Cast<AAWAICharacter>(Death);
		if (AIPlayer)
		{
			// if the AI die...  Rewards the killer
			AAwCharacter* KillerPlayer = Cast<AAwCharacter>(Killer);
			if (KillerPlayer)
			{
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
			else
			{
				// not killed by player
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("A Actor %s was killed by %s "), *GetNameSafe(Death), *GetNameSafe(Killer));
}
