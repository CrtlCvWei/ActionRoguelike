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
	
	UPROPERTY(ReplicatedUsing=OnRep_Visble,Blueprintable,EditAnywhere)
	bool visible;
	
	UPROPERTY(EditAnywhere,Blueprintable)
	float SpawnTime;
	FTimerHandle InvisibleTimerHandle;
	
	UPROPERTY(VisibleAnywhere, Category="Componet")
	USphereComponent* SphereComp;
	UPROPERTY(VisibleAnywhere, Category="Componet")
	UStaticMeshComponent* MeshComp;

	UFUNCTION()
	virtual void OnRep_Visble();
	
	UFUNCTION(BlueprintCallable)
	void SetVisble(bool bNewState );
	UFUNCTION(BlueprintCallable)
	bool GetVisble() const { return visible; }
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server,Unreliable, BlueprintCallable)
	void CoolDown();
	UFUNCTION(Server,Unreliable,BlueprintCallable)
	void ShowUp();
	
	UFUNCTION()
	virtual void Interact_Implementation(APawn* InstigorPawn) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
