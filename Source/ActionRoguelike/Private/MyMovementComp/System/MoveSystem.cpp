// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\..\Public\MyMovementComp\System/MoveSystem.h"
#include "MyMovementComp/AwCharacterMovementComponent.h"
#include "Utils/utils.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
AwMoveSystem::AwMoveSystem(){}
AwMoveSystem::~AwMoveSystem(){}


#define GetForwardVec(Comp) Comp->UpdatedComponent->GetForwardVector()
#define GetRightVec(Comp) Comp->UpdatedComponent->GetRightVector()
#define GetUpVec(Comp) Comp->UpdatedComponent->GetUpVector()
#define GetLocation(Comp) Comp->UpdatedComponent->GetComponentLocation()

bool AwMoveSystem::GetSlideSurface(UCharacterMovementComponent* MoveComp, FHitResult& HitResult)
{
	auto AwMoveComp =  cast(UAwCharacterMovementComponent, MoveComp);
	FVector Start = AwMoveComp->UpdatedComponent->GetComponentLocation();
	FVector End = Start + AwMoveComp->GetAwOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f *
		FVector::DownVector;
	FName ProfileName = FName(TEXT("BlockAll"));
	return AwMoveComp->GetAwOwner()->GetWorld()->LineTraceSingleByProfile(HitResult, Start, End, ProfileName,
												AwMoveComp->GetAwOwner()->GetIgnoreCollisionParams());
}

bool AwMoveSystem::TraceClimbaleSurface(const UCharacterMovementComponent* MoveComp, const float ReachDist, const float ClimbDectHeight)
{
	static constexpr float Box_x = 50, Box_y = 50, Box_z = 25;
	#define AwCharacterOwner AwComp->GetAwOwner()
	//
	const auto AwComp = cast(UAwCharacterMovementComponent, MoveComp);
	const FVector ForWardVector = GetForwardVec(AwComp);
	const FVector StartOffset = 50.f * ForWardVector;
	FVector StartLocation = GetLocation(AwComp) + StartOffset;
	FVector EndLocation = StartLocation + ForWardVector * ReachDist;
	FVector BoxExtend = FVector(Box_x, Box_y, Box_z);
	//
	FCollisionShape CollisionBox = FCollisionShape::MakeBox(BoxExtend);
	FHitResult HitResult;
	// 射线检测
	bool bHit = AwComp->GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity,
												 ECC_WorldStatic, CollisionBox,
												  AwCharacterOwner->GetIgnoreCollisionParams());
	if (!bHit)
		return false;
	/* find some walls */
	FVector Hit_normal = HitResult.ImpactNormal;
	FVector Hit_location = HitResult.ImpactPoint;
	StartLocation = Hit_location + Hit_normal * 100;
	EndLocation = Hit_location - Hit_normal * 200;
	/* 如果人物与攀爬面朝向角度不符合要求，也会失败 */
	const float Angle = Radian2Angle(FMath::Acos(FVector::DotProduct(Hit_normal.ProjectOnTo(-1 * ForWardVector), -1 * ForWardVector)));
	if (abs(Angle) > 35.f)
		return false;
	/* 如果隔了障碍物，也会失败 */
	const float Dis_thred = ReachDist * 0.25;
	FVector Hit_location_pre = Hit_location;
	const FVector ZOffest = FVector(0, 0, 25);
	float ZDelta = 0.f;
	
	while (bHit && ZDelta < AwCharacterOwner->BaseEyeHeight + ClimbDectHeight)
	{
		AwComp->GetWorld()->SweepSingleByObjectType(HitResult, StartLocation, EndLocation, FQuat::Identity,
											ECC_WorldStatic,
											CollisionBox, AwCharacterOwner->GetIgnoreCollisionParams());
		if (!HitResult.IsValidBlockingHit())
			break;

		Hit_normal = HitResult.ImpactNormal;
		Hit_location = HitResult.ImpactPoint;

		float Dis = FMath::Abs((Hit_location - Hit_location_pre).Dot(-Hit_normal));

		if (Dis > Dis_thred || Hit_normal.Z < -0.5 || Hit_normal.Z > 0.86) // Dis > THRED means may have obstacle
			return false;
		//
		DrawDebugBox(AwComp->GetWorld(), HitResult.ImpactPoint, BoxExtend, FQuat::Identity, FColor::Red, false, 1.f);

		Hit_location_pre = Hit_location;
		ZDelta += ZOffest.Z;
		StartLocation += ZOffest;
		EndLocation += ZOffest;
	}

	if (ZDelta > AwCharacterOwner->BaseEyeHeight + ClimbDectHeight / 2)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, Hit_normal.ToString());
		DrawDebugBox(AwComp->GetWorld(), Hit_location, BoxExtend, FQuat::Identity, FColor::Green, false, 2.f);
		return true;
	}

	return false;
}

