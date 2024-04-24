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

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Attack")
	TObjectPtr<UAnimMontage> AttackAni;
	
	UPROPERTY(EditAnywhere,Category = "Effects")
	FName HandSpawnSocketName = "Muzzle_Front";

	UPROPERTY(EditAnywhere,Category = "Effects")
	UParticleSystem* CastingEffect;

	UPROPERTY(EditAnywhere,Category = "Attack")
	float AttackTimeDelay;

	UFUNCTION()
	void StartActionTimeEnasped(AActor* Instigator);

	// UFUNCTION()
	// void SpawnProjectile(FName NotifyName,const FBranchingPointNotifyPayload& BranchingPointPayload);
	
	FRotator GetProjectileRotation(ACharacter* Instigator,FVector HandLocation);

public:

	UAwAction_ProjecileAttack();
	virtual  bool CheckActionAvailable(AActor* Instigator) const override;
	virtual void  StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;

	UFUNCTION(NetMulticast,Reliable)
	void PlayAttackAni(ACharacter* Player);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
