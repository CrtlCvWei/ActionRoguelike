// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AWaiController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void AAWaiController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this,0);
	if (PlayerPawn)
	{
		// Get BlackBoard
		GetBlackboardComponent()->SetValueAsVector("MovetoLoc",PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsObject("TargetActor",PlayerPawn);
	}

}
