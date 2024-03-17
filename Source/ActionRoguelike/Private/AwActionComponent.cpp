// Fill out your copyright notice in the Description page of Project Settings.


#include "AwActionComponent.h"

#include "AwAction.h"

// Sets default values for this component's properties
UAwActionComponent::UAwActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAwActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool UAwActionComponent::AddAction(TSubclassOf<UAwAction> ActionClass)
{
	if(!ensure(ActionClass))
	{
		return false;
	}

	// Skip for clients
	// if (!GetOwner()->HasAuthority())
	// {
	// 	UE_LOGFMT(LogGame, Warning, "Client attempting to AddAction. [Class: {Class}]", GetNameSafe(ActionClass));
	// 	return false;
	// }
	
	UAwAction* Action =  NewObject<UAwAction>(this,ActionClass);
	if(ensure(Action))
	{
		if(Actions.Add(Action) >=0)
			return true;
	}
	return false;
}

void UAwActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for(UAwAction *Action:Actions)
	{
		if(Action && Action->GetActionName()  == ActionName)
		{
			if(!Action->CheckActionAvailable(Instigator))
			{
				continue;
			}
			Action->StartAction(Instigator);
		}
			
	}
}

void UAwActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for(UAwAction *Action:Actions)
	{
		if(Action && Action->GetActionName() ==ActionName)
		{
				Action->StopAction(Instigator);
		}
			
	}
}


// Called every frame
void UAwActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Debug Code
	if(ActiveGameplayTags.Num() != 0)
	{
		FString DebugMsg = GetNameSafe(GetOwner()) + ": " +  ActiveGameplayTags.ToStringSimple();
		FString DebugMsg2 = GetNameSafe(GetOwner()) + ": " +  BlockGamePlayTags.ToStringSimple();
		GEngine->AddOnScreenDebugMessage(-1,0.f,FColor::Black,DebugMsg);
		GEngine->AddOnScreenDebugMessage(-1,0.f,FColor::White,DebugMsg2);
	}
	
}

