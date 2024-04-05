// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AwWorldTipsWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Fonts/SlateFontInfo.h"

void UAwWorldTipsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (AttachActor)
	{
		FVector2D ScreenLocation;
		if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachActor->GetActorLocation() + OffSet,
		                                           ScreenLocation))
		{
			float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
			if (Scale)
			{
				ScreenLocation /= Scale;
			}

			float Dis = FVector::Distance(GetOwningPlayer()->GetCharacter()->GetActorLocation(),
			                              AttachActor->GetActorLocation());

			// 根据距离调整UI文字的大小
			float TextSize = 2.e3 / (2.e3  + Dis) ; // 
			
			if (ensure(TipsTextBlock))
			{
				TipsTextBlock->SetRenderTranslation(ScreenLocation);
				TipsTextBlock->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 24 * TextSize));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachActor is nullptr"));
	}
}

UAwWorldTipsWidget* UAwWorldTipsWidget::CreateCustomWidget(AActor* const actor, const FVector3d offset)
{
	UAwWorldTipsWidget* widget = NewObject<
		UAwWorldTipsWidget>(GetTransientPackage(), UAwWorldTipsWidget::StaticClass());
	if (widget)
	{
		widget->AttachActor = actor;
		widget->OffSet = offset;
	}
	return widget;
}

void UAwWorldTipsWidget::SetText(FText text)
{
	if (TipsTextBlock)
		TipsTextBlock->SetText(text);
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TipsTextBlock is nullptr"));
	}
}
