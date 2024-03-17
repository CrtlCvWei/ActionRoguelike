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
	UPROPERTY(VisibleAnywhere, Category="Force")
	float ProjectileGravityScale = -0.01f;
	UPROPERTY(VisibleAnywhere, Category="Force")
	float InitialSpeed = 500.f;

	float AttractionRange = 500.f;
	float AttractionStrength = -1000.f;

	// UFUNCTION(Blueprintable)
	virtual void OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void PerformAttraction() const;
	UFUNCTION(BlueprintCallable)
	void AttractionInterval();


protected:
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle AttractionStartTimerHandle;
	UPROPERTY(BlueprintReadOnly)
	FTimerHandle AttractionTimerHandle;

	UPROPERTY(VisibleAnywhere, Category="Force")
	URadialForceComponent* RForce;

	void Init_Paramters();

	virtual  void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

	// Sets default values for this actor's properties
	AAWBlackHolePj();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetAttractionRange(const float v);
	void SetAttractionStrength(const float v);
	void SetInitSpeed(const float v);
	void SetGravityScale(const float v);

	void Explode();

};
