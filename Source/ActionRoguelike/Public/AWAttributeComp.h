// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AWAttributeComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChange, AActor*, InstigatorActor, UAWAttributeComp*,
                                              AttributeComp, float, NewHealth, float, Change);

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
public:
	UFUNCTION(BlueprintCallable)
	bool SetHealth(float v, AActor* Sourcer);

	UFUNCTION(BlueprintCallable)
	bool isAlive() const;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return Max_Health; }

	UPROPERTY(BlueprintAssignable)
	FOnHealthChange OnHealthChange;

	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
