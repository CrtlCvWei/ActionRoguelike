// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMovementComp/AwCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#pragma region Slide
void UAwCharacterMovementComponent::EnterSlide()
{
	bWantsToCrouch = true;
	Velocity += Velocity.GetSafeNormal2D() * SlideEnterBoost;
	SetMovementMode(MOVE_Custom, ECustomMovementMode::Move_Slide);
}

void UAwCharacterMovementComponent::ExitSlide()
{
	bWantsToCrouch = false;
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(),
	                                                FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);
	SetMovementMode(MOVE_Walking);
}

void UAwCharacterMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
		return;

	RestorePreAdditiveRootMotionVelocity();

	FHitResult SurfaceHit;
	if (!GetSlideSurface(SurfaceHit) || Velocity.SizeSquared() < pow(SlideMinSpeed, 2))
	{
		ExitSlide();
		StartNewPhysics(deltaTime, Iterations);
		return;
	}
	// v += a * dt
	Velocity += SlideGravityForce * FVector::DownVector * deltaTime;

	if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > .5)
	{
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
	}
	else
		Acceleration = FVector::ZeroVector;
	// Cal Velo
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(deltaTime, SlideFriction, false, GetMaxBrakingDeceleration());
	}
	ApplyRootMotionToVelocity(deltaTime);

	// perform movement
	Iterations++;
	bJustTeleported = false;

	FVector OldLoca = UpdatedComponent->GetComponentLocation();
	FQuat OldRotation = UpdatedComponent->GetComponentQuat();
	FHitResult Hit(1.f);
	FVector Adjusted = Velocity * deltaTime;
	FVector VelPlaneDir = FVector::VectorPlaneProject(Velocity, SurfaceHit.Normal).GetSafeNormal();
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelPlaneDir, SurfaceHit.Normal).ToQuat();
	SafeMoveUpdatedComponent(Adjusted, NewRotation, true, Hit);

	// When U Hit
	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		// Slide Along Surface
		SlideAlongSurface(Adjusted, 1.f - Hit.Time, Hit.Normal, Hit, true);
	}

	FHitResult NewSurfaceHit;
	if (!GetSlideSurface(NewSurfaceHit) || Velocity.SizeSquared() < pow(SlideMinSpeed, 2))
	{
		ExitSlide();
		// StartNewPhysics(deltaTime, Iterations);
	}
	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLoca) / deltaTime; // v =  dx / dt
	}
}

bool UAwCharacterMovementComponent::GetSlideSurface(FHitResult& HitResult) const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + AwCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f *
		FVector::DownVector;
	FName ProfileName = FName(TEXT("BlockAll"));
	return GetWorld()->LineTraceSingleByProfile(HitResult, Start, End, ProfileName,
	                                            AwCharacterOwner->GetIgnoreCollisionParams());
}


#pragma endregion

UAwCharacterMovementComponent::UAwCharacterMovementComponent()
{
	Sprint_MaxWalkSpeed = 800.f;
	Walk_WalkSpeed = 400.f;
	GravityScale = 2.5f;
	bSafe_WantsToSprint = false;
	bSafe_WantsToClimb = false;
	NavAgentProps.bCanCrouch = true;
}

void UAwCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	AwCharacterOwner = Cast<AAwCharacter>(GetOwner());
	checkf(AwCharacterOwner!=nullptr, TEXT("AwCharacterMovementComponent must be used with AwCharacter"));
}

#pragma region Save Move

bool UAwCharacterMovementComponent::FSavedMove_AwCharacter::CanCombineWith(const FSavedMovePtr& NewMove,
                                                                           ACharacter* InCharacter,
                                                                           float MaxDelta) const
{
	// Check currentMove and NewMove  & checks can we combine these two moves
	FSavedMove_AwCharacter* NewMoveCasted = static_cast<FSavedMove_AwCharacter*>(NewMove.Get());
	if (bSaved_WantsToSprint != NewMoveCasted->bSaved_WantsToSprint || bSaved_WantsToClimb != NewMoveCasted->
		bSaved_WantsToClimb)
	{
		return false;
	}
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UAwCharacterMovementComponent::FSavedMove_AwCharacter::Clear()
{
	Super::Clear();
	bSaved_WantsToSprint = 0;
	bSaved_WantsToClimb = 0;
}

uint8 UAwCharacterMovementComponent::FSavedMove_AwCharacter::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	if (bSaved_WantsToSprint)
		// FLAG_Custom_0 : 0x10 The fifth bit
		Result |= FLAG_Custom_0;
	if (bSaved_WantsToClimb)
		// FLAG_Custom_0 : 0x20 The sixth bit
		Result |= FLAG_Custom_1;
	return Result;
}

