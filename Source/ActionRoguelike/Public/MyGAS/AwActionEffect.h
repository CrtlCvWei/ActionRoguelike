// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
// #include "GameplayAbilities/Public/GameplayEffect.h"
// #include "AbilitySystemComponent.h"
#include "AwActionEffect.generated.h"

class UAwAction;
class UAwActionComponent;
/** 
 */

UENUM(BlueprintType)
enum DurationPolicy
{
	Duration,
	Instant,
	infinite
};

/**
 * Data structure that stores an instigator and related data, such as positions and targets
 * Games can subclass this structure and add game-specific information
 * It is passed throughout effect execution so it is a great place to track transient information about an execution
 */
USTRUCT(BlueprintType)
struct ACTIONROGUELIKE_API FAwGameplayEffectContext
{
	GENERATED_BODY()
	FAwGameplayEffectContext()
		: AbilityLevel(1)
		  , WorldOrigin(ForceInitToZero)
		  , bHasWorldOrigin(false)
		  , bReplicateSourceObject(false)
		  , bReplicateInstigator(false)
		  , bReplicateEffectCauser(false)
	{
	}

	FAwGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: AbilityLevel(1)
		  , WorldOrigin(ForceInitToZero)
		  , bHasWorldOrigin(false)
		  , bReplicateSourceObject(false)
		  , bReplicateInstigator(false)
		  , bReplicateEffectCauser(false)
	{
		FAwGameplayEffectContext::AddInstigator(InInstigator, InEffectCauser);
	}

	virtual ~FAwGameplayEffectContext()
	{
	}

	/** Returns the list of gameplay tags applicable to this effect, defaults to the owner's tags */
	virtual void GetOwnedGameplayTags(OUT FGameplayTagContainer& ActorTagContainer,
	                                  OUT FGameplayTagContainer& SpecTagContainer) const;

	/** Sets the instigator and effect causer. Instigator is who owns the ability that spawned this, EffectCauser is the actor that is the physical source of the effect, such as a weapon. They can be the same. */
	virtual void AddInstigator(class AActor* InInstigator, class AActor* InEffectCauser);

	/** Sets the ability that was used to spawn this */
	virtual void SetAbility(const UAwAction* InGameplayAbility);

	/** Returns the immediate instigator that applied this effect */
	virtual AActor* GetInstigator() const;


	/** Returns the CDO of the ability used to instigate this context */
	UAwAction* GetAbility() const;

	/** Returns the specific instance that instigated this, may not always be set */
	const UAwAction* GetAbilityInstance_NotReplicated() const;

	/** Gets the ability level this was evaluated at */
	int32 GetAbilityLevel() const
	{
		return AbilityLevel;
	}

	/** Returns the ability system component of the instigator of this effect */
	virtual UAwActionComponent* GetInstigatorAbilitySystemComponent() const;

	/** Returns the ability system component of the instigator that started the whole chain */
	virtual UAwActionComponent* GetOriginalInstigatorAbilitySystemComponent() const;


	/** Returns the physical actor tied to the application of this effect */
	virtual AActor* GetEffectCauser() const;


	/** Modify the effect causer actor, useful when that information is added after creation */
	void SetEffectCauser(AActor* InEffectCauser)
	{
		EffectCauser = InEffectCauser;
		bReplicateEffectCauser = CanActorReferenceBeReplicated(InEffectCauser);
	}

	/** Should always return the original instigator that started the whole chain. Subclasses can override what this does */
	virtual AActor* GetOriginalInstigator() const;

	/** Sets the object this effect was created from. */
	virtual void AddSourceObject(const UObject* NewSourceObject);

	/** Returns the object this effect was created from. */
	virtual UObject* GetSourceObject() const;

	/** Add actors to the stored actor list */
	virtual void AddActors(const TArray<TWeakObjectPtr<AActor>>& IActor, bool bReset = false);

	/** Returns actor list, may be empty */
	virtual const TArray<TWeakObjectPtr<AActor>>& GetActors() const;
	/** Adds an origin point */
	virtual void AddOrigin(FVector InOrigin);

	/** Returns origin point, may be invalid if HasOrigin is false */
	virtual const FVector& GetOrigin() const;

	/** Returns true if GetOrigin will give valid information */
	virtual bool HasOrigin() const;

	/** Returns debug string */
	virtual FString ToString() const;

	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const;


	/** True if this was instigated by a locally controlled actor */
	virtual bool IsLocallyControlled() const;

	/** True if this was instigated by a locally controlled player */
	virtual bool IsLocallyControlledPlayer() const;

	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

protected:
	static bool CanActorReferenceBeReplicated(const AActor* Actor);

	// The object pointers here have to be weak because contexts aren't necessarily tracked by GC in all cases

	/** Instigator actor, the actor that owns the ability system component */
	UPROPERTY()
	TWeakObjectPtr<AActor> Instigator;

	/** The physical actor that actually did the damage, can be a weapon or projectile */
	UPROPERTY()
	TWeakObjectPtr<AActor> EffectCauser;

	/** The ability CDO that is responsible for this effect context (replicated) */
	UPROPERTY()
	TWeakObjectPtr<UAwAction> AbilityCDO;

