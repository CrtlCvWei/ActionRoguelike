// Fill out your copyright notice in the Description page of Project Settings.


#include "AWItemChest.h"

#include "AWPlayerState.h"

void AAWItemChest::Init_Paramters()
{
	this->BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base");
	this->LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("Lid");
	this->PileMesh = CreateDefaultSubobject<UStaticMeshComponent>("PileMesh");
	BaseMesh->SetupAttachment(RootComponent);
	LidMesh->SetupAttachment(BaseMesh);
	//将碰撞对象类型设置为了ECC_WorldDynamic
	BaseMesh->SetCollisionObjectType(ECC_WorldDynamic);
	LidMesh->SetCollisionObjectType(ECC_WorldDynamic);

	this->TargetPitch = 110.f;
	this->bLock = false;
	this->bUsed = false;
}

// Sets default values
AAWItemChest::AAWItemChest()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Init_Paramters();

}

void AAWItemChest::Unlock()
{
	this->bLock = false;
}

void AAWItemChest::Interact_Implementation(APawn* InstigorPawn)
{
	if (bLock)
	{
		return;
	}
	if(!bUsed)
	{
		bUsed = true;
		AAWPlayerState* PlayerState = 	InstigorPawn->GetPlayerState<AAWPlayerState>();
		if (ensure(PlayerState))
		{
			PlayerState->AddScores(500);
		}
	}
}

// Called when the game starts or when spawned
void AAWItemChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAWItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input


