// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AWBTTask_RangedAttack.h"
#include "..\Public\AWAttributeComp.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UAWBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		ACharacter* MyPawn = Cast<ACharacter>(MyController->GetPawn());
		if (MyPawn)
		{
			FVector SpawnLocation = MyPawn->GetMesh()->GetSocketLocation("Muzzle_Front");
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
				FVector TargetLocation = Target->GetActorLocation();
				FRotator ProjectileRotation = (TargetLocation - SpawnLocation).Rotation();
				FTransform LocaTM = FTransform((FRotator)ProjectileRotation,
				                               (FVector)SpawnLocation);
				FActorSpawnParameters SpawnParams;
				/** Actor will spawn in desired location, regardless of collisions. */
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Instigator = MyPawn;
				// Spawn the projectile (Magic balls or any other things...)
				
				AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, LocaTM, SpawnParams);

				return Projectile ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
			}
		}
	}
	return EBTNodeResult::Failed;
}