FQuat AwMoveSystem::FindClimbRotation(const FVector StartLoca, const FVector EndLoca, const FVector ObstacleNormalVec)
{
	FVector ClimbUpVec = EndLoca - StartLoca;
	ClimbUpVec.Normalize(); // 单位向量
	FVector ClimbRightVec = ObstacleNormalVec.Cross(ClimbUpVec); // ue 是左手系好像
	ClimbRightVec.Normalize();
	FVector Up = ClimbRightVec.Cross(ObstacleNormalVec);
	Up.Normalize();
	return FMatrix(-1 * ObstacleNormalVec, ClimbRightVec, Up, FVector::ZeroVector).Rotator().Quaternion();
}

/*  */
FHitResult AwMoveSystem::DoLineTraceByObject(const UCharacterMovementComponent* MoveComp, const FVector Start,
                                             const FVector End, const bool ShowDebugTrace, const TArray<TEnumAsByte<EObjectTypeQuery>>& ClimbSurTypes)
{
	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingleForObjects(MoveComp,
													Start, End, ClimbSurTypes,
													false, TArray<AActor*>(),
													ShowDebugTrace
														? EDrawDebugTrace::ForOneFrame
														: EDrawDebugTrace::None,
													HitResult, false);
	return std::move(HitResult);
}

FHitResult AwMoveSystem::TraceFromEyeHeight(const UCharacterMovementComponent* MoveComp,
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ClimbSurTypes, const float TraceDis, const float TraceStartOffset)
{
	const auto AwComp = cast(UAwCharacterMovementComponent, MoveComp);
	const FVector CompLoca = AwComp->UpdatedComponent->GetComponentLocation();
	const FVector EyeLoca = FVector(0, 0, AwComp->GetAwOwner()->BaseEyeHeight + TraceStartOffset);
	const FVector Start = CompLoca + EyeLoca + TraceStartOffset;
	const FVector End = Start + AwComp->UpdatedComponent->GetForwardVector() * TraceDis;
	return DoLineTraceByObject(MoveComp, Start, End, true, ClimbSurTypes);
}

TArray<FHitResult> AwMoveSystem::DoCapsuleTraceMultiByObject(const UCharacterMovementComponent* MoveComp,
                                                             const FVector& Start, const FVector& End, const FCollisionQueryParams& Params, bool ShowDebugTrace,
                                                             const float CapsuleTraceRadius, const float CapsuleTraceHalfHeight,
                                                             const TArray<TEnumAsByte<EObjectTypeQuery>>& ClimbSurTypes)
{
	TArray<FHitResult> CapsuleTraceResults = TArray<FHitResult>();
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(MoveComp, Start, End,
													  CapsuleTraceRadius, CapsuleTraceHalfHeight,
													  ClimbSurTypes,
													  false, TArray<AActor*>(),
													  ShowDebugTrace
														  ? EDrawDebugTrace::ForOneFrame
														  : EDrawDebugTrace::None, CapsuleTraceResults, true);
	return std::move(CapsuleTraceResults);
}

