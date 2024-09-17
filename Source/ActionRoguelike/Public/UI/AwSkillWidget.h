// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/AwUserWidget.h"
#include "MyGAS/AwAction.h"
#include "AwSkillWidget.generated.h"

/**
 * 
 */
class UAwAction;

UCLASS()
class ACTIONROGUELIKE_API UAwSkillWidget : public UAwUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FOnAwActionCoolDownStartSignture OnCoolDownStart;
	UPROPERTY(BlueprintAssignable)
	FOnAwActionCoolDownEndSignture OnCoolDownEnd;
	

public:

	UFUNCTION()
	void UICoolDownStart(UAwAction* Action,float CoolDownTime);
	UFUNCTION()
	void UICoolDownEnd(UAwAction* Action);
	
	FOnAwActionCoolDownStartSignture& GetOnCoolDownStart() { return OnCoolDownStart; }
	FOnAwActionCoolDownEndSignture& GetOnCoolDownEnd() { return OnCoolDownEnd; }
};
