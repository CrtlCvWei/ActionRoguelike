// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AWReward.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGiveRewards, AActor*, DeathActor, AActor*,
											  Killer, int,Scores);

class UAWAttributeComp;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UAWReward : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAWReward();
	void Initialize(const int S , const bool bUpdated){Score = S;bUpdate = bUpdated;}
protected:
	UPROPERTY(VisibleAnywhere,Category="Rewards")
	int Score;
	UPROPERTY()
	bool bUpdate = false;
	

	
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable)
	FGiveRewards GiveRewards ;

	int GetScore() const {return  Score;}
	void SetScore(const int NewScore){if(bUpdate){Score = NewScore;}}
	
};
