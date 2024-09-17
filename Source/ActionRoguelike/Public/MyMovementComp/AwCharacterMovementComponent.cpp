// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMovementComp/AwCharacterMovementComponent.h"

#include "System/MoveSystem.h"
#include "Utils/utils.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

using namespace AwUtils;

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
	SetMovementMode(MOVE_Walking, Move_None);
}

/*
 * @ params : deltaTime
 * @ params : Iterations , index between two deltaTime
 **/
void UAwCharacterMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
	GapFrame(deltaTime,MIN_TICK_TIME);
	
	RestorePreAdditiveRootMotionVelocity();

	FHitResult SurfaceHit;
	if (!AwMoveSystem::GetSlideSurface(this, SurfaceHit) || Velocity.SizeSquared() < pow(SlideMinSpeed, 2))
	{
		ExitSlide();
		StartNewPhysics(deltaTime, Iterations); // 
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
		CalcVelocity(deltaTime, SlideFriction, true, GetMaxBrakingDeceleration());
	}
	ApplyRootMotionToVelocity(deltaTime);

	// perform movement
	++Iterations;
	bJustTeleported = false;

	FVector OldLoca = UpdatedComponent->GetComponentLocation();

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

	// End of Sliding , check weather to continue slide
	FHitResult NewSurfaceHit;
	if (!AwMoveSystem::GetSlideSurface(this, NewSurfaceHit) || Velocity.SizeSquared() < pow(SlideMinSpeed, 2))
	{
		ExitSlide();

	}
	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		// may change when hit the wall so we need to re calculate the veco
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLoca) / deltaTime; // v =  dx / dt
	}
}
#pragma endregion

#pragma region ctor
UAwCharacterMovementComponent::UAwCharacterMovementComponent()
{
	Sprint_MaxWalkSpeed = 800.f;
	Walk_WalkSpeed = 400.f;
	GravityScale = 2.5f;
	bSafe_WantsToSprint = false;
	bSafe_WantsToClimb = false;
	NavAgentProps.bCanCrouch = true;
	SetIsReplicatedByDefault(true);
}

void UAwCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	AwCharacterOwner = Cast<AAwCharacter>(GetOwner());
	checkf(AwCharacterOwner!=nullptr, TEXT("AwCharacterMovementComponent must be used with AwCharacter"));
}
#pragma endregion ctor

#pragma region Save Move

bool UAwCharacterMovementComponent::FSavedMove_AwCharacter::CanCombineWith(const FSavedMovePtr& NewMove,
                                                                           ACharacter* InCharacter,
                                                                           float MaxDelta) const
{
	// Check currentMove and NewMove  & checks can we combine these two moves
	FSavedMove_AwCharacter* NewMoveCasted = static_cast<FSavedMove_AwCharacter*>(NewMove.Get());
	checkf(NewMoveCasted, TEXT("NewMove is not FSavedMove_AwCharacter"));
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
	UAwCharacterMovementComponent* CharacterMovement = cast(UAwCharacterMovementComponent, C->GetCharacterMovement());
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
	return FSavedMovePtr(new FSavedMove_AwCharacter());
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


void UAwCharacterMovementComponent::EnterClimb()
{
	SetMovementMode(MOVE_Custom, ECustomMovementMode::Move_Climbing);
}

void UAwCharacterMovementComponent::ExitClimb()
{
	bSafe_WantsToClimb = false; // Reset the flag
	SetMovementMode(MOVE_Falling);
}

bool UAwCharacterMovementComponent::CheckHasReachTheFloor()
{
	const FVector DownVec = -UpdatedComponent->GetUpVector();
	const FVector StartOffset = DownVec * 25 + UpdatedComponent->GetForwardVector() * -10;

	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + DownVec;
	TArray<FHitResult> Hits;
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(this, Start, End,
	                                                  20, 60,
	                                                  ClimbSurTypes,
	                                                  false, TArray<AActor*>(),
	                                                  EDrawDebugTrace::ForOneFrame, Hits, true);
	if (Hits.Num() == 0)
		return false;
	for (const auto& Result : Hits)
	{
		auto Degrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Result.ImpactNormal, -DownVec)));
		Debug::Print("Up: " + (-DownVec).ToString(), FColor::White, -1);
		Debug::Print("Angle: " + FString::SanitizeFloat(Degrees), FColor::White, -1);
		if (Degrees <= 30.f || (Degrees >= 55 && Degrees <= 61))
			return true;
	}
	return false;
}

