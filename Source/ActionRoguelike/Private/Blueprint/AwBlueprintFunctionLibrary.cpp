// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprint/AwBlueprintFunctionLibrary.h"

#include "AwCharacter.h"
#include "AWPlayerState.h"
#include "MyGAS/AWAttributeComp.h"

bool UAwBlueprintFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	
	if(AAwCharacter* Player = Cast<AAwCharacter>(TargetActor))
	{
		UAWAttributeComp* AttributeComp =  Player->GetOwningAttribute();
		if (AttributeComp)
		{
			return AttributeComp->SetHealth( -DamageAmount,DamageCauser);
		}
	}
	return false;
}

bool UAwBlueprintFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount,
	const FHitResult& HitResult)
{
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			// Direction = Target - Origin
			FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
			Direction.Normalize();

			HitComp->AddImpulseAtLocation(Direction * 300000.f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}
	return false;
}

UAwActionComponent* UAwBlueprintFunctionLibrary::GetAwActionComponent(AActor* Actor,bool FindOther)
{
	if (Actor == nullptr)
	{
		return nullptr;
	}

	//May in the PlaerState
	const AAwCharacter* AC = Cast<AAwCharacter>(Actor);
	if(AC)
	{
		const AAWPlayerState* AwPS = Cast<AAWPlayerState>(AC->GetPlayerState());
		if (AwPS)
		{
			return Cast<UAwActionComponent>(AwPS->GetPlayerAction());
		}
	}
	if(FindOther)
	{
		// Fall back to a component search to better support BP-only actors
		return Actor->FindComponentByClass<UAwActionComponent>();
	}
	return nullptr;
}

UAWAttributeComp* UAwBlueprintFunctionLibrary::GetAwAttributeComponent(AActor* TargetActor, bool FindOther)
{
	if (TargetActor == nullptr)
	{
		return nullptr;
	}

	//May in the PlaerState
	const AAwCharacter* AC = Cast<AAwCharacter>(TargetActor);
	if(AC)
	{
		const AAWPlayerState* AwPS = Cast<AAWPlayerState>(AC->GetPlayerState());
		if (AwPS)
		{
			return Cast<UAWAttributeComp>(AwPS->GetPlayerAttribute());
		}
	}
	if(FindOther)
	{
		// Fall back to a component search to better support BP-only actors
		return TargetActor->FindComponentByClass<UAWAttributeComp>();
	}
	return nullptr;
}
