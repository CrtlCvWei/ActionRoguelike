// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "MyGAS/AwActionEffect.h"
#include "AwActionComponent.generated.h"


class UAwAction;
class UAWAttributeComp;

USTRUCT(BlueprintType)
struct FAwEffectRecorder	
{
	GENERATED_BODY()
	
	UPROPERTY()
	AActor* Insigator;

	UPROPERTY()
	UAWAttributeComp* AttributeComp;

	UPROPERTY()
	UAwActionEffect* Effect;

	UPROPERTY()
	FTimerHandle TimerHandle;

	explicit FAwEffectRecorder() = default;
	
	explicit  FAwEffectRecorder(AActor* ins, UAWAttributeComp* att,UAwActionEffect* eff,FTimerHandle& timer):
		Insigator(ins),
		AttributeComp(att),
		Effect(eff),
		TimerHandle(timer)
	{}

	FAwEffectRecorder& operator=(const FAwEffectRecorder& other)
	{
		if(this == &other)
			return *this;
		Insigator = other.Insigator;
		AttributeComp = other.AttributeComp;
		Effect = other.Effect;
		TimerHandle = other.TimerHandle;
		return *this;
	}
	
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UAwActionComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	AActor* OwningActor;

protected:

	/* Granted abilities at game start */
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Actions")
	TArray<TSubclassOf<UAwAction>> DefaultActions;
	
	UPROPERTY(Replicated,EditAnywhere,BlueprintReadOnly, Category="Actions")
	TArray<TObjectPtr<UAwAction>> Actions;

	
	UPROPERTY()
	TMap<FName, FAwEffectRecorder> EffectObjectsPools; // 

	UFUNCTION()
	void OnRep_Actions();
	
	virtual void BeginPlay() override;


public:

	UFUNCTION(BlueprintCallable)
	void CheckActions() const ;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tags")
	FGameplayTagContainer ActiveGameplayTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tags")
	FGameplayTagContainer BlockGamePlayTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tags")
	FGameplayTagContainer CoolDownGamePlayTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tags")
	FGameplayTagContainer EffectsGamePlayTags;
	
	// Sets default values for this component's properties
	UAwActionComponent();
	UFUNCTION(Server,Reliable,BlueprintCallable, Category="Actions")
	void AddAction(TSubclassOf<UAwAction> ActionClass);

	UFUNCTION(Server,Reliable,BlueprintCallable, Category="Actions")
	void RemoveAction(UAwAction* ActionToRemove);

	UFUNCTION(BlueprintCallable, Category="Effects")
	bool ApplyEffect(const FAwGameplayEffectContext& EffectContext, UAWAttributeComp* AttributeComp);

	
	
	UFUNCTION(BlueprintCallable, Category="Actions")
	void StartActionByName(AActor* Instigator, FName ActionName);
	UFUNCTION(BlueprintCallable, Category="Actions")
	void StopActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(Server,Reliable,BlueprintCallable, Category="Actions")
	void ServeStartActionByName(AActor* Instigator, FName ActionName);
	UFUNCTION(Server,Reliable,BlueprintCallable, Category="Actions")
	void ServeStopActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(Blueprintable)
	UAwAction* GetActionByName(FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "GameplayEffects")
	FAwGameplayEffectContextHandle MakeEffectContex(AActor* Causer, UAwAction* Action);

	UFUNCTION(BlueprintCallable)
	bool BindCoolDownDelegate(FName ActionName, UAwUserWidget* Widget);

	UFUNCTION(BlueprintCallable, Category="Actions")
	bool SetOwningActor();

	UFUNCTION(BlueprintCallable, Category="Actions")
	AActor* GetOwningActor() const { return OwningActor; }

	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<UAwAction>> GetDefaultActions() const { return DefaultActions; }
	
	UFUNCTION(Blueprintable)
	void ApplyInstanceEffects(UAwActionEffect* Effect, AActor* Insigator, UAWAttributeComp* AttributeComp);

	UFUNCTION(Blueprintable)
	void ApplyPeriodicEffects(UAwActionEffect* Effect, AActor* Insigator,
	                          UAWAttributeComp* AttributeComp);

	UFUNCTION(Blueprintable)
	void RemovePeriodicEffects(UAwActionEffect* Effect, AActor* Insigator,
	                           UAWAttributeComp* AttributeComp, FTimerHandle& DurationTimerHandle,
	                           FTimerHandle& PeriodTimerHandle);

	UFUNCTION(Blueprintable)
	void RemovePeriodicEffectsByForce(UAwActionEffect* Effect);


	UFUNCTION(Blueprintable)
	void ApplyDurationEffects(UAwActionEffect* Effect, AActor* Insigator,
							  UAWAttributeComp* AttributeComp);

	UFUNCTION(Blueprintable)
	void RemoveDurationEffects(UAwActionEffect* Effect, AActor* Insigator,
							   UAWAttributeComp* AttributeComp, FTimerHandle& DurationTimerHandle,
							   FTimerHandle& PeriodTimerHandle);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
