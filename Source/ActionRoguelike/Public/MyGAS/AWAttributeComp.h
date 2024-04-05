// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Components/ActorComponent.h"
#include "MyGAS/AwAttributeSet.h"
#include "AWAttributeComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChangeSignture, AActor*, InstigatorActor, UAWAttributeComp*,
                                              AttributeComp, float, NewHealth, float, Change);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnManaChangeSignture, AActor*, InstigatorActor, UAWAttributeComp*,
											  AttributeComp, float, NewMana, float, Change);




UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API UAWAttributeComp : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAWAttributeComp();

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Attributes")
	float Max_Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Attributes")
	float Health;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TObjectPtr<UAwAttributeSet> AttributeSet;


public:
	UFUNCTION(BlueprintCallable)
	bool SetHealth(float v, AActor* Sourcer);

	UFUNCTION(BlueprintCallable)
	bool isAlive() const;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return AttributeSet->GetHealth(); }

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const {return AttributeSet->GetMaxHealth(); }

	UFUNCTION(BlueprintCallable)
	UAwAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	UPROPERTY(BlueprintAssignable)
	FOnHealthChangeSignture OnHealthChange;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChangeSignture OnMaxHealthChange;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChangeSignture OnManaChange;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChangeSignture OnMaxManaChange;
	
};
