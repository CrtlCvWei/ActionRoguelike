// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "AwCharacter.generated.h"

class UInputAction;
class UAwCharacterMovementComponent;
class UAwActionComponent;
class UCameraComponent;
class USpringArmComponent;
class UAWInteractionComponent;
class UAWAttributeComp;
class UAnimMontage;
class UAwCharacterMovementComponent;


// 委托多播
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClimingUpSignature, FVector, ImpactPoint);

UCLASS()
class ACTIONROGUELIKE_API AAwCharacter : public ACharacter
{
	GENERATED_BODY()

private:

	const float JumpHoldTimeThreshold = 0.3f;
	const float JumpZVelocity = 800.f;
	const float MaxMoveSpeed = 800.f;
	const float NormalMoveSpeed = 400.f;
	const float GravityScale = 2.5f;
	const float BrakingFriction = 1.f;
	const float BrakingDecelerationWalking = 10.f;
	const float ClimbVectorZ = 170.f;
	const float ClimbVectorX = 100.f;
	
	UFUNCTION()
	void Init_Paramters();
	
	FTimerHandle InitTimerHandle;
	
	UFUNCTION()
	void Init_GAS();

public:
	// Sets default values for this character's properties
	AAwCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	
	UPROPERTY()
	UArrowComponent* ArrowComp;
	
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Represents a camera viewpoint and settings, such as projection type, field of view, and post-process overrides.
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UAWInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere)
	UAwCharacterMovementComponent* AwCharacterMovementComp;

	// GAS demo

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ClimbAndVault")
	FName RightKneeSocketName;
	

	
	//Basic Actions
	UFUNCTION()
	void MoveForward(float Values);
	UFUNCTION()
	void MoveRight(float Values);

	void CrouchPressed();
	
	virtual void Jump() override;
	
	UFUNCTION()
	virtual void ClearJumpInput(float DeltaTime) override;
	
	virtual void StopJumping() override;
	//Attack Action
	UFUNCTION()
	void PrimaryAttack();
	UFUNCTION()
	void BlackHoleAbility();
	UFUNCTION()
	void BeginSprint();
	UFUNCTION()
	void EndSprint();
	
	
	//Interact
	UFUNCTION()
	void PrimaryInterat();
	FVector GetPawnViewLocation() const override;
	
	void Deadth();

	FTimerHandle JumpTimerHandle; // 创建一个FTimerHandle对象
	FTimerHandle ProjectileSpawnHandle;//
	
	// trace detection
	UPROPERTY(BlueprintAssignable)
	FClimingUpSignature ClimbingUp;
	
	UPROPERTY(EditAnywhere,Category="ClimbAndVault")
	UAnimMontage* ClimbStartMontage;

	UPROPERTY(EditAnywhere,Category="ClimbAndVault")
	UAnimMontage* ClimbUpMontage;
	
	UFUNCTION(BlueprintCallable)
	void RidOffClimbingMode();
	UFUNCTION()
	void MoveForwardWhenClimbing(float Values);
	UFUNCTION()
	void MoveRightWhenClimbing(float Values);
	UFUNCTION()
	bool DetectAndClimbUp();
	UFUNCTION()
	void EdgeClimbing();
	
	//
	UFUNCTION()
	void OnHealthChange(AActor* InstigatorActor, UAWAttributeComp* AttributeComponent, float NewHealth, float Change);

	UFUNCTION(Exec)
	void HealSelf(float v = 10000.f);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool GetIsClimbing() const;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	UAWAttributeComp* GetOwningAttribute() const;

	UFUNCTION(BlueprintCallable)
	UAwActionComponent* GetOwningAction() const;

	FCollisionQueryParams GetIgnoreCollisionParams() const;
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UAwCharacterMovementComponent* GetAwCharacterMovement() const { return AwCharacterMovementComp; }
};
