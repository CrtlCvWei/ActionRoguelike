// Fill out your copyright notice in the Description page of Project Settings.


#include "AwAction.h"

void UAwAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Running: %s"), *GetNameSafe(this));
}


void UAwAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Stopping: %s"), *GetNameSafe(this));
}

FName UAwAction::GetActionName() const
{
	return this->ActionName;
}

UWorld* UAwAction::GetWorld() const
{
	
	AActor* Actor = Cast<AActor>(GetOuter());
	
	if (Actor)
	{
		return  Actor->GetWorld();
	}
	return nullptr;
}
