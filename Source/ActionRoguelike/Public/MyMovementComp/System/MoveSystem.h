// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

class UCharacterMovementComponent;

class ACTIONROGUELIKE_API AwMoveSystem
{
private:
	AwMoveSystem();
	~AwMoveSystem();
public:

#pragma region slide
	static bool GetSlideSurface(UCharacterMovementComponent* MoveComp ,FHitResult& HitResult) ;
	
	
#pragma endregion


#pragma region ClimbDetect
	static bool TraceClimbaleSurface(const UCharacterMovementComponent* MoveComp, const float ReachDist,  const float ClimbDectHeight);

	static FQuat FindClimbRotation(const FVector StartLoca, const FVector EndLoca, const FVector ObstacleNormalVec);
#pragma endregion ClimbDetect

	
#pragma region trace
	
	static FHitResult DoLineTraceByObject(const UCharacterMovementComponent* MoveComp,const FVector Start, const FVector End,
															 const bool ShowDebugTrace, const TArray<TEnumAsByte<EObjectTypeQuery>>& ClimbSurTypes );

	static FHitResult TraceFromEyeHeight(const UCharacterMovementComponent* MoveComp,const TArray<TEnumAsByte<EObjectTypeQuery>>& ClimbSurTypes, const float TraceDis = 100,const float TraceStartOffset = 0);
	
	
	static TArray<FHitResult> DoCapsuleTraceMultiByObject(const UCharacterMovementComponent* MoveComp, const FVector& Start, const FVector& End,const FCollisionQueryParams& Params,bool ShowDebugTrace,
		const float CapsuleTraceRadius, const float CapsuleTraceHalfHeight, const TArray<TEnumAsByte<EObjectTypeQuery>>& ClimbSurTypes ) ;

#pragma endregion

};
