// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AWProjectileBase.h"
#include "AWAIProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AAWAIProjectile : public AAWProjectileBase
{
	GENERATED_BODY()
private:
	const float ProjectileGravityScale = -0.01f;
	const float InitialSpeed = 500.f;

protected:

	void Init_Paramters();
	
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
								FVector NormalImpluse, const FHitResult& Hit) override ;

public:
	virtual void PostInitializeComponents() override;
	AAWAIProjectile();
};
