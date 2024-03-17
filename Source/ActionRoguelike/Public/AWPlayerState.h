// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AwCharacter.h"
#include "GameFramework/PlayerState.h"
#include "AWPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnPlayerStateChange,AAwCharacter*,AwPlayer,int32,ItemTpye,int32,NewVal,int32,Delta);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AAWPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	int32 Credits = 0;
	int32 Scores = 0;
public:

	UFUNCTION(BlueprintCallable,Category="Credits&Scores")
	int32 GetCredits() const;
	UFUNCTION(BlueprintCallable,Category="Credits&Scores")
	int32 GetScores() const;
	UFUNCTION(BlueprintCallable,Category="Credits&Scores")
	void AddCredits(int32 v);
	UFUNCTION(BlueprintCallable,Category="Credits&Scores")
	void AddScores(int32 v);
	UFUNCTION(BlueprintCallable,Category="Credits&Scores")
	bool RemoveCredits(int32 v);
	UFUNCTION(BlueprintCallable,Category="Credits&Scores")
	bool RemoveScores(int32 v);
	UPROPERTY(BlueprintAssignable,Category="Event")
	FOnPlayerStateChange StateChangeEvent;

	AAWPlayerState();
};