	/** The ability instance that is responsible for this effect context (NOT replicated) */
	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UAwAction> AbilityInstanceNotReplicated;

	/** The level this was executed at */
	UPROPERTY()
	int32 AbilityLevel;

	/** Object this effect was created from, can be an actor or static object. Useful to bind an effect to a gameplay object */
	UPROPERTY()
	TWeakObjectPtr<UObject> SourceObject;

	/** The ability system component that's bound to instigator */
	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UAwActionComponent> InstigatorAbilitySystemComponent;

	/** Actors referenced by this context */
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> Actors;

	/** Stored origin, may be invalid if bHasWorldOrigin is false */
	UPROPERTY()
	FVector WorldOrigin;

	UPROPERTY()
	uint8 bHasWorldOrigin : 1;

	/** True if the SourceObject can be replicated. This bool is not replicated itself. */
	UPROPERTY(NotReplicated)
	uint8 bReplicateSourceObject : 1;

	/** True if the Instigator can be replicated. This bool is not replicated itself. */
	UPROPERTY(NotReplicated)
	uint8 bReplicateInstigator : 1;

	/** True if the Instigator can be replicated. This bool is not replicated itself. */
	UPROPERTY(NotReplicated)
	uint8 bReplicateEffectCauser : 1;
};

/**
 * Handle that wraps a FGameplayEffectContext or subclass, to allow it to be polymorphic and replicate properly
 */
struct FAwGameplayEffectContextHandleAccessorForNetSerializer;

USTRUCT(BlueprintType)
struct ACTIONROGUELIKE_API FAwGameplayEffectContextHandle
{
	GENERATED_BODY()
	FAwGameplayEffectContextHandle()
	{
	}

	FAwGameplayEffectContextHandle(FAwGameplayEffectContext* _Data): Data(_Data)
	{
	}

	virtual ~FAwGameplayEffectContextHandle()
	{
	}

	void Clear() { Data.Reset(); }
	bool IsValid() const { return Data.IsValid(); }

	/** Returns Raw effet context, may be null */
	FAwGameplayEffectContext* Get()
	{
		return IsValid() ? Data.Get() : nullptr;
	}

	const FAwGameplayEffectContext* Get() const
	{
		return IsValid() ? Data.Get() : nullptr;
	}

	void AddSouceObject(AActor* NewSourceObject)
	{
		if (IsValid())
		{
			Data->AddSourceObject(NewSourceObject);
		}
	}

	void AddInstigator(AActor* Instigator,AActor* Causer)
	{
		if (IsValid())
		{
			Data->AddInstigator(Instigator, Causer);
		}
	}

	
	// Set the ability that was used to spawn this effect
	void SetAbility(UAwAction* InGameplayAbility)
	{
		if (IsValid())
		{
			Data->SetAbility(InGameplayAbility);
		}
	}

private:
	friend FAwGameplayEffectContextHandleAccessorForNetSerializer;

	TSharedPtr<FAwGameplayEffectContext> Data;
};

struct ACTIONROGUELIKE_API FAwGameplayEffectContextHandleAccessorForNetSerializer
{
	static TSharedPtr<FAwGameplayEffectContext>& GetItem(FAwGameplayEffectContextHandle& Source)
	{
		return Source.Data;
	}
};

/*                          */

UCLASS(Blueprintable)
class ACTIONROGUELIKE_API UAwActionEffect : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Effect")
	TMap<FName, float> EffectMap; // FNAME is the Attribute's name to be effect && float is the value

	UPROPERTY(EditAnywhere, Category="Effect")
	TMap<FName, float> EffectMapForAction; // FNAME is the Action's Property's name to be effect && float is the value
	
	UPROPERTY(EditAnywhere, Category="Effect")
	float Duration = 1.f;

	UPROPERTY(EditAnywhere, Category="Effect")
	float Period = 0.5f;

	UPROPERTY(EditAnywhere, Category="EffectType")
	TEnumAsByte<DurationPolicy> Type;

	UPROPERTY(EditAnywhere, Category="Tags")
	FGameplayTagContainer BuffTags;

public:

	
	FTimerHandle DurationTimerHandle;
	FTimerHandle PeriodTimerHandle;
	
	UFUNCTION(BlueprintCallable)
	TEnumAsByte<DurationPolicy> GetType() { return Type; }

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer& GetBuffTags() { return BuffTags; }

	UFUNCTION(BlueprintCallable)
	float GetDuration() { return Duration; }

	UFUNCTION(BlueprintCallable)
	void SetDuration(const float D) { Duration = D; }
	
	UFUNCTION(BlueprintCallable)
	float GetPeriod() { return Period; }

	UFUNCTION(BlueprintCallable)
	void SetPeriod(const float P) { Period = P; }
	
	UFUNCTION(BlueprintCallable)
	const TMap<FName, float>&  GetEffectMap() { return EffectMap; }

	UFUNCTION(BlueprintCallable)
	void SetEffectMap(const FName& Name, const float Value);

	UFUNCTION(BlueprintCallable)
	const TMap<FName, float>&  GetEffectMapForAction() { return EffectMapForAction; }
	
	UFUNCTION(BlueprintCallable)
	void SetEffectMapForAction(const FName& Name, const float Value);
};
