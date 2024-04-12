// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGAS/AwActionEffect.h"
#include "Blueprint/AwBlueprintFunctionLibrary.h"
#include "AwAction.h"
#include "MyGAS/AwActionComponent.h"

void FAwGameplayEffectContext::GetOwnedGameplayTags(FGameplayTagContainer& ActorTagContainer,
                                                    FGameplayTagContainer& SpecTagContainer) const
{
}

void FAwGameplayEffectContext::AddInstigator(AActor* InInstigator, AActor* InEffectCauser)
{
	Instigator = InInstigator;
	EffectCauser = InEffectCauser;
	// Set the ability system component for the instigator
	InstigatorAbilitySystemComponent = UAwBlueprintFunctionLibrary::GetAwActionComponent(InInstigator, true);
}

void FAwGameplayEffectContext::SetAbility(const UAwAction* InGameplayAbility)
{
	if (InGameplayAbility)
	{
		AbilityInstanceNotReplicated = MakeWeakObjectPtr(const_cast<UAwAction*>(InGameplayAbility));
		AbilityCDO = InGameplayAbility->GetClass()->GetDefaultObject<UAwAction>();
		AbilityLevel = InGameplayAbility->GetAbilityLevel();
	}
}

AActor* FAwGameplayEffectContext::GetInstigator() const
{
	return Instigator.Get();
}

UAwAction* FAwGameplayEffectContext::GetAbility() const
{
	return AbilityInstanceNotReplicated.Get();
}

const UAwAction* FAwGameplayEffectContext::GetAbilityInstance_NotReplicated() const
{
	return AbilityInstanceNotReplicated.Get();
}

UAwActionComponent* FAwGameplayEffectContext::GetInstigatorAbilitySystemComponent() const
{
	return InstigatorAbilitySystemComponent.Get();
}

UAwActionComponent* FAwGameplayEffectContext::GetOriginalInstigatorAbilitySystemComponent() const
{
	return InstigatorAbilitySystemComponent.Get();
}

AActor* FAwGameplayEffectContext::GetEffectCauser() const
{
	return EffectCauser.Get();
}

AActor* FAwGameplayEffectContext::GetOriginalInstigator() const
{
	return Instigator.Get();
}

void FAwGameplayEffectContext::AddSourceObject(const UObject* NewSourceObject)
{
	SourceObject = MakeWeakObjectPtr(const_cast<UObject*>(NewSourceObject));
	bReplicateSourceObject = NewSourceObject && NewSourceObject->IsSupportedForNetworking();
}

UObject* FAwGameplayEffectContext::GetSourceObject() const
{
	return SourceObject.Get();
}

FString FAwGameplayEffectContext::ToString() const
{
	const AActor* InstigatorPtr = Instigator.Get();
	return (InstigatorPtr ? InstigatorPtr->GetName() : FString(TEXT("NONE")));
}

UScriptStruct* FAwGameplayEffectContext::GetScriptStruct() const
{
	return FAwGameplayEffectContext::StaticStruct();
}

bool FAwGameplayEffectContext::IsLocallyControlled() const
{
	APawn* Pawn = Cast<APawn>(Instigator.Get());
	if (!Pawn)
	{
		Pawn = Cast<APawn>(EffectCauser.Get());
	}
	if (Pawn)
	{
		return Pawn->IsLocallyControlled();
	}
	return false;
}

bool FAwGameplayEffectContext::IsLocallyControlledPlayer() const
{
	APawn* Pawn = Cast<APawn>(Instigator.Get());
	if (!Pawn)
	{
		Pawn = Cast<APawn>(EffectCauser.Get());
	}
	if (Pawn && Pawn->Controller)
	{
		return Pawn->Controller->IsLocalPlayerController();
	}
	return false;
}

bool FAwGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	// TODO： Implement this later
	return true;
}

bool FAwGameplayEffectContext::CanActorReferenceBeReplicated(const AActor* Actor)
{
	// We always support replication of null references and stably named actors
	if (!Actor || Actor->IsFullNameStableForNetworking())
	{
		return true;
	}

	// If we get here this is a dynamic object and we only want to replicate the reference if the actor is set to replicate, otherwise the resolve on the client will constantly fail
	const bool bIsSupportedForNetWorking = Actor->IsSupportedForNetworking();
	const bool bCanDynamicReferenceBeReplicated = bIsSupportedForNetWorking && Actor->GetIsReplicated();

#if !UE_BUILD_SHIPPING
	// Optionally trigger warning if we are trying to replicate a reference to an object that never will be resolvable on receiving end
	if ((!bCanDynamicReferenceBeReplicated && bIsSupportedForNetWorking))
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Attempted to replicate a reference to dynamically spawned object that is set to not replicate %s."
		       ), *(Actor->GetName()));
	}
#endif

	return bCanDynamicReferenceBeReplicated;
}

void FAwGameplayEffectContext::AddActors(const TArray<TWeakObjectPtr<AActor>>& InActors, bool bReset)
{
	if (bReset && Actors.Num())
	{
		Actors.Reset();
	}
	Actors.Append(InActors);
}

const TArray<TWeakObjectPtr<AActor>>& FAwGameplayEffectContext::GetActors() const
{
	return Actors;
}

void FAwGameplayEffectContext::AddOrigin(FVector InOrigin)
{
	bHasWorldOrigin = true;
	WorldOrigin = InOrigin;
}

const FVector& FAwGameplayEffectContext::GetOrigin() const
{
	return WorldOrigin;
}

bool FAwGameplayEffectContext::HasOrigin() const
{
	return bHasWorldOrigin;
}

/*		*****************************		*/

void UAwActionEffect::SetEffectMap(const FName& Name, const float Value)
{
	if (EffectMap.FindChecked(Name))
	{
		EffectMap[Name] = Value;
		return;
	}
	EffectMap.Add(Name, Value);
}

void UAwActionEffect::SetEffectMapForAction(const FName& Name, const float Value)
{
	if (Name != FName("CoolDownTime") || Name != FName("Cost") || Name != FName("AbilityLevel"))
		return;

	if (EffectMapForAction.FindChecked(Name))
	{
		EffectMapForAction[Name] = Value;
		return;
	}
	EffectMapForAction.Add(Name, Value);
}
