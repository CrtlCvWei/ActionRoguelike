// Fill out your copyright notice in the Description page of Project Settings.


#include "AWProjectileBase.h"

#include "Components/SphereComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void AAWProjectileBase::Init_Paramters()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>("MoveComp");
	ParticaleComp = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComp");
	VanishAudio = CreateDefaultSubobject<UAudioComponent>("VanishAudio");
	//
	RootComponent = SphereComp;
	ParticaleComp->SetupAttachment(SphereComp);
	VanishAudio->SetupAttachment(SphereComp);
	SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComp->SetCollisionProfileName("ProjectileBase");
	SphereComp->SetCollisionResponseToAllChannels(ECR_Block);
	SphereComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	MoveComp->InitialSpeed = this->InitialSpeed;
	//物体将会根据其速度方向自动旋转，以使其面朝移动方向
	MoveComp->bRotationFollowsVelocity = true;
	//物体的初始速度将相对于其自身的本地坐标系
	MoveComp->bInitialVelocityInLocalSpace = true;
	MoveComp->ProjectileGravityScale = this->ProjectileGravityScale;
}

// Sets default values
AAWProjectileBase::AAWProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	this->Init_Paramters();
}


void AAWProjectileBase::OnComponentHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpluse, const FHitResult& Hit)
{
}

void AAWProjectileBase::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

// Called when the game starts or when spawned
void AAWProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAWProjectileBase::Explode()
{
	if (ensure(IsValid(this)))
	{
		// Auto-managed particle pooling
		if (ImpactVFX)
			UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation(), true,
			                                         EPSCPoolMethod::AutoRelease);

		// UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		//
		// UGameplayStatics::PlayWorldCameraShake(this, ImpactShake, GetActorLocation(), GetImpactShakeInnerRadius(), GetImpactShakeOuterRadius());

		Destroy();
		// Release back to pool instead of destroying
		// USActorPoolingSubsystem* PoolingSubsystem = GetWorld()->GetSubsystem<USActorPoolingSubsystem>();
		// PoolingSubsystem->ReleaseToPool(this);
	}
	UE_LOG(LogTemp, Display, TEXT("Explode!!!"));
}

// Called every frame
void AAWProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
