// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "AWMagicProject.generated.h"

UCLASS()
class AAWMagicProject : public AActor
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

	//碰撞组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComp;
	//移动组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* MoveComp;
	//粒子组件
	UPROPERTY(VisibleAnywhere, Category="Niagara")
	UNiagaraComponent* NiagaraComp;

	//音效组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Sound")
	UAudioComponent* VanishAudio;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    FVector NormalImpluse, const FHitResult& Hit);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual  void OnComponentHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor,
													UPrimitiveComponent* OtherComp, FVector NormalImpluse,
													const FHitResult& Hit);
	
	virtual  void OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
													UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
													bool bFromSweep, const FHitResult& SweepResult);
	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
