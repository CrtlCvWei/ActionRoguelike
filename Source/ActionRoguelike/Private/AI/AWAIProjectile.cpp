// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AWAIProjectile.h"

#include "AWAttributeComp.h"
#include "Components/SphereComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


static TAutoConsoleVariable<float> CVarTurnAIProjectileDamage(
	TEXT("aw.AIProjectDamage"), 1,TEXT("AI's Projectile Damage *= <Var>"), ECVF_Cheat);
static TAutoConsoleVariable<bool> CVarDebugVisulizeAIProjectileDamage(
	TEXT("aw.AIProjectDebug"), false,TEXT("AI's Projectile Debug"), ECVF_Cheat);


void AAWAIProjectile::Init_Paramters()
{
	MoveComp->InitialSpeed = this->InitialSpeed;
	
}

void AAWAIProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this->GetInstigator())
	{
		UAWAttributeComp* Attribute = Cast<UAWAttributeComp>(
			OtherActor->GetComponentByClass(UAWAttributeComp::StaticClass()));
		//check if null
		if (Attribute)
		{
			float autral_damage = -this->damage;
			float God_test = CVarTurnAIProjectileDamage.GetValueOnGameThread();
			autral_damage *= God_test;
			
			Attribute->SetHealth( autral_damage, this->GetInstigator());

			bool bDebug = CVarDebugVisulizeAIProjectileDamage.GetValueOnGameThread();
			if(bDebug)
			{
				FString CombineText = FString::Printf(
				TEXT("Hit at here:%s and cost damage %f"), *SweepResult.ImpactPoint.ToString(), autral_damage);
				DrawDebugString(GetWorld(), SweepResult.ImpactPoint, CombineText, nullptr, FColor::Red, 1.0f, true);
			}
			
			if (this->Destroy())
			{
				if (VanishAudio && VanishAudio->Sound)
					VanishAudio->Play();
			}
		}
	}
}

void AAWAIProjectile::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpluse, const FHitResult& Hit)
{
	if (OtherActor && Cast<APawn>(OtherActor) != this->GetInstigator())
	{
		UAWAttributeComp* Attribute = Cast<UAWAttributeComp>(
			OtherActor->GetComponentByClass(UAWAttributeComp::StaticClass()));
		if (Attribute)
		{
			Attribute->SetHealth(-this->damage, this->GetInstigator());
		}

		// DrawDebugSphere(GetWorld(), GetActorLocation(), 20.f, 12, FColor::Blue, true, 0.5f, 1, 10);
		if (VanishAudio && VanishAudio->Sound)
			VanishAudio->Play();
		Explode();
		bool temp = this->Destroy();
		UE_LOG(LogTemp, Display, TEXT("Destroy: %s"), (temp?TEXT("true") : TEXT("false")));
	}
}

void AAWAIProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	this->SphereComp->OnComponentHit.AddDynamic(this, &AAWAIProjectile::OnComponentHit);
	this->SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAWAIProjectile::OnBeginOverlap);
}

AAWAIProjectile::AAWAIProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Init_Paramters();
}
