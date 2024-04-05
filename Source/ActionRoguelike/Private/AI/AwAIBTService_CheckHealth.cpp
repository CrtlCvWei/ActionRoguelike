// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AwAIBTService_CheckHealth.h"

#include "AIController.h"
#include "..\Public\MyGAS/AWAttributeComp.h"
#include "AI/AWAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

void UAwAIBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure((MyController)))
	{
		AAWAICharacter* MyPawn = Cast<AAWAICharacter>(MyController->GetPawn());
		if (MyPawn)
		{
			UAWAttributeComp* Attribute = Cast<UAWAttributeComp>(
				MyPawn->GetComponentByClass(UAWAttributeComp::StaticClass()));
			if (Attribute)
			{
				float roit = Attribute->GetHealth() / Attribute->GetMaxHealth();
				UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
				if (ensure(Blackboard))
				{
					Blackboard->SetValueAsBool(isLowHealth.SelectedKeyName, roit <= LowHealthThreshold);
				}
			}
		}
	}
}
