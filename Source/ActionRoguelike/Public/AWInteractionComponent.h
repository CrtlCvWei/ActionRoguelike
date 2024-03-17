// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AWInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UAWInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void PrimaryIntract();
	
	// Sets default values for this component's properties
	UAWInteractionComponent();

protected:

	UPROPERTY(EditAnywhere);
	float InteractionDistance = 1000.f;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
