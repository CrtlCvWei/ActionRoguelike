// Fill out your copyright notice in the Description page of Project Settings.


#include "AwAction_Sprint.h"

#include "AwCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAwAction_Sprint::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	ACharacter* Owner = Cast<ACharacter>(GetOuter()->GetOuter());
	Owner->GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
	bIsRunning = true;
}

void UAwAction_Sprint::StopAction_Implementation(AActor* Instigator)
{
	if (bIsRunning)
	{
		Super::StopAction_Implementation(Instigator);
		ACharacter* Owner = Cast<ACharacter>(GetOuter()->GetOuter());
		Owner->GetCharacterMovement()->MaxWalkSpeed = NormalMoveSpeed;
		bIsRunning = false;
	}
}
