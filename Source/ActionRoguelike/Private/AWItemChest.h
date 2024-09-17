
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AWGameplayInterface.h"
#include "UI/AwWorldTipsWidget.h"
#include "AWItemChest.generated.h"

UCLASS()
class AAWItemChest : public AActor, public IAWGameplayInterface
{
	GENERATED_BODY()
	void Init_Paramters();

protected:
	UPROPERTY(Replicated,EditAnywhere,Category="Lock")
	bool bUsed;

	UPROPERTY(ReplicatedUsing=OnRep_LidOpened,EditAnywhere, Category="Lock")
	bool LidOpened;
	
	UPROPERTY(Replicated,EditAnywhere,Category="Lock")
	bool bLock;
	// Mesh?
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* BaseMesh;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* PileMesh;


	UPROPERTY(EditDefaultsOnly,Category="UI")
	TSubclassOf<UUserWidget> TipsWidgetClass;

	UPROPERTY()
	UAwWorldTipsWidget* TipsWidget;
	
	UFUNCTION(BlueprintCallable)
	void Unlock();
	UFUNCTION(BlueprintCallable)
	bool getLock(){return this->bLock;}

	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void OnRep_LidOpened();

	UFUNCTION(Client,Unreliable)
	void CallTips();
	
	UFUNCTION()
	virtual  void Interact_Implementation(APawn* InstigorPawn) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere)
	float TargetPitch;

	UFUNCTION(BlueprintCallable)
	void InvisTips();
	
	// Sets default values for this character's properties
	AAWItemChest();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
