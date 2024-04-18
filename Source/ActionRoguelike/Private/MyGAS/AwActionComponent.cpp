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


inline  void UAwActionComponent::ApplyInstanceEffects(UAwActionEffect* Effect, AActor* Insigator,
                                              UAWAttributeComp* AttributeComp)
{
	if (Effect->GetEffectMap().Num() > 0)
	{
		for (auto TPAIR : Effect->GetEffectMap())
		{
			FString NAME = TPAIR.Key;
			const auto VALUE = TPAIR.Value;
			// PRINT ON THE SCREEN
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
	if (ensure(Action))
	{
		if (Actions.Add(Action) >= 0)
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
		if (Effect_INSTANCE->GetType() == DurationPolicy::Instant)
		{
			// Apply the effect instantly
			ApplyInstanceEffects(Effect_INSTANCE, EffectContext.GetInstigator(), AttributeComp);
		}
		else if (Effect_INSTANCE->GetType() == DurationPolicy::Duration)
		{
			// TODO:
		}
		else if (Effect_INSTANCE->GetType() == DurationPolicy::infinite)
		{
			// TODO
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
