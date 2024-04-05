// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AwOverayWidgetController.h"

void UAwOverayWidgetController::BroadcastInitVals() 
{
	UAWAttributeComp* AC  = AttributeComp.Get();
	if (!AC)
	{
		UE_LOG(LogTemp, Error, TEXT("AttributeComp is nullptr"));
		return;
	}

	UAwAttributeSet* AttributeSet = AC->GetAttributeSet();
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("AttributeSet is nullptr"));
		return;
	}
	OnHealthChangeForUI.Broadcast(AttributeSet->GetHealth(), AttributeSet->GetHealth());
	OnMaxHealthChangeForUI.Broadcast(AttributeSet->GetMaxHealth(), AttributeSet->GetMaxHealth());
	OnManaChangeForUI.Broadcast(AttributeSet->GetMana(), AttributeSet->GetMana());
	OnMaxManaChangeForUI.Broadcast(AttributeSet->GetMaxMana(), AttributeSet->GetMaxMana());
}

void UAwOverayWidgetController::BindCallBacksToDependencies()
{
	UAWAttributeComp* AC  = AttributeComp.Get();
	if (!AC)
	{
		UE_LOG(LogTemp, Error, TEXT("AttributeComp is nullptr"));
		return;
	}

	UAwAttributeSet* AttributeSet = AC->GetAttributeSet();
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("AttributeSet is nullptr"));
		return;
	}

	AttributeSet->GetAttributeChangeDelegate(FName("Health"),Current).AddUObject(this, &UAwOverayWidgetController::HealthChangeForUI);
	AttributeSet->GetAttributeChangeDelegate(FName("MaxHealth"),Current).AddUObject(this, &UAwOverayWidgetController::MaxHealthChangeForUI);
}

void UAwOverayWidgetController::HealthChangeForUI(float NewVal, float OldVal) const
{
	OnHealthChangeForUI.Broadcast(NewVal, OldVal);
}

void UAwOverayWidgetController::MaxHealthChangeForUI(float NewVal, float OldVal) const
{
	OnMaxHealthChangeForUI.Broadcast(NewVal, OldVal);
}

