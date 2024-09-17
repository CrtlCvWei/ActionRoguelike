// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AwWidgetController.h"

void UAwWidgetController::SetWidgetControllerParams(FAwWidgetControllerParams& Params)
{
	PlayerController = Cast<APlayerController>(std::move(Params.PlayerController));
	PlayerState = std::move(Params.PlayerState);
	AwAttributeComp = std::move(Params.AttributeComp);
	AbilitySystemComp = std::move(Params.AbilitySystemComponent);
}

void UAwWidgetController::BroadcastInitVals()
{
}

void UAwWidgetController::BindCallBacksToDependencies()
{
}
