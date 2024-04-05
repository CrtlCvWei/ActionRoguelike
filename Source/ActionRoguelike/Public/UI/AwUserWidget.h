// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetController/AwWidgetController.h"
#include "AwUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API UAwUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
	
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();

public:

	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* controller);
	
	
};