bool UAwCharacterMovementComponent::CanClimbDownLedge() const
{
	if (!IsMovingOnGround()) return false;
	const FVector DownVec = -UpdatedComponent->GetUpVector();
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * ClimbDownWalkableSurTraceOffset;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + DownVec * 150;

	FHitResult WalkableSurResult = AwMoveSystem::DoLineTraceByObject(this, Start, End, true, ClimbSurTypes);

	const FVector LedgeTraceStart = WalkableSurResult.TraceStart + UpdatedComponent->GetForwardVector() *
		ClimbDownLedgeTraceOffset;
	const FVector LedgeTraceEnd = LedgeTraceStart + DownVec * 300;

	FHitResult LedgeResult = AwMoveSystem::DoLineTraceByObject(this, Start, End, true, ClimbSurTypes);

	if (WalkableSurResult.IsValidBlockingHit() && !LedgeResult.IsValidBlockingHit())
	{
		return true;
	}
	return false;
}

bool UAwCharacterMovementComponent::CanStartClimb() const
{
	if (AwMoveSystem::TraceClimbaleSurface(this, ClimbReachDistance, ClimbDectHeight))
	{
		if (IsFalling())
		{
			FVector Start = UpdatedComponent->GetComponentLocation();
			FVector End = Start + UpdatedComponent->GetForwardVector() * 50;
			auto HitResult = AwMoveSystem::DoLineTraceByObject(this, Start, End, true, ClimbSurTypes);
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

void UAwCharacterMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{
	GapFrame(deltaTime, MIN_TICK_TIME);

	/* Process all the climbable surface */
	TraceClimbableSurfaceTick(ClimbSurfaceTraceResults);
	ProcessClimbableSurfaceInfo(CurrentClimbSurfaceLocation, CurrentClimbSurfaceNormal);
	/* Check if we should stop climb */
	if (ClimbSurfaceTraceResults.Num() == 0 || CheckHasReachTheFloor())
	{
		bSafe_WantsToClimb = false; // Reset the flag
		return;
	}

	
	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(deltaTime, 0.f, true, GetMaxBrakingDeceleration());
	}
	
	ApplyRootMotionToVelocity(deltaTime);

	Iterations++;
	bJustTeleported = false;

	FHitResult SurHit(1.f);
	const FVector ActorLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime; // dx = v * dt
	// Handle climb rotation
	FQuat NewRotationQuat = GetClimbRotation(deltaTime);
	SafeMoveUpdatedComponent(Adjusted, NewRotationQuat, true, SurHit);

	if (SurHit.Time < 1.f)
	{
		HandleImpact(SurHit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, 1.f - SurHit.Time, SurHit.Normal, SurHit, true);
	}
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - ActorLocation) / deltaTime;
	}

	// Snap movement to surface
	const FVector Projected = (CurrentClimbSurfaceLocation - UpdatedComponent->GetComponentLocation()).ProjectOnTo(
		UpdatedComponent->GetForwardVector());
	const FVector Snap = Projected.Length() * -CurrentClimbSurfaceNormal;
	UpdatedComponent->MoveComponent(Snap * deltaTime * 3,
	                                UpdatedComponent->GetComponentQuat(),
	                                true);
}

bool UAwCharacterMovementComponent::TraceClimbableSurfaceTick(TArray<FHitResult>& HitResults)
{
	const FVector StartOffset = 50.f * UpdatedComponent->GetForwardVector();
	const FVector StartLocation = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector EndLocation = StartLocation + UpdatedComponent->GetForwardVector();
	HitResults = AwMoveSystem::DoCapsuleTraceMultiByObject(this, StartLocation, EndLocation, AwCharacterOwner->GetIgnoreCollisionParams(),
	                                         true,ClimbCapsuleTraceRadius, ClimbCapsuleTraceHalfHeight, ClimbSurTypes);
	return !HitResults.IsEmpty();
}

