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
	
	virtual void BroadcastInitVals() override;
	virtual void BindCallBacksToDependencies() override;
protected:

	UPROPERTY(BlueprintAssignable)
	FHealthChangeUISignture OnHealthChangeForUI;

	UPROPERTY(BlueprintAssignable)
	FMaxHealthChangeUISignture OnMaxHealthChangeForUI;
	
	UPROPERTY(BlueprintAssignable)
	FManaChangeUISignture OnManaChangeForUI;

	UPROPERTY(BlueprintAssignable)
	FMaxManaChangeUISignture OnMaxManaChangeForUI;
	
	
	void HealthChangeForUI(float NewVal,float OldVal) const;
	void MaxHealthChangeForUI(float NewVal,float OldVal) const;
};
