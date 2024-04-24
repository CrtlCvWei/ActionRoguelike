// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGAS/AwActionComponent.h"

#include "AwAction.h"
#include "AWPlayerState.h"
#include "AI/AWAICharacter.h"
#include "UI/AwSkillWidget.h"

// Sets default values for this component's properties
UAwActionComponent::UAwActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


void UAwActionComponent::ApplyInstanceEffects(UAwActionEffect* Effect, AActor* Insigator,
                                              UAWAttributeComp* AttributeComp)
{
	if (!AttributeComp)
		return;
	if (Effect->GetEffectMap().Num() > 0)
	{
		for (auto TPAIR : Effect->GetEffectMap())
		{
			FString NAME = TPAIR.Key;
			const auto VALUE = TPAIR.Value;
			// PRINT ON THE SCREEN
			if (VALUE == 0.f)
				return;
			if (NAME == "Health" && VALUE > 0.f)
				if (AttributeComp->GetHealth() == AttributeComp->GetMaxHealth())
					return;
			if (NAME == "Mana" && VALUE > 0.f)
				if (AttributeComp->GetMana() == AttributeComp->GetMaxMana())
					return;
			AttributeComp->SetAttributeBase(FName(*NAME), VALUE, Insigator);
		}
	}
	if (Effect->GetEffectMapForAction().Num() > 0)
	{
		for (auto TPAIR : Effect->GetEffectMapForAction())
		{
			auto& NAME = TPAIR.Key;
			const auto VALUE = TPAIR.Value;
			// TODO: Effect the action
		}
	}
}

void UAwActionComponent::ApplyPeriodicEffects(UAwActionEffect* Effect, AActor* Insigator,
                                              UAWAttributeComp* AttributeComp)
{
	GetWorld()->GetTimerManager().SetTimer(Effect->PeriodTimerHandle, [this,Effect,Insigator,AttributeComp ]()
	                                       {
		                                       // If the attribute component is not valid, remove the effect
		                                       if (!AttributeComp)
		                                       {
			                                       this->RemovePeriodicEffects(
				                                       Effect, Insigator, AttributeComp, Effect->DurationTimerHandle,
				                                       Effect->PeriodTimerHandle);
		                                       }

		                                       this->ApplyInstanceEffects(Effect, Insigator, AttributeComp);
	                                       }
	                                       , Effect->GetPeriod(), true);

	EffectsGamePlayTags.AppendTags(Effect->GetBuffTags());
	EffectObjectsPools.Add(Effect->GetEffectName(), FAwEffectRecorder(Insigator, AttributeComp, Effect));
	if (Effect->GetDuration() > 0)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RemovePeriodicEffects", Effect, Insigator, AttributeComp,
		                       Effect->DurationTimerHandle,
		                       Effect->PeriodTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(Effect->DurationTimerHandle, Delegate, Effect->GetDuration(), false);
	}
}

void UAwActionComponent::RemovePeriodicEffects(UAwActionEffect* Effect, AActor* Insigator,
                                               UAWAttributeComp* AttributeComp, FTimerHandle& DurationTimerHandle,
                                               FTimerHandle& PeriodTimerHandle)
{
	if (GetWorld()->GetTimerManager().GetTimerRemaining(PeriodTimerHandle) < KINDA_SMALL_NUMBER)
		ApplyInstanceEffects(Effect, Insigator, AttributeComp);
	GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(PeriodTimerHandle);
	EffectsGamePlayTags.RemoveTags(Effect->GetBuffTags());
	EffectObjectsPools.Remove(Effect->GetEffectName());
}

void UAwActionComponent::RemovePeriodicEffectsByForce(UAwActionEffect* Effect)
{
	if (EffectObjectsPools.Contains(Effect->GetEffectName()))
	{
		auto EffectContext = EffectObjectsPools[Effect->GetEffectName()];
		RemovePeriodicEffects(Effect, EffectContext.Insigator, EffectContext.AttributeComp, Effect->DurationTimerHandle,
		                      Effect->PeriodTimerHandle);
	}
}

