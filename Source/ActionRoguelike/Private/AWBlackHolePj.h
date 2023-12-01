// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AWProjectileBase.h"
#include "GameFramework/Actor.h"
#include "AWBlackHolePj.generated.h"

class URadialForceComponent;

UCLASS()
class AAWBlackHolePj : public AAWProjectileBase
{
	GENERATED_BODY()

private:
	const float ProjectileGravityScale = -0.01f;
	const float InitialSpeed = 500.f;
	UPROPERTY(VisibleAnywhere, Category="Force")
	float AttractionRange = 500.f;
	UPROPERTY(VisibleAnywhere, Category="Force")
	float AttractionStrength = -1000.f;

	// UFUNCTION(Blueprintable)
	virtual  void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void PerformAttraction();
	UFUNCTION()
	void AttractionInterval();

public:
	// Sets default values for this actor's properties
	AAWBlackHolePj();

protected:
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle AttractionStartTimerHandle;
	UPROPERTY(BlueprintReadOnly)
	FTimerHandle AttractionTimerHandle;

	UPROPERTY(VisibleAnywhere, Category="Force")
	URadialForceComponent* RForce;

	void Init_Paramters();

	void Explode() override;

	void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
