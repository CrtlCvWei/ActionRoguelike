// Fill out your copyright notice in the Description page of Project Settings.


#include "AWMagicProject.h"

#include "AwCharacter.h"
#include "AWPlayerState.h"
#include "..\Public\MyGAS/AWAttributeComp.h"
#include "Components/AudioComponent.h"


// Sets default values
AAWMagicProject::AAWMagicProject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Init_Paramters();
}

void AAWMagicProject::Init_Paramters()
{
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
		if (AAwCharacter* Player = Cast<AAwCharacter>(OtherActor))
		{
			// Player Attribute is in PlayerState
			APlayerController* PC = Cast<APlayerController>(Player->GetController());
			AAWPlayerState* AwPS = Cast<AAWPlayerState>(PC->PlayerState);
			UAWAttributeComp* Attribute = AwPS->GetPlayerAttribute();
			if (Attribute->SetAttribute(FName("Health"), -20.f, this->GetInstigator()))
			{
				if (this->Destroy())
				{
					if (VanishAudio && VanishAudio->Sound)
						VanishAudio->Play();
				}
				
			}
		}
		else
		{
			// AI
			UAWAttributeComp* Attribute = OtherActor->FindComponentByClass<UAWAttributeComp>();
			if (Attribute->SetHealth(-20, this->GetInstigator()))
			{
				if (this->Destroy())
				{
					if (VanishAudio && VanishAudio->Sound)
						VanishAudio->Play();
				}
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
