// Fill out your copyright notice in the Description page of Project Settings.


#include "AwAction_ProjecileAttack.h"

#include "AwCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UAwAction_ProjecileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	//
	
	if (ensure(this->ProjectileClass) && ensure(AttackAni))
	{
		// Attac Animation
		AAwCharacter* Player = Cast<AAwCharacter>(Instigator);
		if(Player->GetIsClimbing() || Player->GetCharacterMovement()->IsFalling())
		{
			StopAction_Implementation(Instigator);
			return;
		}
		if(Player->GetVelocity() == FVector::ZeroVector)
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			Player->GetController()->GetPlayerViewPoint(CameraLocation,CameraRotation); // Get the camera location and rotation
			// Turn Player to the camera direction
			if(!Player->GetIsClimbing())
				Player->SetActorRotation(FRotator(0.f,CameraRotation.Yaw,0.f));

		}

		Player->PlayAnimMontage(AttackAni, 1.);
		FTimerHandle ProjectileSpawnHandle;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this,"StartActionTimeEnasped",Player);
		if(ensure(GetWorld()))
		{
			GetWorld()->GetTimerManager().SetTimer(ProjectileSpawnHandle,Delegate,0.25f,false);
		}
		
	}
}

void UAwAction_ProjecileAttack::StartActionTimeEnasped(ACharacter* Instigator)
{
	if (ensureAlways(ProjectileClass))
	{
		const FVector HandLocation = Instigator->GetMesh()->GetSocketLocation(HandSpawnSocketName);
		const FRotator ProjectileRotation = this->GetProjectileRotation(Instigator,HandLocation);
		FTransform LocaTM = FTransform((FRotator)ProjectileRotation,
									   (FVector)HandLocation);
		FActorSpawnParameters SpawnParams;
		/** Actor will spawn in desired location, regardless of collisions. */
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = Instigator;
		// Spawn the projectile (Magic balls or any other things...)
		GetWorld()->SpawnActor<AActor>(ProjectileClass, LocaTM, SpawnParams);
	}
	StopAction_Implementation(Instigator);
}

FRotator UAwAction_ProjecileAttack::GetProjectileRotation(ACharacter* Instigator,FVector HandLocation)
{
	/* 从摄像机中心进行射线检测，检测视线处是否有可见物体碰撞，根据碰撞情况修正projectile运动姿态、方向*/
	FVector CameraLocation;
	FVector EyeEndLocation;
	FRotator CameraRotation;
	FRotator ProjectileRotation;
	float longest_dis = 10000.f;

	Instigator->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
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

UAwAction_ProjecileAttack::UAwAction_ProjecileAttack()
{
	HandSpawnSocketName = "Right-Wist";
	AttackTimeDelay = 1.f;
}

void UAwAction_ProjecileAttack::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}
 