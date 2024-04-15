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
	void HealthBaseChangeForUI(float NewVal, float OldVal) const;
	UFUNCTION()
	void HealthCurrChangeForUI(float NewVal, float OldVal) const;
	UFUNCTION()
	void MaxHealthBaseChangeForUI(float NewVal, float OldVal) const;
	UFUNCTION()
	void MaxHealthCurrChangeForUI(float NewVal, float OldVal) const;
	UFUNCTION()
	void ManaBaseChangeForUI(float NewVal, float OldVal) const;
	UFUNCTION()
	void ManaCurrChangeForUI(float NewVal, float OldVal) const;
	UFUNCTION()
	void MaxManaBaseChangeForUI(float NewVal, float OldVal) const;
	UFUNCTION()
	void MaxManaCurrChangeForUI(float NewVal, float OldVal) const;

public:
	virtual void BindCallBacksToDependencies() override;
	virtual void BroadcastInitVals() override;
};
