// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "AwAIBTService_CheckHealth.generated.h"

/**
 * 
 */
UCLASS()
class UAwAIBTService_CheckHealth : public UBTService
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere,Category="AI")
	float LowHealthThreshold = 0.3f;
	UPROPERTY(EditAnywhere,Category="AI")
	FBlackboardKeySelector isLowHealth;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
