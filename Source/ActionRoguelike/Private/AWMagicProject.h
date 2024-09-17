// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AWProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "AWMagicProject.generated.h"

UCLASS()
class AAWMagicProject : public AAWProjectileBase
{
	GENERATED_BODY()

private:
	const float ProjectileGravityScale = 0.0;
	const float InitialSpeed = 1000.f;

public:
	// Sets default values for this actor's properties
	AAWMagicProject();

protected:
	void Init_Paramters();
	
	virtual void AwGamePlayEffectImpact(AActor* Effector) override;

	virtual  void OnComponentHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor,
													UPrimitiveComponent* OtherComp, FVector NormalImpluse,
													const FHitResult& Hit) override;
	
	virtual  void OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
													UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
													bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

};
