// Fill out your copyright notice in the Description page of Project Settings.


#include "AWHealPoint.h"

#include "AwCharacter.h"
#include "..\Public\MyGAS/AWAttributeComp.h"
#include "Components/SphereComponent.h"

// Sets default values
AAWHealPoint::AAWHealPoint()
{

}

// Called when the game starts or when spawned
void AAWHealPoint::BeginPlay()
{
	Super::BeginPlay();
	ShowUp();
}

void AAWHealPoint::Interact_Implementation(APawn* InstigorPawn)
{
	IAWGameplayInterface::Interact_Implementation(InstigorPawn);
	AAwCharacter* Instigor = Cast<AAwCharacter>(InstigorPawn);
	UAWAttributeComp* Attribute = Cast<UAWAttributeComp>(Instigor->GetOwningAttribute());
	if(Attribute)
	{
		if(Attribute->GetHealth() < Attribute->GetMaxHealth() && this->visible)
		{
			// Not  in full health
			Attribute->SetHealth(50.f,this);
			// trigger the event
			CoolDown();
		}
	}
}

void AAWHealPoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


