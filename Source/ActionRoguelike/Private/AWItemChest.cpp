// Fill out your copyright notice in the Description page of Project Settings.


#include "AWItemChest.h"
#include "AWPlayerState.h"
#include "Net/UnrealNetwork.h"

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
	this->LidOpened = false;
}

void AAWItemChest::InvisTips()
{
	if(TipsWidget)
	{
		TipsWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

// Sets default values
AAWItemChest::AAWItemChest()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	Init_Paramters();
	
}

void AAWItemChest::Unlock()
{
	this->bLock = false;
}

void AAWItemChest::CallTips_Implementation()
{
	if(!TipsWidget)
	{
		// TipsWidget = UAwWorldTipsWidget::CreateCustomWidget(this, FVector(0, 0, 100));
		TipsWidget = CreateWidget<UAwWorldTipsWidget>(GetWorld(), TipsWidgetClass);
		TipsWidget->SetAttachActor(this);
		TipsWidget->SetOffset(FVector(0, 0, 100));
		TipsWidget->SetText(FText::FromString("This chest is locked"));
	}
	TipsWidget->SetVisibility(ESlateVisibility::Visible);
	TipsWidget->AddToViewport();
	// 5s后隐藏
	FTimerHandle UIManager;
	GetWorld()->GetTimerManager().SetTimer(UIManager, this, &AAWItemChest::InvisTips, 5.f, false);
}

void AAWItemChest::Interact_Implementation(APawn* InstigorPawn)
{
	if (bLock)
	{
		//
		CallTips();
		return;
	}
	LidOpened = !LidOpened;
	// OnRep_LidOpened();
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

#pragma region replicated
void AAWItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAWItemChest, bLock);
	DOREPLIFETIME(AAWItemChest, bUsed);
	DOREPLIFETIME(AAWItemChest, LidOpened);
}
#pragma endregion


