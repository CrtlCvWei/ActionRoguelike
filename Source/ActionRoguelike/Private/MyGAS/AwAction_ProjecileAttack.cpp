// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGAS/AwAction_ProjecileAttack.h"
#include "Blueprint/AwBlueprintFunctionLibrary.h"
#include "AwCharacter.h"
#include "AWProjectileBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyGAS/AwActionComponent.h"
#include "Net/UnrealNetwork.h"


UAwAction_ProjecileAttack::UAwAction_ProjecileAttack()
{
	HandSpawnSocketName = "Right-Wist";
	AttackTimeDelay = 1.f;
}


void UAwAction_ProjecileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	//

	if (this->ProjectileClass && AttackAni)
	{
		RepData.bIsRunning = true;
		RepData.Instigator = Instigator;
		// Cost Mana

		AAwCharacter* Player = Cast<AAwCharacter>(Instigator);
		if (Player->GetVelocity() == FVector::ZeroVector)
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			if (!Player->GetController())
				return;
			Player->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
			// Get the camera location and rotation
			// Turn Player to the camera direction
			if (!Player->GetIsClimbing())
				Player->SetActorRotation(FRotator(0.f, CameraRotation.Yaw, 0.f));
		}
		// Attac Animation
		PlayAttackAni(Player);

		if (GetWorld()->GetNetMode() < NM_Client)
		{
			FTimerHandle ProjectileSpawnHandle;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "StartActionTimeEnasped", Player);
			if (ensure(GetWorld()))
			{
				GetWorld()->GetTimerManager().SetTimer(ProjectileSpawnHandle, Delegate,
				                                       AttackAni->Notifies[0].GetTriggerTime(), false);
			}
			//cost
			auto Attr = UAwBlueprintFunctionLibrary::GetAwAttributeComponent(Instigator);
			if (Attr)
			{
				Attr->SetAttributeBase("Mana", -ManaCost.GetCurrentValue(), Instigator);
			}
		}
	}
	else
	{
		if (GetWorld()->GetNetMode() < NM_Client)
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
			//cost
			auto Attr = UAwBlueprintFunctionLibrary::GetAwAttributeComponent(Instigator);
			if (Attr)
			{
				Attr->SetAttributeBase("Mana", -ManaCost.GetCurrentValue(), Instigator);
			}
		}
	}
}

void UAwAction_ProjecileAttack::StartActionTimeEnasped(AActor* Instigator)
{
	FTransform LocaTM;
	FActorSpawnParameters SpawnParams;
	AActor* Projectile = nullptr;
	if (AAwCharacter* Player = Cast<AAwCharacter>(Instigator))
	{
		const FVector HandLocation = Player->GetMesh()->GetSocketLocation(HandSpawnSocketName);
		const FRotator ProjectileRotation = this->GetProjectileRotation(Player, HandLocation);
		LocaTM = FTransform((FRotator)ProjectileRotation,
		                    (FVector)HandLocation);

		/** Actor will spawn in desired location, regardless of collisions. */
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = Player;
		// Spawn the projectile (Magic balls or any other things...)
		Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, LocaTM, SpawnParams);
		// 
	}
	else
	{
		LocaTM = FTransform((FRotator)Instigator->GetActorRotation(),
		                    (FVector)Instigator->GetActorLocation());
		/** Actor will spawn in desired location, regardless of collisions. */
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = nullptr;
		Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, LocaTM, SpawnParams);
	}
	// Set Effects only on the server
	if (GetWorld()->IsServer() && EffectsClass.Num() > 0)
	{
		// TODO : TRY USE EFFECTS[0] FOR TEST

		auto ProjecileBase = Cast<AAWProjectileBase>(Projectile);
		FAwGameplayEffectContextHandle GamePlayEffect = GetOwningComponent()->MakeEffectContex(Projectile, this);
		if (ProjecileBase)
		{
			ProjecileBase->SetEffectContext(GamePlayEffect);
		}
	}
	StopAction_Implementation(Instigator);
}

FRotator UAwAction_ProjecileAttack::GetProjectileRotation(ACharacter* Instigator, FVector HandLocation)
{
	/* 从摄像机中心进行射线检测，检测视线处是否有可见物体碰撞，根据碰撞情况修正projectile运动姿态、方向*/
	FVector CameraLocation;
	FVector EyeEndLocation;
	FRotator CameraRotation;
	FRotator ProjectileRotation;
	float longest_dis = 10000.f;
	auto Controller = Instigator->GetController();
	ensureAlways(Controller);
	Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);
	EyeEndLocation = CameraLocation + longest_dis * CameraRotation.Vector();

	// 创建射线参数
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Instigator); // 忽略当前角色自身，避免与自身发生碰撞

	// 执行射线检测
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EyeEndLocation, ECC_Visibility,
	                                                 CollisionParams);
	if (bHit)
	{
		// get the hit point
		EyeEndLocation = HitResult.ImpactPoint;
	}
	// fix the projectile rotation
	ProjectileRotation = FRotationMatrix::MakeFromX(EyeEndLocation - HandLocation).Rotator();
	return ProjectileRotation;
}


bool UAwAction_ProjecileAttack::CheckActionAvailable(AActor* Instigator) const
{
	AAwCharacter* Player = Cast<AAwCharacter>(Instigator);
	if (Player)
	{
		if (Player->GetIsClimbing() || Player->GetCharacterMovement()->IsFalling())
		{
			return false;
		}
	}
	return Super::CheckActionAvailable(Instigator);
}

void UAwAction_ProjecileAttack::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}

void UAwAction_ProjecileAttack::PlayAttackAni_Implementation(ACharacter* Player)
{
	Player->PlayAnimMontage(AttackAni, 1.);
}
