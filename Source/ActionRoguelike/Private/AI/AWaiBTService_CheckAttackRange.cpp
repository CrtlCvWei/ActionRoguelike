// Fill out your copyright notice in the Description page of Project Settings.


#include "AWaiBTService_CheckAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

void UAWaiBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// check dis between this and actor
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComp))
	{
		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
		if (TargetActor != nullptr)
		{
			AAIController* MyController = OwnerComp.GetAIOwner();
			if (MyController)
			{
				APawn* MyPawn = Cast<APawn>(MyController->GetPawn());
				const float Dist = FVector::Distance(TargetActor->GetActorLocation(), MyPawn->GetActorLocation());
				bool bWithIN = Dist < 1000.f;
				bool bOut = Dist > 2000.f;
				bool bLineofSight = false;
				if (bWithIN)
				{
					bLineofSight = MyController->LineOfSightTo(TargetActor);
				}
				BlackboardComp->SetValueAsBool(WithInAttackRange.SelectedKeyName, bWithIN);
				BlackboardComp->SetValueAsBool(OutOfRange.SelectedKeyName, bOut);
				BlackboardComp->SetValueAsBool(LineOfSight.SelectedKeyName, bLineofSight);
				//默认未看见
				BlackboardComp->SetValueAsBool(Seen.SelectedKeyName, false);
			}
		}
		else
		{
			BlackboardComp->SetValueAsBool(WithInAttackRange.SelectedKeyName, false);
			BlackboardComp->SetValueAsBool(OutOfRange.SelectedKeyName, true);
			BlackboardComp->SetValueAsBool(LineOfSight.SelectedKeyName, false);
			
		}
	}
}
