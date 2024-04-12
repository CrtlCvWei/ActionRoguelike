// Fill out your copyright notice in the Description page of Project Settings.


#include "AwAction_BlackHoleAbilbity.h"
#include "..\Public\MyGAS/AWAttributeComp.h"
#include "MyGAS/AwActionComponent.h"
#include "AWBlackHolePj.h"
#include "AWPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"

UAwAction_BlackHoleAbilbity::UAwAction_BlackHoleAbilbity()
{
}

void UAwAction_BlackHoleAbilbity::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	//
	if(bIsRunning)
		return;
	if (ensure(this->ProjectileClass) && ensure(AttackAni))
	{
		bIsRunning = true;
		AAwCharacter* Player = Cast<AAwCharacter>(Instigator);
		if(Player->GetIsClimbing() || Player->GetCharacterMovement()->IsFalling())
		{
			StopAction_Implementation(Instigator);
		}
		if (Player->GetVelocity() == FVector::ZeroVector)
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			Player->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
			// Get the camera location and rotation
			// Turn Player to the camera direction
			Player->SetActorRotation(FRotator(0.f, CameraRotation.Yaw, 0.f));
		}
		// Attac Animation
		Player->PlayAnimMontage(AttackAni, 1.);

		FTimerHandle ProjectileSpawnHandle;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StartAction_TimeElasped", Player);
		if (ensure(GetWorld()))
		{
			GetWorld()->GetTimerManager().SetTimer(ProjectileSpawnHandle, Delegate, 0.25f, false);
		}
	}
}

void UAwAction_BlackHoleAbilbity::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}

void UAwAction_BlackHoleAbilbity::StartAction_TimeElasped(ACharacter* Instigator)
{
	if (ensureAlways(ProjectileClass))
	{
		const FVector HandLocation = Instigator->GetMesh()->GetSocketLocation(HandSpawnSocketName);
		const FRotator ProjectileRotation = GetProjectileRotation(HandLocation);
		FTransform LocaTM = FTransform((FRotator)ProjectileRotation,
		                               (FVector)HandLocation);
		FActorSpawnParameters SpawnParams;
		/** Actor will spawn in desired location, regardless of collisions. */
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = Instigator;
		
		if (ProjectileClass->IsChildOf<AAWBlackHolePj>() )
		{
			// Spawn the projectile (Magic balls or any other things...)
			AAWBlackHolePj* BlackHole = GetWorld()->SpawnActor<AAWBlackHolePj>(ProjectileClass, LocaTM, SpawnParams);
			
			BlackHole->SetGravityScale(ProjectileGravityScale);
			BlackHole->SetInitSpeed(InitialSpeed);
			BlackHole->SetAttractionRange(AttractionRange);
			BlackHole->SetAttractionStrength(AttractionStrength);
			
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "ExplosiveAndCauseDamage", Instigator, BlackHole);

			GetWorld()->GetTimerManager().SetTimer(ExplosiveTimerHandle,
			                                       Delegate,
			                                       ExplosiveTimeDelay, false);
		}
	}
	StopAction_Implementation(Instigator);
}

FRotator UAwAction_BlackHoleAbilbity::GetProjectileRotation(FVector HandLocation) const
{
	/* 从摄像机中心进行射线检测，检测视线处是否有可见物体碰撞，根据碰撞情况修正projectile运动姿态、方向*/

	APlayerState* PS = Cast<APlayerState>(GetOwningComponent()->GetOuter());
	if (ensure(PS))
	{
		FVector CameraLocation;
		FVector EyeEndLocation;
		FRotator CameraRotation;
		FRotator ProjectileRotation;
		float longest_dis = 10000.f;
		AAwCharacter* Player = Cast<AAwCharacter>(PS->GetPawn());
		Player->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		EyeEndLocation = CameraLocation + longest_dis * CameraRotation.Vector();

		// 创建射线参数
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Player); // 忽略当前角色自身，避免与自身发生碰撞

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
	else
	{
		FString DebugMsg = "No Player" + GetNameSafe(this);
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, DebugMsg);
		return FRotator::ZeroRotator;
	}
}

void UAwAction_BlackHoleAbilbity::ExplosiveAndCauseDamage(AActor* Instigator,AActor* Projectile)
{
	AAWBlackHolePj* BlackHole = Cast<AAWBlackHolePj>(Projectile);
	if (BlackHole)
	{
		BlackHole->Explode();
	}


	// cause damage

	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(this->AttractionRange);
	// 定义一个查询参数，这里使用 ECC_Visibility 作为碰撞频道，你也可以根据需要更改
	FCollisionQueryParams QueryParams(NAME_None, false, BlackHole);
	QueryParams.AddIgnoredActor(Instigator);
	QueryParams.AddIgnoredActor(BlackHole);

	// 定义一个数组用于存储扫描到的 Actor
	TArray<FHitResult> HitResults;

	// 执行扫描
	if (GetWorld()->SweepMultiByChannel(HitResults, BlackHole->GetActorLocation(), BlackHole->GetActorLocation(),
	                                    FQuat::Identity, ECC_Pawn,
	                                    CollisionShape, QueryParams))
	{
		for (const FHitResult& HitResult : HitResults)
		{
			AActor* HitActor = HitResult.GetActor();
			// 
			if (HitActor)
			{
				// IF Actor IS PAWN
				if (Cast<APawn>(HitActor))
				{
					UAWAttributeComp* Attribute = Cast<UAWAttributeComp>(
						HitActor->GetComponentByClass(UAWAttributeComp::StaticClass()));
					if (Attribute)
					{
						// if(OtherActor->GetInstigator())
						Attribute->SetHealth(-ExplosiveDamage, Instigator);
					}
				}
			}
		}
	}
}
