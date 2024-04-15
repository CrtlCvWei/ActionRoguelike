// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGAS/AwActionEffect.h"
#include "MyGAS/AwAttributeSet.h"
#include "AwAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAwActionCoolDownStartSignture, UAwAction*,Action,float,CoolDownTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAwActionCoolDownEndSignture, UAwAction*,Action);
struct FAwAttributeData;
/**
 * 
 */
UCLASS(Blueprintable)
class UAwAction : public UObject
{
	GENERATED_BODY()
	
	
	FOnAwActionCoolDownStartSignture OnCoolDownStart;

	FOnAwActionCoolDownEndSignture OnCoolDownEnd;
	
protected:
	UPROPERTY(EditAnywhere, Category="Idnetify")
	FName ActionName;

	bool bIsRunning = false;

	/* Cool Down Sys */
	UPROPERTY()
	FTimerHandle CoolDownTimerHandle;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="CoolDown")
	FAwAttributeData CoolDownTimeAttributeData;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Level")
	int32 AbilityLevel = 1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Cost")
	FAwAttributeData CostAttributeData;

	UPROPERTY(EditAnywhere, Category="GamePlay Effect")
	TArray<TSubclassOf<UAwActionEffect>> Effects;

	UPROPERTY(EditAnywhere, Category="Tags")
	FGameplayTagContainer GrandTags;
	UPROPERTY(EditAnywhere, Category="Tags")
	FGameplayTagContainer BlockTags;
	UPROPERTY(EditAnywhere, Category="Idnetify")
	FGameplayTag ActionTag;

	UFUNCTION(BlueprintCallable)
	void InitAtt();

public:
	UAwAction();

	UFUNCTION(Blueprintable, Category = "Action")
	UAwActionComponent* GetOwningComponent() const;

	UFUNCTION(BlueprintCallable, Category="Action")
	int32 GetAbilityLevel() const { return AbilityLevel; }

	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StopAction(AActor* Instigator);

	UFUNCTION()
	FName GetActionName() const;

	UFUNCTION()
	void CoolDownOver();

	UFUNCTION()
	UWorld* GetWorld() const override;

	UFUNCTION()
	FGameplayTag GetActionTag() const;

	UFUNCTION()
	virtual  bool CheckActionAvailable(AActor* Instigator) const;

	FOnAwActionCoolDownStartSignture& GetCoolDownStartDelegate(){ return OnCoolDownStart; };
	FOnAwActionCoolDownEndSignture& GetCoolDownEndDelegate(){ return OnCoolDownEnd;}
		
	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<UAwActionEffect>>& GetActionEffect();
};
