// Fill out your copyright notice in the Description page of Project Settings.


#include "AwAction_Sprint.h"

#include "AwCharacter.h"
#include "AWPlayerState.h"
#include "MyMovementComp/AwCharacterMovementComponent.h"

inline AActor* UAwAction_Sprint::GetTheOwner() const
{
	AAWPlayerState* OwnerPS = Cast<AAWPlayerState>(GetOuter()->GetOuter());
	return OwnerPS->GetPawn();
}

void UAwAction_Sprint::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	AActor* Owner = GetTheOwner();
	if (Owner == nullptr)
	{
		return;
	}
	if (AAwCharacter* AwOwner = Cast<AAwCharacter>(Owner))
	{
		auto AwMoveComp = Cast<UAwCharacterMovementComponent>(AwOwner->GetCharacterMovement());
		AwMoveComp->Sprint();
		bIsRunning = true;
	}
}

void UAwAction_Sprint::StopAction_Implementation(AActor* Instigator)
{
	if (bIsRunning)
	{
		Super::StopAction_Implementation(Instigator);
		if (AAwCharacter* Owner = Cast<AAwCharacter>(GetTheOwner()))
		{
			auto AwMoveComp = Cast<UAwCharacterMovementComponent>(Owner->GetCharacterMovement());
			AwMoveComp->EndSprint();
		}
	}
}
