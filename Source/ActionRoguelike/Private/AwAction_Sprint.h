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

	
public:
	UAwAction_Sprint(){};

	virtual void  StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;

	AActor* GetTheOwner() const;
};


