// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AwAction.h"
#include "AwCharacter.h"
#include "AWMagicProject.h"
#include "AwAction_BlackHoleAbilbity.generated.h"

/**
 * 
 */
UCLASS()
class UAwAction_BlackHoleAbilbity : public UAwAction
{
	GENERATED_BODY()

private:

	FTimerHandle ExplosiveTimerHandle;
	
	UFUNCTION()
	void StartAction_TimeElasped(ACharacter* Instigator);

	UFUNCTION()
	void ExplosiveAndCauseDamage(AActor* Instigator,AActor* Projectile);

	FRotator GetProjectileRotation(FVector HandLocation) const;

	
protected:
	
	UPROPERTY(VisibleAnywhere, Category="Attack")
	float AttractionRange = 500.f;
	UPROPERTY(VisibleAnywhere, Category="Attack")
	float AttractionStrength = -1000.f;
	
	UPROPERTY(EditAnywhere,Category = "Attack")
	float ExplosiveDamage = 100.f;

	UPROPERTY(EditAnywhere,Category = "Attack")
	float ExplosiveTimeDelay = 3.f;

	UPROPERTY(EditAnywhere,Category = "Attack")
	float ProjectileGravityScale = -0.01f;

	UPROPERTY(EditAnywhere,Category = "Attack")
	float InitialSpeed = 500.f;
	
	UPROPERTY(EditAnywhere,Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere,Category = "Attack")
	UAnimMontage* AttackAni;
	
	UPROPERTY(EditAnywhere,Category = "Effects")
	FName HandSpawnSocketName;

	
public:
	UAwAction_BlackHoleAbilbity();
	
	virtual void  StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;

};
