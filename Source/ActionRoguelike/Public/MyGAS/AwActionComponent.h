// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "MyGAS/AwActionEffect.h"
#include "AwActionComponent.generated.h"


class UAwAction;
class UAWAttributeComp;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UAwActionComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* OwningActor;

	void ApplyInstanceEffects(UAwActionEffect* Effect,AActor* Insigator,UAWAttributeComp* AttributeComp);
	
protected:
	UPROPERTY()
	TArray<UAwAction*> Actions;

	UPROPERTY()
	TArray<UAwActionEffect*> EffectObjectsPools; // 
	
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tags")
	FGameplayTagContainer ActiveGameplayTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tags")
	FGameplayTagContainer BlockGamePlayTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tags")
	FGameplayTagContainer CoolDownGamePlayTags;

	// Sets default values for this component's properties
	UAwActionComponent();
	UFUNCTION(BlueprintCallable, Category="Actions")
	bool AddAction(TSubclassOf<UAwAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category="Actions")
	bool RemoveAction(UAwAction* ActionToRemove);

	UFUNCTION(BlueprintCallable, Category="Effects")
	bool ApplyEffect(const FAwGameplayEffectContext& EffectContext, UAWAttributeComp* AttributeComp);

	UFUNCTION(BlueprintCallable, Category="Actions")
	void StartActionByName(AActor* Instigator, FName ActionName);
	UFUNCTION(BlueprintCallable, Category="Actions")
	void StopActionByName(AActor* Instigator, FName ActionName);
	UFUNCTION(Blueprintable)
	UAwAction* GetActionByName(FName ActionName);
	
	UFUNCTION(BlueprintCallable, Category = "GameplayEffects")
	FAwGameplayEffectContextHandle MakeEffectContex(AActor* Causer,UAwAction* Action);

	UFUNCTION(BlueprintCallable)
	bool BindCoolDownDelegate(FName ActionName,UAwUserWidget* Widget);
	
	UFUNCTION(BlueprintCallable, Category="Actions")
	bool SetOwningActor();

	UFUNCTION(BlueprintCallable, Category="Actions")
	AActor* GetOwningActor() const{return OwningActor;}

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
