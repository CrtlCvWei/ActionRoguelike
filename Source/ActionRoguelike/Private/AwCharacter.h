// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AWInteractionComponent.h"
#include "GameFramework/Character.h"
// #include "MyGAS/AwAttributeSet.h"
#include "AwCharacter.generated.h"

class UAwActionComponent;
class UCameraComponent;
class USpringArmComponent;
class UAWInteractionComponent;
class UAWAttributeComp;
class UAnimMontage;


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


public:
	// Sets default values for this character's properties
	AAwCharacter();

protected:
	
	UPROPERTY()
	UArrowComponent* ArrowComp;
	
	UPROPERTY()
	bool bIsClimbing;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Represents a camera viewpoint and settings, such as projection type, field of view, and post-process overrides.
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UAWInteractionComponent* InteractionComp;

	// GAS demo
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UAwActionComponent* ActionComp;;
	// GAS demo
	// UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	// UAwAttributeSet* MyAttributeSet;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ClimbAndVault")
	FName RightKneeSocketName;
	
	
	//Basic Actions
	void MoveForward(float Values);
	void MoveRight(float Values);
	virtual void Jump() override;
	virtual void ClearJumpInput(float DeltaTime) override;
	virtual void StopJumping() override;
	//Attack Action
	
	void PrimaryAttack();
	void BlackHoleAbility();
	void BeginSprint();
	void EndSprint();
	
	
	//Interact
	void PrimaryInterat();
	FVector GetPawnViewLocation() const override;
	
	void Deadth();
	//Camera Shake
	// TSubclassOf<UCameraShake> CameraShakeClass = UMyCameraShake::StaticClass();

	// Trigger the camera shake effect
	// APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	// if (PlayerController)
	// {
	// 	PlayerController->ClientPlayCameraShake(CameraShakeClass);
	// }

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
	void ClimbingTick();
	
	FRotator ClimbRotator = FRotator::ZeroRotator;
	bool DetectWall();
	FRotator FindClimbRotation(const FVector StartLoca,const FVector EndLoca, FVector ObstacleNormalVec); 
	
	
	UFUNCTION(BlueprintCallable)
	void SwitchToClimbingMode();
	UFUNCTION(BlueprintCallable)
	void RidOffClimbingMode();
	UFUNCTION()
	void MoveForwardWhenClimbing(float Values);
	UFUNCTION()
	void MoveRightWhenClimbing(float Values);
	UFUNCTION()
	bool DetectAndClimbUp();
	
	//
	UFUNCTION()
	void OnHealthChange(AActor* InstigatorActor, UAWAttributeComp* AttributeComponent, float NewHealth, float Change);

	UFUNCTION(Exec)
	void HealSelf(float v = 100);


	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool GetIsClimbing() const { return bIsClimbing; }
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	UAWAttributeComp* GetOwningAttribute() const;
	
};
