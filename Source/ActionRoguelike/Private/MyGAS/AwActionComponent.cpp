// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGAS/AwActionComponent.h"

#include "AwAction.h"
#include "AWPlayerState.h"
#include "UI/AwSkillWidget.h"

// Sets default values for this component's properties
UAwActionComponent::UAwActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


void UAwActionComponent::ApplyInstanceEffects(UAwActionEffect* Effect,AActor* Insigator)
{
	if (!OwningActor)
		return;
	if (AAwCharacter* owner = Cast<AAwCharacter>(OwningActor))
	{
		// find the att comp
		auto AttributeComp = owner->GetOwningAttribute();
		if (Effect->GetEffectMap().Num() > 0)
		{
			for (auto TPAIR : Effect->GetEffectMap())
			{
				auto& NAME = TPAIR.Key;
				const auto VALUE = TPAIR.Value;
				AttributeComp->SetAttribute(NAME, VALUE,Insigator);
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
}

// Called when the game starts
void UAwActionComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UAwActionComponent::SetOwningActor()
{
	AAWPlayerState* OwningPlayerState = Cast<AAWPlayerState>(GetOuter());
	if (OwningPlayerState)
	{
		OwningActor = OwningPlayerState->GetOwner();
		return true;
	}

	if (AActor* Actor = Cast<AActor>(GetOuter()))
	{
		OwningActor = Actor;
		return true;
	}

	return false;
}

FAwGameplayEffectContextHandle UAwActionComponent::MakeEffectContex(AActor* Causer)
{
	FAwGameplayEffectContext* Context = new FAwGameplayEffectContext();
	FAwGameplayEffectContextHandle ContextHandle = FAwGameplayEffectContextHandle(Context);
	ContextHandle.AddInstigator(OwningActor, Causer);
	// By default use the owner and avatar as the instigator and causer
	return ContextHandle;
}

bool UAwActionComponent::BindCoolDownDelegate(FName ActionName, UAwUserWidget* Widget)
{
	for (UAwAction* Action : Actions)
	{
		if(Action && Action->GetActionName() == ActionName)
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
			return true;
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

bool UAwActionComponent::ApplyEffect(const FAwGameplayEffectContext& EffectContext)
{
	UAwAction* Skill = EffectContext.GetAbility();
	// Skill stores the info of effect
	if (!Skill)
		return false;
	for (auto& Effect : Skill->GetActionEffect())
	{
		// Apply the effect
		UAwActionEffect* Effect_INSTANCE = NewObject<UAwActionEffect>(this, Effect);
		if (Effect_INSTANCE->GetType() == DurationPolicy::Instant)
		{
			// Apply the effect instantly
			ApplyInstanceEffects(Effect_INSTANCE,EffectContext.GetInstigator());
		}
		else if (Effect_INSTANCE->GetType() == DurationPolicy::Duration)
		{
			// TODO:
		}
		else if (Effect_INSTANCE->GetType() == DurationPolicy::infinite)
		{
			// TODO
		}

		delete Effect_INSTANCE;
	}
	return true;
}

void UAwActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (UAwAction* Action : Actions)
	{
		if (Action && Action->GetActionName() == ActionName)
		{
			if (!Action->CheckActionAvailable(Instigator))
			{
				continue;
			}
			Action->StartAction(Instigator);
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


// Called every frame
void UAwActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Debug Code
	if (ActiveGameplayTags.Num() != 0)
	{
		FString DebugMsg = GetNameSafe(GetOwner()) + ": " + ActiveGameplayTags.ToStringSimple();
		FString DebugMsg2 = GetNameSafe(GetOwner()) + ": " + BlockGamePlayTags.ToStringSimple();
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Black, DebugMsg);
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, DebugMsg2);
	}
}
