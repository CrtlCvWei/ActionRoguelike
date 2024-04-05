// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "AwWorldTipsWidget.generated.h"

/**
 * 
 */
UCLASS()
class UAwWorldTipsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TipsTextBlock;

	UPROPERTY()
	AActor* AttachActor;
	

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:

	UPROPERTY(EditAnywhere,Category="UI")
	FVector OffSet;

	// should be a static function
	static UAwWorldTipsWidget* CreateCustomWidget(AActor* const actor,const FVector3d offset);
	
	UFUNCTION(BlueprintCallable)
	AActor* GetAttachActor() const {return this->AttachActor;}

	// virtual void SetVisibility(ESlateVisibility InVisibility) override;
	void SetAttachActor(AActor* actor) {this->AttachActor = actor;}
	void SetOffset(FVector offset) {this->OffSet = offset;}
	virtual void SetText(FText text);
	// UFUNCTION(BlueprintCallable)
	// virtual void SetVisibility(ESlateVisibility InVisibility) override;
};
