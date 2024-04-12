// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGAS/AwActionComponent.h"
#include "MyGAS/AWAttributeComp.h"
#include "AbilitySystemComponent.h"
#include "AwWidgetController.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHealthChangeUISignture, float, NewVal,float, OldVal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMaxHealthChangeUISignture, float, NewVal,float, OldVal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FManaChangeUISignture, float, NewVal,float, OldVal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMaxManaChangeUISignture, float, NewVal,float, OldVal);

USTRUCT(BlueprintType)
struct FAwWidgetControllerParams
{
	GENERATED_BODY()

	FAwWidgetControllerParams()
	{
	};

	FAwWidgetControllerParams(AController* PC, APlayerState* PS, UAWAttributeComp* AC, UAwActionComponent* ASC):
		PlayerController(PC), PlayerState(PS), AttributeComp(AC), AbilitySystemComponent(ASC)
	{
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WidgetController")
	TObjectPtr<AController> PlayerController = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WidgetController")
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WidgetController")
	TObjectPtr<UAWAttributeComp> AttributeComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WidgetController")
	TObjectPtr<UAwActionComponent> AbilitySystemComponent = nullptr;
};

UCLASS(BlueprintType, Blueprintable)
class ACTIONROGUELIKE_API UAwWidgetController : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WidgetController")
	TObjectPtr<UAWAttributeComp> AttributeComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WidgetController")
	TObjectPtr<UAwActionComponent> AbilitySystemComp;


public:
	
	void SetWidgetControllerParams(FAwWidgetControllerParams& Params);

	UFUNCTION(BlueprintCallable)
	virtual  void BroadcastInitVals();

	UFUNCTION()
	virtual  void BindCallBacksToDependencies();
};
