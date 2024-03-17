// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\AWAttributeComp.h"

#include "AWGameModeBase.h"
#include "AWReward.h"
#include "AI/AWAICharacter.h"

// Sets default values for this component's properties
UAWAttributeComp::UAWAttributeComp()
{
	// ...
	Max_Health = 100.f;
	Health = 100.f;
}

bool UAWAttributeComp::SetHealth(float v, AActor* Sourcer)
{
	if (v != 0.f && Health > 0)
	{
		this->Health += v;
		Health = Health > 100 ? 100 : Health;
		Health = Health < 0 ? 0 : Health;
		// trigger the event!
		OnHealthChange.Broadcast(Sourcer, this, Health, v);
		if (Health == 0)
		{
			AAWGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AAWGameModeBase>();
			if (ensure(GameMode))
			{
				AActor* Owner = GetOwner();
				UAWReward* Rewards = Cast<UAWReward>(Owner->GetComponentByClass(UAWReward::StaticClass()));
				if (Rewards)
				{
					//Player has  Reward Comp
					GameMode->ActorBeenKilled(Owner, Sourcer, Rewards);
				}
				else
				{
					//Player has not Reward Comp
					GameMode->ActorBeenKilled(Owner, Sourcer, nullptr);
				}
			}
		}
		return true;
	}
	return false;
}

bool UAWAttributeComp::isAlive() const
{
	return (Health > 0.f && Max_Health > 0.f);
}