void UAwActionComponent::ApplyDurationEffects(UAwActionEffect* Effect, AActor* Insigator,
	UAWAttributeComp* AttributeComp)
{

	if (!AttributeComp)
		return;
	if (Effect->GetEffectMap().Num() > 0)
	{
		for (auto TPAIR : Effect->GetEffectMap())
		{
			FString NAME = TPAIR.Key;
			const auto VALUE = TPAIR.Value;
			// PRINT ON THE SCREEN
			if (VALUE == 0.f)
				return;
			if (NAME == "Health" && VALUE > 0.f)
				if (AttributeComp->GetHealth() == AttributeComp->GetMaxHealth())
					return;
			if (NAME == "Mana" && VALUE > 0.f)
				if (AttributeComp->GetMana() == AttributeComp->GetMaxMana())
					return;
			AttributeComp->SetAttributeCurr(FName(*NAME), VALUE, Insigator);
		}
	}
	EffectsGamePlayTags.AppendTags(Effect->GetBuffTags());
	EffectObjectsPools.Add(Effect->GetEffectName(), FAwEffectRecorder(Insigator, AttributeComp, Effect));
	
	if (Effect->GetDuration() > 0)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RemoveDurationEffects", Effect, Insigator, AttributeComp,
							   Effect->DurationTimerHandle,
							   Effect->PeriodTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(Effect->DurationTimerHandle, Delegate, Effect->GetDuration(), false);
	}
}

void UAwActionComponent::RemoveDurationEffects(UAwActionEffect* Effect, AActor* Insigator,
	UAWAttributeComp* AttributeComp, FTimerHandle& DurationTimerHandle, FTimerHandle& PeriodTimerHandle)
{
	if (!AttributeComp)
		return;
	if (Effect->GetEffectMap().Num() > 0)
	{
		for (auto TPAIR : Effect->GetEffectMap())
		{
			FString NAME = TPAIR.Key;
			const auto VALUE = TPAIR.Value;
			// PRINT ON THE SCREEN
			if (VALUE == 0.f)
				return;
			if (NAME == "Health" && VALUE > 0.f)
				if (AttributeComp->GetHealth() == AttributeComp->GetMaxHealth())
					return;
			if (NAME == "Mana" && VALUE > 0.f)
				if (AttributeComp->GetMana() == AttributeComp->GetMaxMana())
					return;
			AttributeComp->SetAttributeCurr(FName(*NAME), -VALUE, Insigator);
		}
	}
	GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(PeriodTimerHandle);
	EffectsGamePlayTags.RemoveTags(Effect->GetBuffTags());
	EffectObjectsPools.Remove(Effect->GetEffectName());
}

// Called when the game starts
void UAwActionComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UAwActionComponent::SetOwningActor()
{
	ensure(GetOuter());
	AAWPlayerState* OwningPlayerState = Cast<AAWPlayerState>(GetOuter());
	if (OwningPlayerState)
	{
		OwningActor = OwningPlayerState->GetPawn();
		if (OwningActor)
			return true;
		return false;
	}

	if (AActor* Actor = Cast<AActor>(GetOuter()))
	{
		OwningActor = Actor;
		return true;
	}
	return false;
}

FAwGameplayEffectContextHandle UAwActionComponent::MakeEffectContex(AActor* Causer, UAwAction* Action)
{
	if (Causer == nullptr)
	{
		Causer = OwningActor;
	}
	FAwGameplayEffectContext* Context = new FAwGameplayEffectContext();
	Context->SetAbility(Action);
	FAwGameplayEffectContextHandle ContextHandle = FAwGameplayEffectContextHandle(Context);
	ContextHandle.AddInstigator(OwningActor, Causer);
	// By default use the owner and avatar as the instigator and causer
	return ContextHandle;
}

