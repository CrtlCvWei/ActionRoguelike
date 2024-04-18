// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGAS/AwActionComponent.h"
#include "MyGAS/AWAttributeComp.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AwBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API UAwBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable,Category = "Gameplay")
	static  UAwActionComponent* GetAwActionComponent(AActor* TargetActor,bool FindOther = true);

	UFUNCTION(Blueprintable,Category = "Gameplay")
	static UAWAttributeComp* GetAwAttributeComponent(AActor* TargetActor,bool FindOther = true);
};
