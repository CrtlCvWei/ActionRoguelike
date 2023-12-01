// Fill out your copyright notice in the Description page of Project Settings.


#include "AWBlackHolePj.h"

#include "AWAttributeComp.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


void AAWBlackHolePj::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                        const FHitResult& SweepResult)
{
	if (Cast<APawn>(OtherActor) != this->GetInstigator())
	{
		FCollisionObjectQueryParams CollisionEnableType;
		CollisionEnableType.AddObjectTypesToQuery(ECC_WorldDynamic);
		CollisionEnableType.AddObjectTypesToQuery(ECC_Pawn);
		CollisionEnableType.AddObjectTypesToQuery(ECC_PhysicsBody);
		// UE_LOG(LogTemp, Warning, TEXT("OtherComp: %s, CollisionObjectType: %s"), *OtherComp->GetName(), UEnum::GetEnumName(static_cast<int32>(OtherComp->GetCollisionObjectType())).ToString() );
		ECollisionChannel TChannel = OtherComp->GetCollisionObjectType();
		
		if (CollisionEnableType.IsValidObjectQuery(TChannel))
		{
			if (TChannel)
			{
				if (TChannel != ECC_Pawn)
				{
					OtherActor->Destroy();
				}
				else
				{
					// cause damage
					UAWAttributeComp* Attribute = Cast<UAWAttributeComp>(
						OtherActor->GetComponentByClass(UAWAttributeComp::StaticClass()));
					if (Attribute)
						Attribute->SetHealth(-damage, this->GetInstigator());
				}
			}
		}
	}
}

void AAWBlackHolePj::PerformAttraction()
{
	/*持续不断吸引周遭物体*/
	// 施加径向力
	RForce->SetWorldLocation(GetActorLocation());
	RForce->FireImpulse();
}

void AAWBlackHolePj::AttractionInterval()
{
	GetWorld()->GetTimerManager().SetTimer(AttractionTimerHandle, this, &AAWBlackHolePj::PerformAttraction, 0.1F, true);
}

// Sets default values
AAWBlackHolePj::AAWBlackHolePj()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->AAWBlackHolePj::Init_Paramters();
}

void AAWBlackHolePj::Init_Paramters()
{
	MoveComp->InitialSpeed = this->InitialSpeed;

	this->SphereComp->OnComponentHit.AddDynamic(this, &AAWBlackHolePj::OnComponentHit);

	RForce = CreateDefaultSubobject<URadialForceComponent>("RForce");
	RForce->SetupAttachment(this->SphereComp);
	RForce->Radius = this->AttractionRange;
	RForce->ForceStrength = this->AttractionStrength;
	RForce->AddCollisionChannelToAffect(ECC_Pawn);
	this->damage = 0;
}

void AAWBlackHolePj::Explode()
{
	Super::Explode();
}

void AAWBlackHolePj::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	this->SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAWBlackHolePj::OnBeginOverlap);
}

// Called when the game starts or when spawned
void AAWBlackHolePj::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAWBlackHolePj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
