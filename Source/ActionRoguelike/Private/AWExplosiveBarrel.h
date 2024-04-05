// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PhysicsEngine/RadialForceComponent.h"
#include "AWExplosiveBarrel.generated.h"

UCLASS()
class AAWExplosiveBarrel : public AActor
{
	GENERATED_BODY()

private:
	void Init_Paramters();
	const float ExplosiveRadius = 1000.f;
	const float ForceStrength = 500.f;
	const float ImpluseStrength = 6000.f;
	const float ImpluseItself = 1000.f;
	FTimerHandle ExplosiveTimeHandler;

public:
	// Sets default values for this actor's properties
	AAWExplosiveBarrel();

protected:
	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* SMeshComp;
	//
	UPROPERTY(VisibleAnywhere, Category="Components")
	URadialForceComponent* RForce;

	UFUNCTION(BlueprintCallable)
	void WhenActorsHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                   FVector NormalImpluse, const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable)
	void Fire(AActor* OtherActor);

	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
