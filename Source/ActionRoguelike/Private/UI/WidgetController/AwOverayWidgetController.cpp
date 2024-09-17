// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AwOverayWidgetController.h"

#define ATTRIBUTECHANGEBIND(COMP,NAME)\
	COMP->AttributeChangeBindBase(#NAME, this, &UAwOverayWidgetController::NAME##BaseChangeForUI,"&UAwOverayWidgetController::"#NAME"BaseChangeForUI");\
	COMP->AttributeChangeBindCurr(#NAME, this, &UAwOverayWidgetController::NAME##CurrChangeForUI,"&UAwOverayWidgetController::"#NAME"CurrChangeForUI");
	

void UAwOverayWidgetController::BroadcastInitVals() 
{
	UAWAttributeComp* AC  = AwAttributeComp.Get();
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


void UAwOverayWidgetController::HealthBaseChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change)
{
	OnHealthBaseFChangeorUI.Broadcast(NewVal, NewVal-Change);
}

void UAwOverayWidgetController::HealthCurrChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change) 
{
	OnHealthCurrFChangeorUI.Broadcast(NewVal, NewVal-Change);
}

void UAwOverayWidgetController::MaxHealthBaseChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change) 
{
	OnMaxHealthBaseChangeForUI.Broadcast(NewVal, NewVal-Change);
}

void UAwOverayWidgetController::MaxHealthCurrChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change) 
{
	OnMaxHealthCurrChangeForUI.Broadcast(NewVal, NewVal-Change);
}

void UAwOverayWidgetController::ManaBaseChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change) 
{
	OnManaBaseChangeForUI.Broadcast(NewVal, NewVal-Change);
}

void UAwOverayWidgetController::ManaCurrChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change)
{
	OnManaCurrChangeForUI.Broadcast(NewVal, NewVal-Change);
}

void UAwOverayWidgetController::MaxManaBaseChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change)
{
	OnMaxManaBaseChangeForUI.Broadcast(NewVal, NewVal-Change);
}

void UAwOverayWidgetController::MaxManaCurrChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change)
{
	OnMaxManaCurrChangeForUI.Broadcast(NewVal, NewVal-Change);
}

void UAwOverayWidgetController::BindCallBacksToDependencies()
{
	UAWAttributeComp* AC  = AwAttributeComp.Get();
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
	ATTRIBUTECHANGEBIND(AC, Health);
	ATTRIBUTECHANGEBIND(AC, MaxHealth);
	ATTRIBUTECHANGEBIND(AC, Mana);
	ATTRIBUTECHANGEBIND(AC, MaxMana);

	// AttributeSet->GetAttributeChangeDelegate(FName("MaxMana"),Current).AddUObject(this, &UAwOverayWidgetController::MaxManaCurrChangeForUI);
}


