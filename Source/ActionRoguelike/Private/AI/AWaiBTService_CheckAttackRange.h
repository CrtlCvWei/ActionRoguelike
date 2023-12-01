// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "AWaiBTService_CheckAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class UAWaiBTService_CheckAttackRange : public UBTService
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere,Category="AI")
	FBlackboardKeySelector WithInAttackRange;

	UPROPERTY(EditAnywhere,Category="AI")
	FBlackboardKeySelector OutOfRange;

	UPROPERTY(EditAnywhere,Category="AI")
	FBlackboardKeySelector LineOfSight;

	UPROPERTY(EditAnywhere,Category="AI")
	FBlackboardKeySelector Seen;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
