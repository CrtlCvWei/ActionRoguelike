// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AWPowerUpActorBase.h"
#include "GameFramework/Actor.h"
#include "AWHealPoint.generated.h"

class UAWAttributeComp;
UCLASS()
class AAWHealPoint : public AAWPowerUpActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAWHealPoint();

protected:
	
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnRep_Visble() override;
	
	virtual void Interact_Implementation(APawn* InstigorPawn) override;

public:

	

	virtual void PostInitializeComponents() override;
};
