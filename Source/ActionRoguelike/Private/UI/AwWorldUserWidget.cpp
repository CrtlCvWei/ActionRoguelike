// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AwWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

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
