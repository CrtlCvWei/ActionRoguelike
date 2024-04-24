// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AwCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Debug/DebugHelper.h"
#include "AwCharacterMovementComponent.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum ECustomMovementMode : int
{
	Move_None UMETA(Hidden),
	Move_Slide UMETA(DisplayName= "Slide"),
	Move_Climbing UMETA(DisplayName= "Climb"),
	Move_Max UMETA(Hidden)
};

UCLASS()
class ACTIONROGUELIKE_API UAwCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_AwCharacter : public FSavedMove_Character
	{
	public:
		typedef FSavedMove_Character Super;
		uint8 bSaved_WantsToSprint : 1;
		uint8 bSaved_PreWantsToCrouch : 1;
		uint8 bSaved_WantsToClimb : 1;

		virtual bool
		CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
		                        FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};


	// NetWorkPrediction
	class FNetworkPredictionData_Client_AwCharacter : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_AwCharacter(const UCharacterMovementComponent& ClientMovement);
		typedef FNetworkPredictionData_Client_Character Super;
		virtual FSavedMovePtr AllocateNewMove() override;
	};

	bool bSafe_WantsToSprint;
	bool bSafe_WantsToClimb;
	bool bSaved_PreWantsToCrouch;

	UPROPERTY(EditDefaultsOnly)
	float Sprint_MaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly)
	float Walk_WalkSpeed;


	UPROPERTY(Transient)
	AAwCharacter* AwCharacterOwner;

private:
#pragma region SlidePhy

	UPROPERTY(EditDefaultsOnly)
	float SlideMinSpeed = 350.f;
	UPROPERTY(EditDefaultsOnly)
	float SlideMaxSpeed = 5000.f;
	UPROPERTY(EditDefaultsOnly)
	float SlideGravityForce = 5000.f;
	UPROPERTY(EditDefaultsOnly)
	float SlideFriction = 1.f;
	UPROPERTY(EditDefaultsOnly)
	float SlideEnterBoost = 500.f;

	void EnterSlide();
	void ExitSlide();
	void PhysSlide(float deltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& HitResult) const;

#pragma endregion

#pragma region Climb

	FHitResult DoLineTraceByObject(const FVector& Start, const FVector& End,bool ShowDebugTrace) const;
	FHitResult TraceFromEyeHeight(const float TraceDis,const float TraceStartOffset = 0) const;
	TArray<FHitResult> DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End,const FCollisionQueryParams& Params,bool ShowDebugTrace) const;

#pragma region ClimbVariables
	UPROPERTY(EditDefaultsOnly)
	float ClimbCapsuleTraceRadius = 50.f;
	UPROPERTY(EditDefaultsOnly)
	float ClimbCapsuleTraceHalfHeight = 72.f;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery> >  ClimbSurTypes;
	
	UPROPERTY(EditDefaultsOnly)
	float ClimbMaxSpeed = 200.f;
	UPROPERTY(EditDefaultsOnly)
	float ClimbingGravityScale = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float BrakingDecelerationClimbing = 512.f;
	UPROPERTY(EditDefaultsOnly)
	float ClimbReachDistance = 150.f;
	UPROPERTY(EditDefaultsOnly)
	float ClimbDectHeight = 100.f;
#pragma endregion
	
public:
	bool TraceClimbaleSurface() const;
private:
	void EnterClimb();
	void ExitClimb();
	void PhysClimb(float deltaTime, int32 Iterations);
	bool CanStartClimb() const;
#pragma endregion
	
protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

public:
	UAwCharacterMovementComponent();
	
	virtual void InitializeComponent() override;
	
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	inline void SetWalkMaxSpeed();

	virtual bool IsMovingOnGround() const override;
	virtual  bool CanAttemptJump() const override;
	UFUNCTION(BlueprintCallable)
	void Sprint();
	UFUNCTION(BlueprintCallable)
	void EndSprint();
	UFUNCTION(BlueprintCallable)
	void CrouchPressed();

	UFUNCTION(BlueprintPure)
	bool IsCustomMoveMode(ECustomMovementMode CustomMode) const;
	
	
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	
	UFUNCTION(BlueprintCallable)
	bool IsClimbing() const;

	virtual  void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
