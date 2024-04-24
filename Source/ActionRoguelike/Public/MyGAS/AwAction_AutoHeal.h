// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AwAction.h"
#include "AwAction_AutoHeal.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API UAwAction_AutoHeal : public UAwAction
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<UAWAttributeComp> AttributeComp;

	UPROPERTY()
	TObjectPtr<UAwActionComponent> ActionComp;

	
public:
	UAwAction_AutoHeal();

	inline void CacheEffects();
	void StopAction_Implementation(AActor* Instigator) override;
	void StartAction_Implementation(AActor* Instigator) override;
	bool CheckActionAvailable(AActor* Instigator) const override;
};
