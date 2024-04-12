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
	FHealthChangeUISignture OnHealthChangeForUI;

	UPROPERTY(BlueprintAssignable)
	FMaxHealthChangeUISignture OnMaxHealthChangeForUI;
	
	UPROPERTY(BlueprintAssignable)
	FManaChangeUISignture OnManaChangeForUI;

	UPROPERTY(BlueprintAssignable)
	FMaxManaChangeUISignture OnMaxManaChangeForUI;


	UFUNCTION()
	void HealthChangeForUI(float NewVal,float OldVal) const;
	UFUNCTION()
	void MaxHealthChangeForUI(float NewVal,float OldVal) const;
	UFUNCTION()
	void ManaChangeForUI(float NewVal,float OldVal) const;
	UFUNCTION()
	void MaxManaChangeForUI(float NewVal,float OldVal) const;
	
public:
	virtual void BindCallBacksToDependencies() override;
	virtual void BroadcastInitVals() override;
};
