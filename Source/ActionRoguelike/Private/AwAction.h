// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "AwActionComponent.h"
#include "AwAction.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UAwAction : public UObject
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere,Category="Idnetify")
	FName ActionName;

	UPROPERTY(EditAnywhere,Category="Idnetify")
	FGameplayTag ActionTag;
	
	/* Cool Down Sys */
	UPROPERTY()
	FTimerHandle CoolDownTimerHandle;

	UPROPERTY(EditAnywhere,Category="Cost")
	float Cost = 0.f;
	
	UPROPERTY(EditAnywhere,Category="CoolDown")
	float CoolDownTime = 1.f;
	
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

	UFUNCTION()
	void CoolDownOver() const;

	UFUNCTION()
	UWorld* GetWorld() const override;

	UFUNCTION()
	FGameplayTag GetActionTag() const;

	UFUNCTION()
	bool CheckActionAvailable(AActor* Instigator) const;
};
