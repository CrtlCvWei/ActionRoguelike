// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGAS/AwAction_AutoHeal.h"
#include "Blueprint/AwBlueprintFunctionLibrary.h"

UAwAction_AutoHeal::UAwAction_AutoHeal()
{
	bAuto = true;
	HealPeriod = 0.1f;
	CoolDownTimeAttributeData = FAwAttributeData(0);
}

void UAwAction_AutoHeal::CacheEffects()
{
	for (auto& Effect : Effects)
	{
		UAwActionEffect* Effect_INSTANCE = Effect.GetDefaultObject();
		if (Effect_INSTANCE->GetType() == DurationPolicy::Instant)
		{
			for (const auto& Pair : Effect_INSTANCE->GetEffectMap())
			{
				if (EffectCache.Find(FName(Pair.Key)) == nullptr)
					EffectCache.Add(FName(Pair.Key), Pair.Value);
				else
				{
					EffectCache[FName(Pair.Key)] += Pair.Value;
				}
			}
		}
	}
}

void UAwAction_AutoHeal::HealPerTick(AActor* Instigator)
{

	for (auto& Pair : EffectCache)
	{
		if(Pair.Key == "Health" && AttributeComp->GetHealth() == AttributeComp->GetMaxHealth())
			return;
		else if(Pair.Key == "Mana" && AttributeComp->GetMana() == AttributeComp->GetMaxMana())
			return;
		AttributeComp->SetAttributeBase(Pair.Key, Pair.Value, Instigator);
	}
}


void UAwAction_AutoHeal::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
	GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
}

bool UAwAction_AutoHeal::CheckActionAvailable(AActor* Instigator) const
{
	return Super::CheckActionAvailable(Instigator);
}

void UAwAction_AutoHeal::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	AttributeComp = UAwBlueprintFunctionLibrary::GetAwAttributeComponent(Instigator);
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "HealPerTick", Instigator);
	if (ensure(GetWorld()) && Effects.Num() > 0 && AttributeComp)
	{
		CacheEffects();
		GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, Delegate, HealPeriod, true);
	}
}
