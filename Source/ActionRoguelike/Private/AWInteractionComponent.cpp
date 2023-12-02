// Fill out your copyright notice in the Description page of Project Settings.


#include "AWInteractionComponent.h"

#include "AWGameplayInterface.h"


static TAutoConsoleVariable<bool> CVarDebugDrawInteractionLines(
	TEXT("aw.DrawIntLines"), false,TEXT("Enable Debug lines for  Interact Compoents"), ECVF_Cheat);

void UAWInteractionComponent::PrimaryIntract()
{
	AActor* Owner = GetOwner();
	APawn* Owner_Character = Cast<APawn>(GetOwner());
	if (!Owner)
	{
		ensure(Owner); // would trigger this
		UE_LOG(LogTemp, Error, TEXT("Owner is null in UAWInteractionComponent::PrimaryIntract()"));
		return;
	}
	else
	{
		FVector OwnerLocation;
		FRotator EyeRotation;
		FVector EndLocation;
		bool bDraw = CVarDebugDrawInteractionLines.GetValueOnGameThread(); // visulize

		if (Owner_Character)
		{
			/* 从摄像机中心获取Rotation*/
			// Owner_Character->GetController()->GetPlayerViewPoint(OwnerLocation, EyeRotation);
			Owner_Character->GetActorEyesViewPoint(OwnerLocation,EyeRotation);
		}

		else
			Owner->GetActorEyesViewPoint(OwnerLocation, EyeRotation);
		// 此时 EyeRotation.Vector() 为旋转器旋转状态的单位向量，也就是旋转器的指向方向
		EndLocation = OwnerLocation + (EyeRotation.Vector() * this->InteractionDistance);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

		FColor color = FColor::Red;
		TArray<FHitResult> HitResults;
		FCollisionShape shape;
		shape.SetSphere(30.f);

		if (GetWorld()->SweepMultiByObjectType(HitResults, OwnerLocation, EndLocation, FQuat::Identity,
		                                       ObjectQueryParams, shape))
		{
			// if any hits found out
			for (auto HitResult : HitResults)
			{
				AActor* HitActor = HitResult.GetActor();
				if (HitActor)
				{
					//do implements
					if (HitActor->Implements<UAWGameplayInterface>()) //  是UAWGameplayInterface不是IAWGameplayInterface
					{
						IAWGameplayInterface::Execute_Interact(HitActor, Owner_Character);
						break; // one thing a time
					}
				}
				if (bDraw)
				{
					color = FColor::Green;
					DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 30.f, 32, color, false, 2., 0., 1);
				}
			}
		}
		if (bDraw)
		{
			DrawDebugLine(GetWorld(), OwnerLocation, EndLocation, color, false, 3.f, 0, 1.5f);
		}
	}
}

// Sets default values for this component's properties
UAWInteractionComponent::UAWInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAWInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called every frame
void UAWInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
