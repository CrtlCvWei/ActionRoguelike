// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AWInteractionComponent.generated.h"


class UAwWorldUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UAWInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void PrimaryIntract();
	
	// Sets default values for this component's properties
	UAWInteractionComponent();

protected:

	UFUNCTION(Server,Unreliable)
	void ServeInteract(AActor* InFocs);

	void FindBestInteractable();
	
	UPROPERTY()
	TObjectPtr<AActor> FocusedActor;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceRadius = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	
	UPROPERTY(EditAnywhere,Category = "Trace");
	float InteractionDistance = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UAwWorldUserWidget> DefaultWidgetClass;

	UPROPERTY()
	TObjectPtr<UAwWorldUserWidget> DefaultWidgetInstance;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
