// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AWInteractionComponent.h"
#include "GameFramework/Character.h"
#include "AwCharacter.generated.h"

class UAwActionComponent;
class UCameraComponent;
class USpringArmComponent;
class UAWInteractionComponent;
class UAWAttributeComp;
class UAnimMontage;



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
	const float BrakingDecelerationWalking = 512.f;
	
	UFUNCTION()
	void Init_Paramters();
	
	UFUNCTION()
	virtual void Init_BeginPlay();

protected:

	UPROPERTY(EditAnywhere,Category = "Attack")
	TSubclassOf<AActor> BlackHoleClass;
	UPROPERTY(EditAnywhere,Category = "Attack")
	UAnimMontage* AttackAni;

public:
	// Sets default values for this character's properties
	AAwCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Represents a camera viewpoint and settings, such as projection type, field of view, and post-process overrides.
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UAWInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UAWAttributeComp* AttributeComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UAwActionComponent* ActionComp;;
	
		
	//Sets Character Movement Speed to Sprint values.
	void BeginSprint();

	//Sets Character Movement Speed back to default speed values.
	void EndSprint();

	
	//Basic Actions
	void MoveForward(float Values);
	void MoveRight(float Values);
	virtual void Jump() override;
	virtual void StopJumping() override;
	//Attack Action
	FRotator GetProjectileRotation(FVector HandLocation);
	void SpawnProjectile(TSubclassOf<AActor> ClassToSpawn);
	void PrimaryAttack();
	void BlackHoleAbility();
	void BlackHoleAbility_TimeElasped();
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
	void UpdateJumpKeyHoldTime();

	//
	UFUNCTION()
	void OnHealthChange(AActor* InstigatorActor, UAWAttributeComp* AttributeComponent, float NewHealth, float Change);

	UFUNCTION(Exec)
	void HealSelf(float v = 100);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;
};
