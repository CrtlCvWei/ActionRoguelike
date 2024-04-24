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
	OnHealthBaseFChangeorUI.Broadcast(AttributeSet->GetHealthBase(), AttributeSet->GetHealthBase());
	OnMaxHealthBaseChangeForUI.Broadcast(AttributeSet->GetMaxHealthBase(), AttributeSet->GetMaxHealthBase());
	OnManaBaseChangeForUI.Broadcast(AttributeSet->GetManaBase(), AttributeSet->GetManaBase());
	OnMaxManaBaseChangeForUI.Broadcast(AttributeSet->GetMaxManaBase(), AttributeSet->GetMaxManaBase());
}


void UAwOverayWidgetController::HealthBaseChangeForUI(float NewVal, float OldVal) const
{
	OnHealthBaseFChangeorUI.Broadcast(NewVal, OldVal);
}

void UAwOverayWidgetController::HealthCurrChangeForUI(float NewVal, float OldVal) const
{
	OnHealthCurrFChangeorUI.Broadcast(NewVal, OldVal);
}

void UAwOverayWidgetController::MaxHealthBaseChangeForUI(float NewVal, float OldVal) const
{
	OnMaxHealthBaseChangeForUI.Broadcast(NewVal, OldVal);
}

void UAwOverayWidgetController::MaxHealthCurrChangeForUI(float NewVal, float OldVal) const
{
	OnMaxHealthCurrChangeForUI.Broadcast(NewVal, OldVal);
}

void UAwOverayWidgetController::ManaBaseChangeForUI(float NewVal, float OldVal) const
{
	OnManaBaseChangeForUI.Broadcast(NewVal, OldVal);
}

void UAwOverayWidgetController::ManaCurrChangeForUI(float NewVal, float OldVal) const
{
	OnManaCurrChangeForUI.Broadcast(NewVal, OldVal);
}

void UAwOverayWidgetController::MaxManaBaseChangeForUI(float NewVal, float OldVal) const
{
	OnMaxManaBaseChangeForUI.Broadcast(NewVal, OldVal);
}

void UAwOverayWidgetController::MaxManaCurrChangeForUI(float NewVal, float OldVal) const
{
	OnMaxManaCurrChangeForUI.Broadcast(NewVal, OldVal);
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

	// My Blinding to the Attribute Set
	AttributeSet->GetAttributeChangeDelegate(FName("Health"),Base).AddUObject(this, &UAwOverayWidgetController::HealthBaseChangeForUI);
	AttributeSet->GetAttributeChangeDelegate(FName("MaxHealth"),Base).AddUObject(this, &UAwOverayWidgetController::MaxHealthBaseChangeForUI);
	AttributeSet->GetAttributeChangeDelegate(FName("Mana"),Base).AddUObject(this, &UAwOverayWidgetController::ManaBaseChangeForUI);
	AttributeSet->GetAttributeChangeDelegate(FName("MaxMana"),Base).AddUObject(this, &UAwOverayWidgetController::MaxManaBaseChangeForUI);

	AttributeSet->GetAttributeChangeDelegate(FName("Health"),Current).AddUObject(this, &UAwOverayWidgetController::HealthCurrChangeForUI);
	AttributeSet->GetAttributeChangeDelegate(FName("MaxHealth"),Current).AddUObject(this, &UAwOverayWidgetController::MaxHealthCurrChangeForUI);
	AttributeSet->GetAttributeChangeDelegate(FName("Mana"),Current).AddUObject(this, &UAwOverayWidgetController::ManaCurrChangeForUI);
	AttributeSet->GetAttributeChangeDelegate(FName("MaxMana"),Current).AddUObject(this, &UAwOverayWidgetController::MaxManaCurrChangeForUI);
}


