// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AwActionComponent.generated.h"


class UAwAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UAwActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAwActionComponent();

protected:
	UPROPERTY()
	TArray<UAwAction*> Actions; 
	
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable,Category="Actions")
	bool AddAction(TSubclassOf<UAwAction> ActionClass);

	UFUNCTION(BlueprintCallable,Category="Actions")
	void StartActionByName(AActor* Instigator, FName ActionName);
	UFUNCTION(BlueprintCallable,Category="Actions")
	void StopActionByName(AActor* Instigator, FName ActionName);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
