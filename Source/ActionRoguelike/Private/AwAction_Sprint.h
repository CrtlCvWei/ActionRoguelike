// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AwAction.h"
#include "AwAction_Sprint.generated.h"

/**
 * 
 */
UCLASS()
class UAwAction_Sprint : public UAwAction
{
	GENERATED_BODY()
private:

	bool bIsRunning = false;
	
	UPROPERTY(EditAnywhere,Blueprintable,Category="Attribute")
	float MaxSpeed = 800.f;
	UPROPERTY(EditAnywhere,Blueprintable,Category="Attribute")
	float NormalMoveSpeed = 400.f;
	
public:
	UAwAction_Sprint(){};
	UAwAction_Sprint(float v):MaxSpeed(v){};

	virtual void  StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
};
