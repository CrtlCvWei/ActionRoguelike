// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "AWProjectileBase.generated.h"

class UNiagaraComponent;
class UProjectileMovementComponent;
class USphereComponent;
class USoundCue;

UCLASS()
class AAWProjectileBase : public AActor
{
	GENERATED_BODY()

private:
	const float ProjectileGravityScale = -0.01f;
	const float InitialSpeed = 500.f;

public:
	// Sets default values for this actor's properties
	AAWProjectileBase();

protected:
	UPROPERTY(EditAnywhere, Category = "Damage")
	float damage = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	TSubclassOf<UCameraShakeBase> ImpactShake;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> ImpactVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<USoundCue> ImpactSound;

	void Init_Paramters();
	//碰撞组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComp;
	//移动组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* MoveComp;
	//粒子组件

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ParticalEffect")
	TObjectPtr<UNiagaraComponent> ParticaleComp;

	//音效组件
	UPROPERTY(VisibleAnywhere, Category="Sound")
	UAudioComponent* VanishAudio;

	UFUNCTION(Blueprintable,BlueprintNativeEvent)
	void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                            FVector NormalImpluse, const FHitResult& Hit);
	UFUNCTION(Blueprintable,BlueprintNativeEvent)
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual  void OnComponentHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor,
													UPrimitiveComponent* OtherComp, FVector NormalImpluse,
													const FHitResult& Hit);
	UFUNCTION()
	virtual  void OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
													UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
													bool bFromSweep, const FHitResult& SweepResult);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(Blueprintable, BlueprintCallable)
	virtual void Explode();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
