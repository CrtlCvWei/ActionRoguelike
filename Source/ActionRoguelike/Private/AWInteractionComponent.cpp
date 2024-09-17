// Fill out your copyright notice in the Description page of Project Settings.


#include "AWInteractionComponent.h"
#include "DrawDebugHelpers.h"
#include "AWGameplayInterface.h"
#include "Debug/DebugHelper.h"


static TAutoConsoleVariable<bool> CVarDebugDrawInteractionLines(
	TEXT("aw.DrawIntLines"), false,TEXT("Enable Debug lines for  Interact Compoents"), ECVF_Cheat);
#pragma region interaction
void UAWInteractionComponent::FindBestInteractable()
{
	AActor* Owner = GetOwner();
	APawn* Owner_Character = Cast<APawn>(Owner);
	
	FVector OwnerLocation;
	FRotator EyeRotation;
	FVector EndLocation;
	bool bDraw = CVarDebugDrawInteractionLines.GetValueOnGameThread(); // visulize
	FocusedActor = nullptr;
	
	if (Owner_Character)
	{
		/* 从摄像机中心获取Rotation*/
		Owner_Character->GetActorEyesViewPoint(OwnerLocation, EyeRotation);
	}

	else
		Owner->GetActorEyesViewPoint(OwnerLocation, EyeRotation);
	// 此时 EyeRotation.Vector() 为旋转器旋转状态的单位向量，也就是旋转器的指向方向
	EndLocation = OwnerLocation + (EyeRotation.Vector() * this->InteractionDistance);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);
	ObjectQueryParams.IgnoreMask = ECC_WorldDynamic;

	FColor color = FColor::Red;
	TArray<FHitResult> HitResults;
	FCollisionShape shape;
	shape.SetSphere(TraceRadius);

	if (auto bHit = GetWorld()->SweepMultiByObjectType(HitResults, OwnerLocation, EndLocation, FQuat::Identity,
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
					FocusedActor = HitActor;
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

void UAWInteractionComponent::PrimaryIntract()
{
	ServeInteract(FocusedActor);
}

void UAWInteractionComponent::ServeInteract_Implementation(AActor* InFocs)
{
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		ensure(Owner); // would trigger this
		UE_LOG(LogTemp, Error, TEXT("Owner is null in UAWInteractionComponent::PrimaryIntract()"));
		return;
	}
	if (!InFocs)
	{
		Debug::Print("No Focused Actor", FColor::Red);
		return;
	}
	APawn* MyPawn = Cast<APawn>(GetOwner());
	IAWGameplayInterface::Execute_Interact(InFocs, MyPawn);
}

#pragma endregion

// Sets default values for this component's properties
UAWInteractionComponent::UAWInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceRadius = 30.0f;
	InteractionDistance = 600.0f;
	CollisionChannel = ECC_WorldStatic;
}


// Called when the game starts
void UAWInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	PrimaryComponentTick.bCanEverTick = true;
	FocusedActor = nullptr;
}

// Called every frame
void UAWInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	const APawn* MyPawn = Cast<APawn>(GetOwner());
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}
}
