// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AWAICharacter.generated.h"

class UAwWorldUserWidget;
class UPawnSensingComponent;
class UAWAttributeComp;
class UAWReward;

UCLASS()
class ACTIONROGUELIKE_API AAWAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAWAICharacter();

protected:

	void Init_Paramters();
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UAWAttributeComp* AttributeComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UAWReward* RewardComp;

	UPROPERTY(VisibleAnywhere,Category="AI")
	UPawnSensingComponent* SensingComp;

	UPROPERTY(EditDefaultsOnly,Category="AI")
	TSubclassOf<UUserWidget> HealthBarClass;

	UAwWorldUserWidget* AIHealthBar;
	
	UFUNCTION()
	virtual void OnHealthChange(AActor* InstigatorActor, UAWAttributeComp* AttributeComponent, float NewHealth, float Change);

	UFUNCTION()
	void SetTarget(APawn* SeenPawn);
	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);
public:


	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	
};
