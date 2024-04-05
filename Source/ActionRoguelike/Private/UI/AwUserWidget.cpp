// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AwUserWidget.h"

void UAwUserWidget::SetWidgetController(UObject* controller)
{
	WidgetController = controller;
	WidgetControllerSet();
}
