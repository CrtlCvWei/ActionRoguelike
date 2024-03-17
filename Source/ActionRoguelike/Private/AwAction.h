// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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

	UPROPERTY(EditAnywhere,Category="Tags")
	FGameplayTagContainer GrandTags;
	UPROPERTY(EditAnywhere,Category="Tags")
	FGameplayTagContainer BlockTags;
	
public:

	UFUNCTION(Blueprintable,Category = "Action")
	UAwActionComponent* GetOwningComponent() const;
	
	UFUNCTION(BlueprintNativeEvent,Category="Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent,Category="Action")
	void StopAction(AActor* Instigator);
	UFUNCTION()
	FName GetActionName () const;

	UWorld* GetWorld() const override;

	UFUNCTION()
	bool CheckActionAvailable(AActor* Instigator) const;
};