void UAwCharacterMovementComponent::ProcessClimbableSurfaceInfo(FVector& SurfaceLoca, FVector& SurfaceNormal)
{
	SurfaceLoca = FVector::ZeroVector;
	SurfaceNormal = FVector::ZeroVector;
	if (ClimbSurfaceTraceResults.Num() == 0)
		return;
	for (const auto& HitResult : ClimbSurfaceTraceResults)
	{
		SurfaceLoca += HitResult.ImpactPoint;
		SurfaceNormal += HitResult.ImpactNormal;
	}
	SurfaceLoca /= ClimbSurfaceTraceResults.Num();
	SurfaceNormal = SurfaceNormal.GetSafeNormal();
	// Debug::Print("ClimbSurfaceLocation: " + CurrentClimbSurfaceLocation.ToString(),FColor::Cyan,-1);
	// Debug::Print("ClimbSurfaceLNormal: " + CurrentClimbSurfaceNormal.ToString(),FColor::Cyan,-1);
}

FQuat UAwCharacterMovementComponent::GetClimbRotation(float DeltaTime)
{
	const FQuat CurrentQuat = UpdatedComponent->GetComponentQuat(); // 当前朝向， 四元数
	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return CurrentQuat;
	}

	FVector OL_Norm = CurrentClimbSurfaceNormal;
	FVector OL = CurrentClimbSurfaceLocation; // would be the start location of FindClimbRotation Call

	const FVector UpperStartOffset = 50.f * UpdatedComponent->GetForwardVector() + 25.f * FVector::UpVector;
	const FVector StartLocation = UpdatedComponent->GetComponentLocation() + UpperStartOffset;
	const FVector EndLocation = StartLocation + UpdatedComponent->GetForwardVector();

	ClimbSurfaceTraceResults = AwMoveSystem::DoCapsuleTraceMultiByObject(this, StartLocation, EndLocation,
	                                                       AwCharacterOwner->GetIgnoreCollisionParams(), true, ClimbCapsuleTraceRadius, ClimbCapsuleTraceHalfHeight, ClimbSurTypes);
	ProcessClimbableSurfaceInfo(CurrentClimbSurfaceUpperLocation, CurrentClimbSurfaceUpperNormal);

	FVector OLE = CurrentClimbSurfaceUpperLocation; // would be the end location of FindClimbRotation Call
	FVector OLE_Norm = CurrentClimbSurfaceUpperNormal;

	OL_Norm = FMath::VInterpTo(OL_Norm, OLE_Norm, DeltaTime, 5.f);
	//
	// DrawDebugPoint(GetWorld(), OLE, 10.f, FColor::Blue, false, 0.f);
	// DrawDebugPoint(GetWorld(), OL, 10.f, FColor::Yellow, false, 0.f);

	const FQuat TargetQuat = AwMoveSystem::FindClimbRotation(OL, OLE, OL_Norm);
	return FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, 5.f);
}

void UAwCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	bSafe_WantsToSprint = (Flags & FSavedMove_AwCharacter::FLAG_Custom_0) != 0;
	bSafe_WantsToClimb = (Flags & FSavedMove_AwCharacter::FLAG_Custom_1) != 0;
}

void UAwCharacterMovementComponent::OnClientCorrectionReceived(FNetworkPredictionData_Client_Character& ClientData,
	float TimeStamp, FVector NewLocation, FVector NewVelocity, UPrimitiveComponent* NewBase, FName NewBaseBoneName,
	bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode)
{
	Super::OnClientCorrectionReceived(ClientData, TimeStamp, NewLocation, NewVelocity, NewBase, NewBaseBoneName,
	                                  bHasBase, bBaseRelativePosition,
	                                  ServerMovementMode);
}

void UAwCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (MovementMode == MOVE_Walking && !bWantsToCrouch && bSaved_PreWantsToCrouch)
	{
		// when you quick double press crouch button, you will slide
		FHitResult HitResult;
		if (Velocity.SizeSquared() > pow(SlideMinSpeed, 2) && AwMoveSystem::GetSlideSurface(this, HitResult) )
		{
			EnterSlide();
		}
	}
	else if (IsCustomMoveMode(Move_Slide) && !bWantsToCrouch)
	{
		ExitSlide();
	}
	else if (MovementMode == MOVE_Walking || MovementMode == MOVE_Falling)
	{
		if (bSafe_WantsToClimb)
		{
			// TODO: Enter Climb Mode
			EnterClimb();
		}
	}
	else if (IsCustomMoveMode(Move_Climbing) && !bSafe_WantsToClimb)
	{
		ExitClimb();
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

#pragma region delegrate
void UAwCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
                                                      const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	switch (MovementMode)
	{
	case MOVE_Walking:
		SetWalkMaxSpeed();
		break;
	case MOVE_Falling:
		if (!bSafe_WantsToClimb && CanStartClimb())
			bSafe_WantsToClimb = true;
		break;
	case MOVE_Custom:
		if(CustomMovementMode == Move_Climbing)
			MaxCustomMovementSpeed = GetMaxSpeed();
		/* other custom move mode*/
		break;
	default:
		break;
	}

	if(SlideRecFlag == 0)
	{
		bSaved_PreWantsToCrouch = bWantsToCrouch;
		SlideRecFlag = 0x08;
	}
	else
		SlideRecFlag >>= 1;

}
void UAwCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (IsClimbing() && PreviousMovementMode == MOVE_Falling)
	{
		StopMovementImmediately();
		FVector StartLoca = UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * 50;
		auto EndLoca = StartLoca + UpdatedComponent->GetForwardVector() * 100;
		TArray<FHitResult> Hits = TArray<FHitResult>();
		while(Hits.Num() == 0)
		{
			Hits =  AwMoveSystem::DoCapsuleTraceMultiByObject(this, StartLoca,
															  EndLoca,
															  AwCharacterOwner->GetIgnoreCollisionParams(),
															  true,
															  ClimbCapsuleTraceRadius, ClimbCapsuleTraceHalfHeight, ClimbSurTypes);
			if (Hits.Num() > 0)
			{
				break;
			}
			EndLoca += UpdatedComponent->GetForwardVector() * 50;
		}
		FVector HitNormal = FVector::ZeroVector;
		FVector HitLocation = FVector::ZeroVector;
		if(Hits.Num() == 0)
			return;
		for (const auto& Hit : Hits)
		{
			HitNormal += Hit.ImpactNormal;
			HitLocation += Hit.ImpactPoint;
		}
		HitNormal /= Hits.Num();
		HitLocation /= Hits.Num();
 		FQuat NewRotation = FRotationMatrix::MakeFromX(-HitNormal).ToQuat();
		UpdatedComponent->MoveComponent((HitLocation - UpdatedComponent->GetComponentLocation()).GetSafeNormal(),
		                                NewRotation, false);
		bOrientRotationToMovement = false;
		AwCharacterOwner->GetMesh()->SetRelativeLocation(FVector(-20, 0, -120));
		AwCharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(60);
	}
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == Move_Climbing)
	{
		// Exit Climbing

		bOrientRotationToMovement = true;
		AwCharacterOwner->GetMesh()->SetRelativeLocation(FVector(0, 0, -95));
		AwCharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(90);
		FHitResult Hit;
		SafeMoveUpdatedComponent(UpdatedComponent->GetForwardVector() * -100,
		                         FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0).Quaternion(), true, Hit);
		StopMovementImmediately();
	}
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

#pragma endregion

void UAwCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case Move_Slide:
		PhysSlide(deltaTime, Iterations);
		return;
	case Move_Climbing:
		PhysClimb(deltaTime, Iterations);
		return;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Unknown Custom Movement Mode"));
		SetMovementMode(MOVE_None);
		return;
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

void UAwCharacterMovementComponent::Climb()
{
	bSafe_WantsToClimb = true;
}

