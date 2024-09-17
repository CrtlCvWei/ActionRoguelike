// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGAS/AwActionEffect.h"
#include "MyGAS/AwAttributeSet.h"
#include "AwAction.generated.h"

class UAWAttributeComp;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAwActionCoolDownStartSignture, UAwAction*, Action, float, CoolDownTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAwActionCoolDownEndSignture, UAwAction*,Action);
struct FAwAttributeData;
/**
 * 
 */
USTRUCT()
struct FAwActionRepData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	bool bIsRunning;
	UPROPERTY()
	TObjectPtr<AActor> Instigator;
	 
	FAwActionRepData() = default;
	FAwActionRepData(bool IsRunning,AActor* Inst):bIsRunning(IsRunning),Instigator(Inst){}
};



UCLASS(Blueprintable)
class UAwAction : public UObject
{
	GENERATED_BODY()
	
	FOnAwActionCoolDownStartSignture OnCoolDownStart;

	FOnAwActionCoolDownEndSignture OnCoolDownEnd;
	
protected:
	UPROPERTY(EditAnywhere, Category="Idnetify")
	FName ActionName;

	UPROPERTY(EditAnywhere, Category="Idnetify")
	bool bAuto = false;

	UPROPERTY(ReplicatedUsing=OnRep_RepData)
	FAwActionRepData RepData;

	UFUNCTION()
	void OnRep_RepData();
	
	/* Cool Down Sys */
	UPROPERTY()
	FTimerHandle CoolDownTimerHandle;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="CoolDown")
	FAwAttributeData CoolDownTimeAttributeData;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="CoolDown")
	FAwAttributeData  ManaCost;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Level")
	int32 AbilityLevel = 1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Cost")
	FAwAttributeData CostAttributeData;

	UPROPERTY(EditAnywhere, Category="GamePlay Effect")
	TArray<TSubclassOf<UAwActionEffect>> EffectsClass;

	UPROPERTY(VisibleDefaultsOnly,Category="GamePlay Effect")
	TArray<UAwActionEffect*> EffectInstances;
	
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

	UFUNCTION(BlueprintCallable, Category = "Action")
	UAwActionComponent* GetOwningComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	UAWAttributeComp* GetOwningAttribute() const;

	UFUNCTION(BlueprintCallable, Category="Action")
	int32 GetAbilityLevel() const { return AbilityLevel; }

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category="Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category="Action")
	void StopAction(AActor* Instigator);

	UFUNCTION()
	bool IsAuto() const { return bAuto; }

	UFUNCTION()
	bool IsRunning() const { return RepData.bIsRunning; }
	
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

	UFUNCTION()
	void CreateEffectInstances();

	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};


