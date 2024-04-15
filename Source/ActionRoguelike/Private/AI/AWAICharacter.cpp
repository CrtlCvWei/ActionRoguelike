// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AWAICharacter.h"

#include "AIController.h"
#include "AWReward.h"
#include "..\Public\MyGAS/AWAttributeComp.h"
#include "AwCharacter.h"
#include "UI/AwWorldUserWidget.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AAWAICharacter::AAWAICharacter()
{
	Init_Paramters();
}

void AAWAICharacter::Init_Paramters()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	AttributeComp = CreateDefaultSubobject<UAWAttributeComp>(TEXT("AttributeComp"));
	ActionComp = CreateDefaultSubobject<UAwActionComponent>(TEXT("ActionComp"));
	ensure(ActionComp->SetOwningActor());
	ensure(AttributeComp->SetOwningActor());
	RewardComp = CreateDefaultSubobject<UAWReward>(TEXT("RewardComp"));
	RewardComp->Initialize(100, false);
	SensingComp = CreateDefaultSubobject<UPawnSensingComponent>("SensingComp");

}



// Called when the game starts or when spawned
void AAWAICharacter::BeginPlay()
{
	Super::BeginPlay();

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	if (ensure(this->AttributeComp))
	{
		AttributeComp->AttributeChangeBind("Health", this, &AAWAICharacter::OnHealthChange,"&AAWAICharacter::OnHealthChange");
	}
	if (ensure(this->SensingComp))
	{
		SensingComp->OnSeePawn.AddDynamic(this, &AAWAICharacter::OnPawnSeen);
	}
}

void AAWAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}


void AAWAICharacter::OnHealthChange(AActor* InstigatorActor, UAWAttributeComp* AttributeComponent, float NewHealth,
                                    float Change)
{
	if (this->AttributeComp && Change < 0.f)
	{
		if (!AttributeComp->isAlive())
		{
			if (AAIController* AIController = Cast<AAIController>(this->GetController()))
			{
				// Stop the behavior tree and give reason
				AIController->GetBrainComponent()->StopLogic("Killed");
			}
			// ragdoll
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");
			
			DetachFromControllerPendingDestroy();
			FTimerHandle DeadTimer;
			GetWorld()->GetTimerManager().SetTimer(DeadTimer, [this]() { this->Destroy(); }, 5.f, false);
		}
		else
		{
			if (InstigatorActor)
			{
				if (HealthBarClass != nullptr)
				{
					if (AIHealthBar == nullptr)
					{
						AIHealthBar = CreateWidget<UAwWorldUserWidget>(GetWorld(), HealthBarClass);
						AIHealthBar->AttachActor = this;
						AIHealthBar->GetWidgetController(FAwWidgetControllerParams(this->GetController(), nullptr, AttributeComp, nullptr));
						AIHealthBar->AddToViewport();
					}
					OnPawnSeen(Cast<APawn>(InstigatorActor));
				}
			}
		}
	}
}

void AAWAICharacter::SetTarget(APawn* SeenPawn)
{
	AAIController* AIC = Cast<AAIController>(this->GetController());
	if (AIC)
	{
		UBlackboardComponent* BBC = AIC->GetBlackboardComponent();
		if (ensure(BBC))
		{
			BBC->SetValueAsObject("TargetActor", SeenPawn);
			BBC->SetValueAsBool("Seen", true);
		}
	}
}

void AAWAICharacter::OnPawnSeen(APawn* SeenPawn)
{
	AAwCharacter* isPlayer = Cast<AAwCharacter>(SeenPawn);
	if (isPlayer)
	{
		SetTarget(SeenPawn);
	}
}