void UAwCharacterMovementComponent::FSavedMove_AwCharacter::SetMoveFor(ACharacter* C, float InDeltaTime,
                                                                       FVector const& NewAccel,
                                                                       FNetworkPredictionData_Client_Character&
                                                                       ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	UAwCharacterMovementComponent* CharacterMovement = Cast<UAwCharacterMovementComponent>(C->GetCharacterMovement());
	if (CharacterMovement)
	{
		// Copy the state value 
		bSaved_WantsToSprint = CharacterMovement->bSafe_WantsToSprint;
		bSaved_PreWantsToCrouch = CharacterMovement->bSaved_PreWantsToCrouch;
		bSaved_WantsToClimb = CharacterMovement->bSafe_WantsToClimb;
	}
}

void UAwCharacterMovementComponent::FSavedMove_AwCharacter::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);
	UAwCharacterMovementComponent* CharacterMovement = Cast<UAwCharacterMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->bSafe_WantsToSprint = bSaved_WantsToSprint;
	CharacterMovement->bSaved_PreWantsToCrouch = bSaved_PreWantsToCrouch;
	CharacterMovement->bSafe_WantsToClimb = bSaved_WantsToClimb;
}

#pragma endregion

#pragma region Client Network Prediction

UAwCharacterMovementComponent::FNetworkPredictionData_Client_AwCharacter::FNetworkPredictionData_Client_AwCharacter(
	const UCharacterMovementComponent& ClientMovement):
	Super(ClientMovement)
{
}

FSavedMovePtr UAwCharacterMovementComponent::FNetworkPredictionData_Client_AwCharacter::AllocateNewMove()
{
	return FNetworkPredictionData_Client_Character::AllocateNewMove();
}

FNetworkPredictionData_Client* UAwCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);
	if (!ClientPredictionData)
	{
		UAwCharacterMovementComponent* MutableThis = const_cast<UAwCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_AwCharacter(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}
	return ClientPredictionData;
}

TArray<FHitResult> UAwCharacterMovementComponent::DoCapsuleTraceMultiByObject(
	const FVector& Start, const FVector& End, const FCollisionQueryParams& Params, bool ShowDebugTrace) const
{
	TArray<FHitResult> CapsuleTraceResults;
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(this, Start, End,
	                                                  ClimbCapsuleTraceRadius, ClimbCapsuleTraceHalfHeight,
	                                                  ClimbSurTypes,
	                                                  false, TArray<AActor*>(),
	                                                  ShowDebugTrace
		                                                  ? EDrawDebugTrace::ForOneFrame
		                                                  : EDrawDebugTrace::None, CapsuleTraceResults, false);
	return std::move(CapsuleTraceResults);
}

void UAwCharacterMovementComponent::EnterClimb()
{
	Velocity = FVector::ZeroVector;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, UpdatedComponent->GetComponentLocation(),
	                                     UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetForwardVector()
	                                     * 100,
	                                     ECC_WorldStatic, AwCharacterOwner->GetIgnoreCollisionParams());
	FQuat NewRotation = FRotationMatrix::MakeFromX(-Hit.Normal).ToQuat();
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, false, Hit);

	bOrientRotationToMovement = false;
	AwCharacterOwner->GetMesh()->SetRelativeLocation(FVector(-20, 0, -120));
	AwCharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(60);

	SetMovementMode(MOVE_Custom, ECustomMovementMode::Move_Climbing);
}

void UAwCharacterMovementComponent::ExitClimb()
{
}

void UAwCharacterMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
		return;

	if (!AwCharacterOwner || (!AwCharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !
		CurrentRootMotion.HasOverrideVelocity()))
	{
		Velocity = FVector::ZeroVector;
		Acceleration = FVector::ZeroVector;
		return;
	}

	FHitResult SurHit;
	const FVector ActorLocation = UpdatedComponent->GetComponentLocation();
	const FVector ActorForwardVector = UpdatedComponent->GetForwardVector();

	bool bHit = GetWorld()->LineTraceSingleByObjectType(SurHit, ActorLocation,
	                                                    ActorLocation + ActorForwardVector * ClimbReachDistance,
	                                                    ECC_WorldStatic,
	                                                    AwCharacterOwner->GetIgnoreCollisionParams()); // 忽略当前角色自身，
	if (!bHit)
	{
		ExitClimb();
		StartNewPhysics(deltaTime, Iterations);
		return;
	}
	// perform movement
	Iterations++;
	bJustTeleported = false;

	// Transform Acceleration
	Acceleration.Z = 0;
	Acceleration = Acceleration.RotateAngleAxis(90.f, -UpdatedComponent->GetRightVector());


	// AddMovementInput
	CalcVelocity(deltaTime, 0.f, false, GetMaxBrakingDeceleration());
	Velocity = FVector::VectorPlaneProject(Velocity, SurHit.Normal).GetSafeNormal() * ClimbMaxSpeed;

	const auto dx = deltaTime * Velocity;

	FHitResult HitResult;
	// const FRotator Rotation = FRotator(0, ClimbRotator.Yaw, 0);
	// const FVector Direction = FRotationMatrix(ClimbRotator).GetScaledAxis(EAxis::Z);
	// const FVector StartLocation = GetActorLocation() + FRotationMatrix(ClimbRotator).GetScaledAxis(EAxis::Z) * 100 *
	// 	FVector::VectorPlaneProject(dx,SurHit.Normal).;
	// const FVector EndLocation = StartLocation + FRotationMatrix(Rotation).GetScaledAxis(EAxis::X) * 100;
	//
	// bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), StartLocation, EndLocation,
	// 													 10, 35, UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
	// 													 false,
	// 													 TArray<AActor*>({this}), EDrawDebugTrace::ForOneFrame,
	// 													 HitResult,
	// 													 true);
}

bool UAwCharacterMovementComponent::TraceClimbaleSurface() const
{
	const FVector ForWardVector = UpdatedComponent->GetForwardVector();
	const FVector StartOffset = 50.f * ForWardVector;
	FVector StartLocation = UpdatedComponent->GetComponentLocation() + StartOffset;
	FVector EndLocation = StartLocation + ForWardVector * ClimbReachDistance;
	FVector BoxExtend = FVector(50, 50, 25);

	FCollisionShape CollisionBox = FCollisionShape::MakeBox(BoxExtend);

	// 执行射线检测
	FHitResult HitResult;
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity,
	                                             ECC_WorldStatic, CollisionBox,
	                                             AwCharacterOwner->GetIgnoreCollisionParams());

	if (!bHit)
		return false;

	FVector Hit_normal = HitResult.ImpactNormal;
	FVector Hit_location = HitResult.ImpactPoint;

	StartLocation = Hit_location + Hit_normal * 100;
	EndLocation = Hit_location - Hit_normal * 200;

	// 计算 法向量 和角色朝向夹角
	const float Angle = FMath::Acos(FVector::DotProduct(Hit_normal, -1 * ForWardVector));
	if (abs(Angle) > 30.f)
		return false;

	const float Dis_thred = ClimbReachDistance * 0.5;
	FVector Hit_location_pre = Hit_location;
	FVector ZOffest = FVector(0, 0, 25);

	while (bHit && Hit_location.Z < AwCharacterOwner->BaseEyeHeight + ClimbDectHeight)
	{
		GetWorld()->SweepSingleByObjectType(HitResult, StartLocation, EndLocation, FQuat::Identity,
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
		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, BoxExtend, FQuat::Identity, FColor::Red, false, 1.f);

		Hit_location_pre = Hit_location;
		StartLocation += ZOffest;
		EndLocation += ZOffest;
	}

	if (Hit_location.Z > AwCharacterOwner->BaseEyeHeight + 50.)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, Hit_normal.ToString());
		DrawDebugBox(GetWorld(), Hit_location, BoxExtend, FQuat::Identity, FColor::Green, false, 2.f);
		return true;
	}

	return false;
}

void UAwCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	bSafe_WantsToSprint = (Flags & FSavedMove_AwCharacter::FLAG_Custom_0) != 0;
	bSafe_WantsToClimb = (Flags & FSavedMove_AwCharacter::FLAG_Custom_1) != 0;
}

void UAwCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (MovementMode == MOVE_Walking && !bWantsToCrouch && bSaved_PreWantsToCrouch)
	{
		// when you quick double press crouch button, you will slide
		FHitResult HitResult;
		if (Velocity.SizeSquared() > pow(SlideMinSpeed, 2) && GetSlideSurface(HitResult))
		{
			EnterSlide();
		}
	}
	else if (IsCustomMoveMode(Move_Slide) && !bWantsToCrouch)
	{
		ExitSlide();
	}
	else if (MovementMode == MOVE_Walking || IsFalling())
	{
		if(CanStartClimb())
		{
			// TODO:
			Debug::Print("Check Can Start Climb");
		}
	}
	else if(IsCustomMoveMode(Move_Climbing))
	{
		
	}

	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

