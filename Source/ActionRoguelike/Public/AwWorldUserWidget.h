// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwWorldUserWidget.generated.h"

class USizeBox;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API UAwWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta=(BindWidget))
	USizeBox* ParentSizeBox;

	FName SocketName = "HealthBar";

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	
	UPROPERTY(BlueprintReadOnly,Category="UI")
	AActor* AttachActor;
};
