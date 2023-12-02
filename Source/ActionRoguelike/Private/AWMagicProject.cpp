// Fill out your copyright notice in the Description page of Project Settings.


#include "AWMagicProject.h"

#include "AWAttributeComp.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AAWMagicProject::AAWMagicProject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Init_Paramters();
}

void AAWMagicProject::Init_Paramters()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>("MoveComp");
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComp");
	VanishAudio = CreateDefaultSubobject<UAudioComponent>("VanishAudio");
	//
	RootComponent = SphereComp;
	NiagaraComp->SetupAttachment(SphereComp);
	VanishAudio->SetupAttachment(SphereComp);
	SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComp->SetCollisionProfileName("Projectile");
	SphereComp->SetCollisionResponseToAllChannels(ECR_Block);
	SphereComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);


	MoveComp->InitialSpeed = this->InitialSpeed;
	//物体将会根据其速度方向自动旋转，以使其面朝移动方向
	MoveComp->bRotationFollowsVelocity = true;
	//物体的初始速度将相对于其自身的本地坐标系
	MoveComp->bInitialVelocityInLocalSpace = true;
	MoveComp->ProjectileGravityScale = this->ProjectileGravityScale;
}


void AAWMagicProject::OnComponentHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, FVector NormalImpluse,
                                                    const FHitResult& Hit)
{
	if (OtherActor && (Cast<APawn>(OtherActor) != this->GetInstigator()))
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), 20.f, 12, FColor::Blue, true, 0.5f, 1, 10);
		if (this->Destroy())
		{
			if (VanishAudio && VanishAudio->Sound)
				VanishAudio->Play();
		}


	}
}


void AAWMagicProject::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                    bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this->GetInstigator())
	{
		UAWAttributeComp* Attribute = Cast<UAWAttributeComp>(
			OtherActor->GetComponentByClass(UAWAttributeComp::StaticClass()));
		//check if null
		if (Attribute)
		{
			Attribute->SetHealth(-20,this->GetInstigator());
			// FString CombineText = FString::Printf(
			// 	TEXT("Hit at here:%s and cost damage -20"), *SweepResult.ImpactPoint.ToString());
			// DrawDebugString(GetWorld(), SweepResult.ImpactPoint, CombineText, nullptr, FColor::Red, 1.0f, true);
			if (this->Destroy())
			{
				if (VanishAudio && VanishAudio->Sound)
					VanishAudio->Play();
			}
		}
	}
}

void AAWMagicProject::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	this->SphereComp->OnComponentHit.AddDynamic(this, &AAWMagicProject::OnComponentHit);
	this->SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAWMagicProject::OnBeginOverlap);
}

// Called when the game starts or when spawned
void AAWMagicProject::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAWMagicProject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
