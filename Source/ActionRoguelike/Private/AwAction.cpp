// Fill out your copyright notice in the Description page of Project Settings.


#include "AwAction.h"

#include "ConstraintsManager.h"
#include "MyGAS/AwActionComponent.h"
#include "Blueprint/AwBlueprintFunctionLibrary.h"

void UAwAction::InitAtt()
{
	CostAttributeData = FAwAttributeData(1);
	CoolDownTimeAttributeData = FAwAttributeData(1);
}

UAwAction::UAwAction()
{
	bIsRunning = false;
	InitAtt();
	for (const auto EffectClass : EffectsClass)
	{
		UAwActionEffect* Effect = NewObject<UAwActionEffect>(this, EffectClass);
		if (Effect)
		{
			EffectInstances.Add(Effect);
		}
	}
}

void UAwAction::StartAction_Implementation(AActor* Instigator)
{
	// UE_LOG(LogTemp, Warning, TEXT("Running: %s"), *GetNameSafe(this));
	UAwActionComponent* Comp = GetOwningComponent();
	if (Comp)
	{
		if (!this->GrandTags.IsEmpty())
			Comp->ActiveGameplayTags.AppendTags(this->GrandTags);
		if (!this->BlockTags.IsEmpty())
			Comp->BlockGamePlayTags.AppendTags(this->BlockTags);
		// CoolDowning
		if (CoolDownTimeAttributeData.GetCurrentValue() > 0.f)
		{
			Comp->CoolDownGamePlayTags.AddTag(this->ActionTag);
			// CoolDown Over
			GetWorld()->GetTimerManager().SetTimer(CoolDownTimerHandle, this, &UAwAction::CoolDownOver,
			                                       CoolDownTimeAttributeData.GetCurrentValue(), false);
			OnCoolDownStart.Broadcast(this, CoolDownTimeAttributeData.GetCurrentValue());
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString("No Comp"));
	}
}

void UAwAction::StopAction_Implementation(AActor* Instigator)
{
	bIsRunning = false;
	UAwActionComponent* Comp = GetOwningComponent();
	if (ensure(Comp))
	{
		if (Comp->ActiveGameplayTags.HasAny(GrandTags))
			Comp->ActiveGameplayTags.RemoveTags(GrandTags);
		if (Comp->BlockGamePlayTags.HasAny(BlockTags))
			Comp->BlockGamePlayTags.RemoveTags(BlockTags);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString("No Comp"));
	}
}

bool UAwAction::CheckActionAvailable(AActor* Instigator) const
{
	UAwActionComponent* Comp = Cast<UAwActionComponent>(GetOuter());
	if (Comp->ActiveGameplayTags.HasAny(BlockTags))
	{
		// debug
		FString DebugMsg = FString("Block Actions : ") + ": " + BlockTags.ToStringSimple();
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, DebugMsg);
		return false;
	}
	else if (Comp->CoolDownGamePlayTags.HasTag(ActionTag))
	{
		return false;
	}
	UAWAttributeComp* Attribute = Cast<UAWAttributeComp>(GetOwningAttribute());
	if (ManaCost.GetCurrentValue() > 0)
	{
		if (Attribute && Attribute->GetMana() < ManaCost.GetCurrentValue())
			return false;
	}
	return true;
}

TArray<TSubclassOf<UAwActionEffect>>& UAwAction::GetActionEffect()
{
	// TODO: Implement this function later
	return EffectsClass;
}

void UAwAction::CreateEffectInstances()
{
	for(auto const EffectClass: EffectsClass)
	{
		UAwActionEffect* Effect = NewObject<UAwActionEffect>(this, EffectClass);
		if (Effect)
		{
			EffectInstances.Add(Effect);
		}
	}
}


UAwActionComponent* UAwAction::GetOwningComponent() const
{
	return Cast<UAwActionComponent>(GetOuter());
}

UAWAttributeComp* UAwAction::GetOwningAttribute() const
{
	return UAwBlueprintFunctionLibrary::GetAwAttributeComponent(GetOwningComponent()->GetOwningActor(), true);
}

FName UAwAction::GetActionName() const
{
	return this->ActionName;
}

void UAwAction::CoolDownOver()
{
	UAwActionComponent* Comp = GetOwningComponent();
	if (ensure(Comp))
	{
		Comp->CoolDownGamePlayTags.RemoveTag(this->ActionTag);
		OnCoolDownEnd.Broadcast(this);
	}
}

UWorld* UAwAction::GetWorld() const
{
	AActor* Actor = Cast<AActor>(GetOuter()->GetOuter());

	if (Actor)
	{
		return Actor->GetWorld();
	}
	return nullptr;
}

FGameplayTag UAwAction::GetActionTag() const
{
	return this->ActionTag;
}