bool UAwActionComponent::BindCoolDownDelegate(FName ActionName, UAwUserWidget* Widget)
{
	for (UAwAction* Action : Actions)
	{
		if (Action && Action->GetActionName() == ActionName)
		{
			UAwSkillWidget* SkillWidget = Cast<UAwSkillWidget>(Widget);
			Action->GetCoolDownStartDelegate().AddDynamic(SkillWidget, &UAwSkillWidget::UICoolDownStart);
			Action->GetCoolDownEndDelegate().AddDynamic(SkillWidget, &UAwSkillWidget::UICoolDownEnd);
			return true;
		}
	}
	return false;
}

bool UAwActionComponent::AddAction(TSubclassOf<UAwAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return false;
	}

	UAwAction* Action = NewObject<UAwAction>(this, ActionClass);
	Action->CreateEffectInstances();
	if (ensure(Action))
	{
		if (Actions.Contains(Action) || Actions.Add(Action) >= 0)
		{
			if (Action->IsAuto())
			{
				if (!OwningActor)
					SetOwningActor();
				if (Action->CheckActionAvailable(OwningActor))
					Action->StartAction(OwningActor);
			}
			return true;
		}
	}
	return false;
}

bool UAwActionComponent::RemoveAction(UAwAction* ActionToRemove)
{
	if (ensure(ActionToRemove))
	{
		if (Actions.Num() == 0 || Actions.Find(ActionToRemove) == INDEX_NONE)
		{
			return false;
		}
		Actions.Remove(ActionToRemove);
		return true;
	}
	return false;
}

bool UAwActionComponent::ApplyEffect(const FAwGameplayEffectContext& EffectContext, UAWAttributeComp* AttributeComp)
{
	UAwAction* Skill = EffectContext.GetAbility();
	// Skill stores the info of effect
	if (!Skill)
		return false;

	for (auto& Effect : Skill->GetActionEffect())
	{
		// Apply the effect
		UAwActionEffect* Effect_INSTANCE = Effect.GetDefaultObject();
		// test

		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("The Damage is  %f"), Effect_INSTANCE->GetEffectMapValue("Health")));	
		
		if (Effect_INSTANCE->GetType() == DurationPolicy::Instant)
		{
			// Apply the effect instantly
			ApplyInstanceEffects(Effect_INSTANCE, EffectContext.GetInstigator(), AttributeComp);
		}
		else if (Effect_INSTANCE->GetType() == DurationPolicy::Duration)
		{
			// TODO:
			ApplyDurationEffects(Effect_INSTANCE, EffectContext.GetInstigator(), AttributeComp);
		}
		else if (Effect_INSTANCE->GetType() == DurationPolicy::infinite)
		{
			// TODO
		}
		else if (Effect_INSTANCE->GetType() == DurationPolicy::Periodic)
		{
			// Apply the effect periodically
			ApplyPeriodicEffects(Effect_INSTANCE, EffectContext.GetInstigator(), AttributeComp);
		}
	}
	return true;
}

void UAwActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (UAwAction* Action : Actions)
	{
		if (Action && Action->GetActionName() == ActionName)
		{
			if (Action->CheckActionAvailable(Instigator))
			{
				Action->StartAction(Instigator);
			}
		}
	}
}

void UAwActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (UAwAction* Action : Actions)
	{
		if (Action && Action->GetActionName() == ActionName)
		{
			Action->StopAction(Instigator);
		}
	}
}

UAwAction* UAwActionComponent::GetActionByName(FName ActionName)
{
	for (UAwAction* Action : Actions)
	{
		if (Action && Action->GetActionName() == ActionName)
		{
			return Action;
		}
	}
	return nullptr;
}


// Called every frame
void UAwActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Debug Code
	// if (ActiveGameplayTags.Num() != 0)
	// {
	// 	FString DebugMsg = GetNameSafe(GetOwner()) + ": " + ActiveGameplayTags.ToStringSimple();
	// 	FString DebugMsg2 = GetNameSafe(GetOwner()) + ": " + BlockGamePlayTags.ToStringSimple();
	// 	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Black, DebugMsg);
	// 	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, DebugMsg2);
	// }
}
