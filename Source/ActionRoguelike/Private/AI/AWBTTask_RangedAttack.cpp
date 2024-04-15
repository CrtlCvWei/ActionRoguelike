// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AWBTTask_RangedAttack.h"
#include "..\Public\MyGAS/AWAttributeComp.h"
#include "AIController.h"
#include "AI/AwAction_AIRangedAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "MyGAS/AwActionComponent.h"

EBTNodeResult::Type UAWBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		ACharacter* MyPawn = Cast<ACharacter>(MyController->GetPawn());
		if (MyPawn)
		{
			ACharacter* Target = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
			if (Target)
			{
				UAWAttributeComp* TargetAttribute = Cast<UAWAttributeComp>(Target->GetComponentByClass(UAWAttributeComp::StaticClass())); 
				if(TargetAttribute)
				{
					if (!TargetAttribute->isAlive())
					{
						return EBTNodeResult::Failed;
					}
				}
				auto ActionComp = Cast<UAwActionComponent>(MyPawn->GetComponentByClass(UAwActionComponent::StaticClass()));
				if (ActionComp)
				{
					if(ActionComp->GetActionByName("Attack"))
					{
						auto AttackAction = Cast<UAwAction_AIRangedAttack>(ActionComp->GetActionByName("Attack")) ;
						AttackAction->SetTarget(Target);
						ActionComp->StartActionByName(MyPawn, "Attack");
						return EBTNodeResult::Succeeded;
					}
				}
				return EBTNodeResult::Failed;
			}
		}
	}
	return EBTNodeResult::Failed;
}
