// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AwUserWidget.h"
#include "GameFramework/HUD.h"
#include "WidgetController/AwOverayWidgetController.h"
#include "AwHUD.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AAwHUD : public AHUD
{
	GENERATED_BODY()
public:

	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<UAwUserWidget> OverlayUserWidget;
	
	UFUNCTION()
	void InitOverlayWidget(APlayerController* PC, APlayerState* PS, UAWAttributeComp* AC, UAwActionComponent* ASC);

	UPROPERTY()
	TObjectPtr<UAwWidgetController> OverlayWidgetController;

	UFUNCTION()
	UAwWidgetController* GetOverlayWidgetController(FAwWidgetControllerParams& Params);

	
protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAwUserWidget> OverlayUserWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAwWidgetController> OverlayWidgetControllerClass;
};