void UAwCharacterMovementComponent::EdgeClimb()
{
	FHitResult HitResult(1.f);
	// TURN AROUND
	UpdatedComponent->SetWorldRotation(FRotator(0, -UpdatedComponent->GetComponentRotation().Yaw, 0));
}

void UAwCharacterMovementComponent::StopClimb()
{
	bSafe_WantsToClimb = false;
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

	switch (CustomMovementMode)
	{
	case Move_Climbing:
		return ClimbMaxSpeed;
	default:
		return 0.f;
	}
}

float UAwCharacterMovementComponent::GetMaxAcceleration() const
{
	if (MovementMode != MOVE_Custom)
		return Super::GetMaxAcceleration();
	switch (CustomMovementMode)
	{
	case Move_Climbing:
		return ClimbMaxAccleration;
	default:
		return 0.f;
	}
}

float UAwCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (MovementMode != MOVE_Custom)
		return Super::GetMaxBrakingDeceleration();
	switch (CustomMovementMode)
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



#pragma region move
void UAwCharacterMovementComponent::HandleMoveNormal(float Values, EMoveDirection D) const
{
	const FRotator Rotation = AwCharacterOwner->Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Direction = FVector::ZeroVector;
	if(D == EMoveDirection::Y)
		Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);
	else if (D == ::EMoveDirection::X)
		Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::X);
	AwCharacterOwner->AddMovementInput(Direction, Values);
}

void UAwCharacterMovementComponent::HandleMoveClimb(float Values, EMoveDirection D) const
{
	FHitResult HitResult;
	static TArray<AActor*> Actors2Ignore ({AwCharacterOwner,});
	FVector Direction = FVector::ZeroVector;
	if(D == EMoveDirection::Y)
		Direction = FVector::CrossProduct(-GetClimbSurfaceNormal(),-AwCharacterOwner->GetActorUpVector()).GetSafeNormal();
	else if (D == ::EMoveDirection::X)
		Direction = FVector::CrossProduct(-GetClimbSurfaceNormal(),AwCharacterOwner->GetActorRightVector()).GetSafeNormal();

	const FVector StartLocation = GetActorLocation() + Direction * 100 * Values;
	const FVector EndLocation = StartLocation + (-GetClimbSurfaceNormal()) * 100;

	bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), StartLocation, EndLocation,
														 10, 40, UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
														 false,
														 Actors2Ignore, EDrawDebugTrace::ForOneFrame,
														 HitResult,
														 true);
	if(!bHit)
	{
		if(D == ::EMoveDirection::X)
			DetectAndClimbUp();
		return;
	}
	AwCharacterOwner->AddMovementInput(Direction, Values);
}

void UAwCharacterMovementComponent::HandleMove(float Values, EMoveDirection D) const
{
	if(!AwCharacterOwner)
		return;
	if(IsClimbing())
		HandleMoveClimb(Values, D);
	else
	{
		HandleMoveNormal(Values, D);
	}
}

bool UAwCharacterMovementComponent::DetectAndClimbUp() const
{
	// Detect any obstacle on the edge
	FHitResult HitResult;
	static const TArray<AActor*> Actors2Ignore ({AwCharacterOwner,});
	const FVector UpVector = FVector::CrossProduct(-GetClimbSurfaceNormal(),AwCharacterOwner->GetActorRightVector()).GetSafeNormal();
	const FVector StartLocation = GetActorLocation() + UpVector * 150;
	const FVector EndLocation = StartLocation + -GetClimbSurfaceNormal() * 50;

	bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), StartLocation, EndLocation,
														 30, 90, UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
														 false,
														 Actors2Ignore, EDrawDebugTrace::ForOneFrame,
														 HitResult,
														 true);
	if (bHit) // if there is any obstacles
		return false;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(AwCharacterOwner); // 忽略当前角色自身，避免与自身发生碰撞
	if (GetWorld()->LineTraceSingleByChannel(HitResult, EndLocation, EndLocation + FVector(0, 0, -200), ECC_Visibility,
											 CollisionParams))
	{
		// can land
		AwCharacterOwner->ClimbingUp.Broadcast(HitResult.ImpactPoint);
		return true;
	}
	return false;
}

#pragma endregion

void UAwCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
