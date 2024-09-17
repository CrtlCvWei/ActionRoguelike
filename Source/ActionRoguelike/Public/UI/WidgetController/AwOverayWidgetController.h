// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AwWidgetController.h"
#include "AwOverayWidgetController.generated.h"

/**
 * 
 */


UCLASS()
class ACTIONROGUELIKE_API UAwOverayWidgetController : public UAwWidgetController
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintAssignable)
	FHealthChangeUISignture OnHealthBaseFChangeorUI;
	UPROPERTY(BlueprintAssignable)
	FHealthChangeUISignture OnHealthCurrFChangeorUI;

	UPROPERTY(BlueprintAssignable)
	FMaxHealthChangeUISignture OnMaxHealthBaseChangeForUI;
	UPROPERTY(BlueprintAssignable)
	FMaxHealthChangeUISignture OnMaxHealthCurrChangeForUI;

	UPROPERTY(BlueprintAssignable)
	FManaChangeUISignture OnManaBaseChangeForUI;
	UPROPERTY(BlueprintAssignable)
	FManaChangeUISignture OnManaCurrChangeForUI;

	UPROPERTY(BlueprintAssignable)
	FMaxManaChangeUISignture OnMaxManaBaseChangeForUI;
	UPROPERTY(BlueprintAssignable)
	FMaxManaChangeUISignture OnMaxManaCurrChangeForUI;


	UFUNCTION()
	// void HealthBaseChangeForUI(float NewVal, float OldVal) const;
	void HealthBaseChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change);
	UFUNCTION()
	void HealthCurrChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change);
	UFUNCTION()
	void MaxHealthBaseChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change);
	UFUNCTION()
	void MaxHealthCurrChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change) ;
	UFUNCTION()
	void ManaBaseChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change) ;
	UFUNCTION()
	void ManaCurrChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change) ;
	UFUNCTION()
	void MaxManaBaseChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change);
	UFUNCTION()
	void MaxManaCurrChangeForUI(AActor* InstigatorActor, UAWAttributeComp* AttributeComp, float NewVal, float Change);

public:
	virtual void BindCallBacksToDependencies() override;
	virtual void BroadcastInitVals() override;
};
