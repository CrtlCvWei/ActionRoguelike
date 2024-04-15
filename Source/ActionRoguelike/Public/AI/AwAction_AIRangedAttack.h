// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AwAction.h"
#include "AwAction_AIRangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API UAwAction_AIRangedAttack : public UAwAction
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere,Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere,Category = "Attack")
	UAnimMontage* AttackAni;

	UPROPERTY(EditAnywhere,Category = "Effects")
	FName HandSpawnSocketName;

	UPROPERTY(EditAnywhere,Category = "Effects")
	UParticleSystem* CastingEffect;

	UPROPERTY(EditAnywhere,Category = "Attack")
	float AttackTimeDelay;

	UPROPERTY()
	AActor* Target;
	
	UFUNCTION()
	void StartActionTimeEnasped(AActor* Instigator);
public:

	void SetTarget(AActor* NewTarget) { Target = NewTarget; }
	
	virtual  bool CheckActionAvailable(AActor* Instigator) const override;
	virtual void  StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	
	
};
