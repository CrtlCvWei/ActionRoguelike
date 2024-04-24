// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "AwPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AAwPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float CrouchBlendDuration = 2.f;
	
	UPROPERTY(EditDefaultsOnly)
	float CrouchBlendTime ;
	
public:
	AAwPlayerCameraManager();
	
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
};

