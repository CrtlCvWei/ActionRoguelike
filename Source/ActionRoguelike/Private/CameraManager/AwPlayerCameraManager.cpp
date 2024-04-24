// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraManager/AwPlayerCameraManager.h"
#include "MyMovementComp/AwCharacterMovementComponent.h"
#include "AwCharacter.h"
#include "Components/CapsuleComponent.h"

AAwPlayerCameraManager::AAwPlayerCameraManager()
{
	CrouchBlendDuration = 0.5f;
}

void AAwPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);
	if (AAwCharacter* Character = Cast<AAwCharacter>(GetOwningPlayerController()->GetPawn()))
	{
		UAwCharacterMovementComponent* MoveComp = Character->GetAwCharacterMovement();
		FVector TargetCrouchOffset = FVector(
			0.f, 0.f,
			MoveComp->GetCrouchedHalfHeight() - Character->GetClass()->GetDefaultObject<ACharacter>()->
			                                               GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		FVector Offest = FMath::Lerp(FVector::ZeroVector, TargetCrouchOffset,FMath::Clamp(CrouchBlendTime / CrouchBlendDuration, 0.f, 1.f));	
		if (MoveComp->IsCrouching())
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime + DeltaTime, 0.f, CrouchBlendDuration);
			Offest -= TargetCrouchOffset;
		}
		else
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime - DeltaTime, 0.f, CrouchBlendDuration);
			Offest += TargetCrouchOffset;
		}
		if(MoveComp->IsMovingOnGround())
			OutVT.POV.Location += Offest;
	}
}
