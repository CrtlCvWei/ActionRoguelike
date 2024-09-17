// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AwAction_AIRangedAttack.h"

#include "AWProjectileBase.h"
#include "AI/AWAICharacter.h"

void UAwAction_AIRangedAttack::StartActionTimeEnasped(AActor* Instigator)
{
	AActor* Projectile = nullptr;
	FVector TargetLocation;
	if (ACharacter* AIPlayer = Cast<ACharacter>(Instigator))
	{
		const FVector SpawnLocation = AIPlayer->GetMesh()->GetSocketLocation(HandSpawnSocketName);
		if (Target)
			TargetLocation = Target->GetActorLocation();
		else
			TargetLocation = AIPlayer->GetActorLocation() + AIPlayer->GetActorForwardVector() * 1000.f;
		const FRotator ProjectileRotation = (TargetLocation - SpawnLocation).Rotation();
		FTransform LocaTM = FTransform((FRotator)ProjectileRotation,
		                               (FVector)SpawnLocation);

		/** Actor will spawn in desired location, regardless of collisions. */
		FActorSpawnParameters SpawnParams;
		/** Actor will spawn in desired location, regardless of collisions. */
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = AIPlayer;
		// Spawn the projectile (Magic balls or any other things...)
		Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, LocaTM, SpawnParams);
	}
	// Set Effects
	if (EffectsClass.Num() > 0)
	{
		// TODO : TRY USE EFFECTS[0] FOR TEST
		// Projectile is null ?
		auto Comp = this->GetOwningComponent();
		if (Comp)
		{
			FAwGameplayEffectContextHandle GamePlayEffect = Comp->MakeEffectContex(Projectile, this);
			auto ProjecileBase = Cast<AAWProjectileBase>(Projectile);
			if (ProjecileBase)
			{
				ProjecileBase->SetEffectContext(GamePlayEffect);
			}
		}
	}
	StopAction_Implementation(Instigator);
}

bool UAwAction_AIRangedAttack::CheckActionAvailable(AActor* Instigator) const
{
	return Super::CheckActionAvailable(Instigator);
}

void UAwAction_AIRangedAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	if (this->ProjectileClass)
	{
		RepData.bIsRunning = true;
		RepData.Instigator = Instigator;
		// Attac Animation
		FTimerHandle ProjectileSpawnHandle;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StartActionTimeEnasped", Instigator);
		if (ensure(GetWorld()))
		{
			if (AttackTimeDelay > 0.f)
				GetWorld()->GetTimerManager().SetTimer(ProjectileSpawnHandle, Delegate, AttackTimeDelay, false);
			else if (AttackTimeDelay == 0.f)
			{
				StartActionTimeEnasped(Instigator);
			}
		}
	}
	else
	{
		FTimerHandle ProjectileSpawnHandle;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StartActionTimeEnasped", Instigator);
		if (ensure(GetWorld()))
		{
			if (AttackTimeDelay > 0.f)
				GetWorld()->GetTimerManager().SetTimer(ProjectileSpawnHandle, Delegate, AttackTimeDelay, false);
			else if (AttackTimeDelay == 0.f)
			{
				StartActionTimeEnasped(Instigator);
			}
		}
	}
}

void UAwAction_AIRangedAttack::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}
