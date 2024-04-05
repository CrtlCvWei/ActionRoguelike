// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AwAction.h"
#include "AwAction_ProjecileAttack.generated.h"

/**
 * 
 */
UCLASS()
class UAwAction_ProjecileAttack : public UAwAction
{
	GENERATED_BODY()
protected:
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

	UFUNCTION()
	void StartActionTimeEnasped(AActor* Instigator);
	
	FRotator GetProjectileRotation(ACharacter* Instigator,FVector HandLocation);
public:
	
	UAwAction_ProjecileAttack();
	
	virtual void  StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;


};
