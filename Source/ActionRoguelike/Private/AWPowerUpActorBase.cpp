// Fill out your copyright notice in the Description page of Project Settings.


#include "AWPowerUpActorBase.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AAWPowerUpActorBase::AAWPowerUpActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	RootComponent = SphereComp;
	SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if(RootComponent)
		MeshComp->SetupAttachment(RootComponent);
	SetVisble(true);
	SpawnTime = 10.f;
}

void AAWPowerUpActorBase::OnRep_Visble()
{
}

void AAWPowerUpActorBase::SetVisble(bool bNewState)
{
	this->visible = bNewState;
	RootComponent->SetVisibility(bNewState);
	MeshComp->SetVisibility(bNewState);
}

// Called when the game starts or when spawned
void AAWPowerUpActorBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAWPowerUpActorBase::CoolDown_Implementation()
{
	visible = false;
	OnRep_Visble();
	GetWorld()->GetTimerManager().SetTimer(InvisibleTimerHandle,this,&AAWPowerUpActorBase::ShowUp,SpawnTime);
}

void AAWPowerUpActorBase::ShowUp_Implementation()
{
	visible = true;
	OnRep_Visble();
}


void AAWPowerUpActorBase::Interact_Implementation(APawn* InstigorPawn)
{
	IAWGameplayInterface::Interact_Implementation(InstigorPawn);
	// any other logics implements
}

void AAWPowerUpActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAWPowerUpActorBase, visible);
}



