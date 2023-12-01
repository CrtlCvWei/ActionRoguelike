// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AwAction.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UAwAction : public UObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere,Category="Name")
	FName ActionName;
	
public:
	UFUNCTION(BlueprintNativeEvent,Category="Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent,Category="Action")
	void StopAction(AActor* Instigator);
	UFUNCTION()
	FName GetActionName () const;

	UWorld* GetWorld() const override;
};
