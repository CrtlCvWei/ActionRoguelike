// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AwWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/AwOverayWidgetController.h"

void UAwWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	FVector2D ScreenLocation;
	ACharacter* MyCharacter = Cast<ACharacter>(AttachActor);
	if(UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(),MyCharacter->GetMesh()->GetSocketLocation(SocketName),ScreenLocation))
	{
		// Adjust 
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		if(Scale)
		{
			ScreenLocation /= Scale;
			if(ensure(ParentSizeBox))
			{
				ParentSizeBox->SetRenderTranslation(ScreenLocation);
			}
		}
	}
}

UAwWidgetController* UAwWorldUserWidget::GetWidgetController(FAwWidgetControllerParams Params)
{
	// TODO: Implement this
	if(WidgetController)
		return Cast<UAwWidgetController>(WidgetController);
	
	// TODO: Create a new object of type UAwOverayWidgetController
	const auto Controller = Cast<UAwOverayWidgetController>(NewObject<UAwOverayWidgetController>(this, UAwOverayWidgetController::StaticClass()));
	Controller->SetWidgetControllerParams(Params);
	Controller->BindCallBacksToDependencies();
	// Controller->BroadcastInitVals();
	SetWidgetController(Controller);
	return Controller;
}


