// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGAS/AwAction_AutoHeal.h"
#include "Blueprint/AwBlueprintFunctionLibrary.h"

UAwAction_AutoHeal::UAwAction_AutoHeal()
{
	bAuto = true;
	CoolDownTimeAttributeData = FAwAttributeData(0);
}

void UAwAction_AutoHeal::StopAction_Implementation(AActor* Instigator)
{
	ActionComp = GetOwningComponent();
	if (!ensure(ActionComp))
		return;
	if (EffectsClass.Num() > 0)
	{
		for (auto Effect : EffectsClass)
		{
			auto Effect_Instance = Effect.GetDefaultObject();
			if (Effect_Instance->GetType() == DurationPolicy::Periodic)
			{
				ActionComp->RemovePeriodicEffectsByForce(Effect_Instance);
			}
		}
	}
	Super::StopAction_Implementation(Instigator);
}

bool UAwAction_AutoHeal::CheckActionAvailable(AActor* Instigator) const
{
	return Super::CheckActionAvailable(Instigator);
}

void UAwAction_AutoHeal::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	AttributeComp = UAwBlueprintFunctionLibrary::GetAwAttributeComponent(Instigator);
	ActionComp = GetOwningComponent();
	if (!ensure(ActionComp))
		return;
	if (EffectsClass.Num() > 0)
	{
		for (auto Effect : EffectsClass)
		{
			auto Effect_Instance = Effect.GetDefaultObject();
			if (Effect_Instance->GetType() == DurationPolicy::Periodic)
			{
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Effect_Instance, Instigator]()
				{
					ActionComp->ApplyPeriodicEffects(Effect_Instance, Instigator, AttributeComp);
				}, 0.1f, false);
			}
			if (Effect_Instance->GetType() == DurationPolicy::Duration)
			{
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Effect_Instance, Instigator]()
				{
					ActionComp->ApplyDurationEffects(Effect_Instance, Instigator, AttributeComp);
				}, 0.1f, false);
			
			}
		}
	}
}
