// Fill out your copyright notice in the Description page of Project Settings.


#include "AWExplosiveBarrel.h"

#include "AWAttributeComp.h"
#include "Kismet/GameplayStatics.h"

void AAWExplosiveBarrel::Init_Paramters()
{
	this->SMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	this->RForce = CreateDefaultSubobject<URadialForceComponent>("RadialForce");

	this->SMeshComp->SetupAttachment(this->RootComponent);
	this->RForce->SetupAttachment(this->SMeshComp);


	this->SMeshComp->SetSimulatePhysics(true); //将物理模拟设置为真
	//将碰撞对象类型设置为了物理实体，这一项的作用时，默认情况下RadialForceComponent仅会影响几条通道，而物理实体则是其中一条
	this->SMeshComp->SetCollisionObjectType(ECC_PhysicsBody);

	this->RForce->AddCollisionChannelToAffect(ECC_WorldDynamic);
	this->RForce->SetAutoActivate(false);
	this->RForce->bImpulseVelChange = true; //作用力速率变化为真
	this->RForce->Falloff = RIF_Linear;
	this->RForce->ImpulseStrength = this->ImpluseStrength;
	this->RForce->ForceStrength = this->ForceStrength;
	this->RForce->Radius = this->ExplosiveRadius;

}

// Sets default values
AAWExplosiveBarrel::AAWExplosiveBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	this->Init_Paramters();
}

void AAWExplosiveBarrel::WhenActorsHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                       FVector NormalImpluse, const FHitResult& Hit)
{
	/* 被碰撞组件HitComp、碰撞组件OtherComp */
	FName CollisionProfile = OtherComp->GetCollisionProfileName();
	
	if (CollisionProfile == FName("Projectile")||CollisionProfile == FName("ProjectileBase"))
	{
		// 使用 FTimerDelegate 
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("Fire"), OtherActor);
		//
		GetWorldTimerManager().SetTimer(ExplosiveTimeHandler, TimerDelegate, 2.f, false);
	}
	else
	{
		//...
	}
}

void AAWExplosiveBarrel::Fire(AActor* OtherActor)
{
	//我们对网格体组件施加作用力，让其能向上弹起
	FVector BoostIntensity = FVector::UpVector * this->ImpluseItself; 
	this->SMeshComp->AddImpulse(BoostIntensity, NAME_None, true);
	this->RForce->FireImpulse();
	
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(this->RForce->Radius);

	// 定义一个查询参数，这里使用 ECC_Visibility 作为碰撞频道，你也可以根据需要更改
	FCollisionQueryParams QueryParams(NAME_None, false, this);
	QueryParams.AddIgnoredActor(this);

	// 定义一个数组用于存储扫描到的 Actor
	TArray<FHitResult> HitResults;

	// 执行扫描
	if (GetWorld()->SweepMultiByChannel(HitResults, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_Pawn,
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
					UAWAttributeComp* Attribute = Cast<UAWAttributeComp>(HitActor->GetComponentByClass(UAWAttributeComp::StaticClass()));
					if (Attribute)
					{
						if(OtherActor->GetInstigator())
							Attribute->SetHealth(-30,OtherActor->GetInstigator());
						else
							Attribute->SetHealth(-30,this);
					}	
				}
			}
		}
	}
}


void AAWExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	this->SMeshComp->OnComponentHit.AddDynamic(this, &AAWExplosiveBarrel::WhenActorsHit);
}

// Called when the game starts or when spawned
void AAWExplosiveBarrel::BeginPlay()
{
	// 获取碰撞物体的碰撞频道

	Super::BeginPlay();
}

// Called every frame
void AAWExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
