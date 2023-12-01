// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AWGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "AWPowerUpActorBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class AAWPowerUpActorBase : public AActor,public IAWGameplayInterface
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	AAWPowerUpActorBase();

protected:
	UPROPERTY(EditAnywhere,Blueprintable)
	bool visible;
	UPROPERTY(EditAnywhere,Blueprintable)
	float SpawnTime;
	FTimerHandle InvisibleTimerHandle;
	
	UPROPERTY(VisibleAnywhere, Category="Componet")
	USphereComponent* SphereComp;
	UPROPERTY(VisibleAnywhere, Category="Componet")
	UStaticMeshComponent* MeshComp;

	UFUNCTION(BlueprintCallable)
	void SetVisble(bool bNewState );
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Blueprintable)
	void CoolDown();
	UFUNCTION(Blueprintable)
	void ShowUp();
	
	UFUNCTION()
	virtual void Interact_Implementation(APawn* InstigorPawn) override;
	

};