#pragma endregion

#pragma region Input

void UAwCharacterMovementComponent::SetWalkMaxSpeed()
{
	if (bSafe_WantsToSprint)
	{
		MaxWalkSpeed = Sprint_MaxWalkSpeed;
	}
	else
	{
		MaxWalkSpeed = Walk_WalkSpeed;
	}
}

void UAwCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
                                                      const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (MovementMode == MOVE_Walking)
	{
		SetWalkMaxSpeed();
	}
	else if (MovementMode == Move_Climbing)
	{
		MaxCustomMovementSpeed = GetMaxSpeed();
	}
	bSaved_PreWantsToCrouch = bWantsToCrouch;
}

void UAwCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case Move_Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	case Move_Climbing:
		PhysClimb(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Unknown Custom Movement Mode"));
		SetMovementMode(MOVE_None);
		break;
	}
}

bool UAwCharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMoveMode(Move_Slide);
}

bool UAwCharacterMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump();
}

void UAwCharacterMovementComponent::Sprint()
{
	bSafe_WantsToSprint = true;
}

void UAwCharacterMovementComponent::EndSprint()
{
	bSafe_WantsToSprint = false;
}

void UAwCharacterMovementComponent::CrouchPressed()
{
	bWantsToCrouch = !bWantsToCrouch;
}

bool UAwCharacterMovementComponent::IsCustomMoveMode(ECustomMovementMode CustomMode) const
{
	return MovementMode == MOVE_Custom && CustomMode == CustomMovementMode;
}

float UAwCharacterMovementComponent::GetMaxSpeed() const
{
	if (MovementMode == MOVE_Walking && bSafe_WantsToSprint && !IsCrouching())
		return Sprint_MaxWalkSpeed;
	if (MovementMode != MOVE_Custom)
		return Super::GetMaxSpeed();

	switch (MovementMode)
	{
	case Move_Climbing:
		return ClimbMaxSpeed;
	default:
		return 0.f;
	}
}

float UAwCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (MovementMode != MOVE_Custom)
		return Super::GetMaxBrakingDeceleration();
	switch (MovementMode)
	{
	case Move_Climbing:
		return BrakingDecelerationClimbing;
	default:
		return 1000.f;
	}
}

bool UAwCharacterMovementComponent::IsClimbing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == Move_Climbing;
}

#pragma endregion


void UAwCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// const FVector ForWardVector = UpdatedComponent->GetForwardVector();
	// const FVector StartOffset = 50.f * ForWardVector;
	// FVector StartLocation = UpdatedComponent->GetComponentLocation() + StartOffset;
	// FVector EndLocation = StartLocation + ForWardVector;
	// // DoCapsuleTraceMultiByObject(StartLocation,EndLocation,AwCharacterOwner->GetIgnoreCollisionParams(),true);
	// TraceFromEyeHeight(100,-10);
}

FHitResult UAwCharacterMovementComponent::DoLineTraceByObject(const FVector& Start, const FVector& End,
                                                              bool ShowDebugTrace) const
{
	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingleForObjects(this,
	                                                Start, End, ClimbSurTypes,
	                                                false, TArray<AActor*>(),
	                                                ShowDebugTrace
		                                                ? EDrawDebugTrace::ForOneFrame
		                                                : EDrawDebugTrace::None,
	                                                HitResult, false);
	return std::move(HitResult);
}

FHitResult UAwCharacterMovementComponent::TraceFromEyeHeight(const float TraceDis, const float TraceStartOffset) const
{
	const FVector CompLoca = UpdatedComponent->GetComponentLocation();
	const FVector EyeLoca = FVector(0, 0, AwCharacterOwner->BaseEyeHeight + TraceStartOffset);
	const FVector Start = CompLoca + EyeLoca + TraceStartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDis;
	return DoLineTraceByObject(Start, End, true);
}

bool UAwCharacterMovementComponent::CanStartClimb() const
{
	if (TraceClimbaleSurface())
	{
		if (IsFalling())
		{
			FVector Start = UpdatedComponent->GetComponentLocation();
			FVector End = Start + UpdatedComponent->GetForwardVector() * 75;
			auto HitResult = DoLineTraceByObject(Start, End, true);
			if (HitResult.IsValidBlockingHit())
				return true;
			return false;
		}
		else if (IsMovingOnGround())
		{
			return true;
		}
	}
	return false;
}
