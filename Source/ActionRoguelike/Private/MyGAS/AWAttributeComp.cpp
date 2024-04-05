// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\MyGAS/AWAttributeComp.h"

#include "AWGameModeBase.h"
#include "AWReward.h"
#include "GameFramework/PlayerState.h"
#include "AI/AWAICharacter.h"
#include "UObject/UnrealTypePrivate.h"

// Sets default values for this component's properties
UAWAttributeComp::UAWAttributeComp()
{
	// ...
	AttributeSet = CreateDefaultSubobject<UAwAttributeSet>(TEXT("AttributeSet"));
}


bool UAWAttributeComp::SetHealth(float v, AActor* Sourcer)
{
	// TODO: COVERT TO ATTRIBUTE SET
	if (v != 0.f && AttributeSet->GetHealth() >= 0)
	{
		float NewHP = AttributeSet->GetHealth() + v;
		NewHP = NewHP > AttributeSet->GetMaxHealth() ? AttributeSet->GetMaxHealth() : NewHP;
		NewHP = NewHP < 0 ? 0 : NewHP;
		if (NewHP == 0)
		{
			AAWGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AAWGameModeBase>();
			if (ensure(GameMode))
			{
				if(APlayerState* PS = Cast<APlayerState>(GetOuter()) )
				{

					//Player has PlayerState
					GameMode->ActorBeenKilled(PS->GetPawn(), Sourcer, nullptr);
				}
				else
				{
					//AIPlayer has not PlayerState but has AttributeComp
					AActor* Owner = GetOwner();
					UAWReward* Rewards = Cast<UAWReward>(Owner->GetComponentByClass(UAWReward::StaticClass()));
					GameMode->ActorBeenKilled(Owner, Sourcer, Rewards);
				}
			}
		}
		AttributeSet->SetHealth(NewHP);
		// trigger the event!
		OnHealthChange.Broadcast(Sourcer, this, AttributeSet->GetHealth(), v);
		return true;
	}
	return false;
}

bool UAWAttributeComp::isAlive() const
{
	return (AttributeSet->GetHealth() > 0.f && AttributeSet->GetMaxHealth() > 0.f);
}
