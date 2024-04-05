// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AwHUD.h"


void AAwHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AAwHUD::InitOverlayWidget(APlayerController* PC, APlayerState* PS, UAWAttributeComp* AC, UAwActionComponent* ASC)
{
	checkf(OverlayUserWidgetClass, TEXT("Check OverlayUserWidgetClass in AwHUD failed,plz fill it out\n"));
	checkf(OverlayWidgetControllerClass, TEXT("Check OverlayWidgetControllerClass in AwHUD failed,plz fill it out\n"));

	//	 prevent creating multiple widgets
	if (OverlayUserWidget)
		return;
	UUserWidget* UserWidget = CreateWidget<UAwUserWidget>(GetWorld(), OverlayUserWidgetClass);
	OverlayUserWidget = Cast<UAwUserWidget>(UserWidget);
	FAwWidgetControllerParams Params(PC, PS, AC, ASC);
	OverlayWidgetController = GetOverlayWidgetController(Params);
	OverlayUserWidget->SetWidgetController(OverlayWidgetController);
	OverlayWidgetController->BroadcastInitVals();
	if (ensure(OverlayUserWidget))
		OverlayUserWidget->AddToViewport();
}

UAwWidgetController* AAwHUD::GetOverlayWidgetController(FAwWidgetControllerParams& Params)
{
	// Singleton 
	if (OverlayWidgetController)
	{
		return OverlayWidgetController;
	}
	OverlayWidgetController = NewObject<UAwOverayWidgetController>(this, OverlayWidgetControllerClass);
	if (OverlayWidgetController)
	{
		OverlayWidgetController->SetWidgetControllerParams(Params);
		OverlayWidgetController->BindCallBacksToDependencies();
	}

	return OverlayWidgetController;
}
