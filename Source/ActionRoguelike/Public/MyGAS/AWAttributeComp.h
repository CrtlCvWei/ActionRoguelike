// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Delegates/DelegateInstancesImpl.h"
#include "Components/ActorComponent.h"
#include "MyGAS/AwAttributeSet.h"
#include <functional>
#include "AWAttributeComp.generated.h"

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChangeSignture, AActor*, InstigatorActor, UAWAttributeComp*,
                                              AttributeComp, float, NewHealth, float, Change);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API UAWAttributeComp : public UActorComponent
{
	GENERATED_BODY()


private:
	UPROPERTY(BlueprintCallable)
	FOnAttributeChangeSignture OnChange;

	// Sets default values for this component's properties

	UPROPERTY(EditDefaultsOnly, Blueprintable, Category="Attributes")
	TObjectPtr<UAwAttributeSet> AttributeSet;

	UPROPERTY()
	TMap<FName, FOnAttributeChangeSignture> AllAttributeChangeMap;

	inline  FAwAttributeData GetAttributeData(const FName AttributeName) const;
	
public:
	UAWAttributeComp();
	void CreateAllAttributeChangeMap();

	UFUNCTION(BlueprintCallable)
	bool SetHealth(const float v, AActor* Sourcer);

	UFUNCTION(BlueprintCallable)
	bool SetAttribute( FName AttributeName, const float v,AActor* Sourcer);
	
	UFUNCTION(BlueprintCallable)
	bool isAlive() const;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return AttributeSet->GetHealth(); }

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return AttributeSet->GetMaxHealth(); }
	
	UFUNCTION(BlueprintCallable)
	inline  float GetAttributeBase(FName AttributeName) const;

	UFUNCTION(BlueprintCallable)
	inline  float GetAttributeCurrent(FName AttributeName) const;
	
	UFUNCTION(BlueprintCallable)
	UAwAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangeSignture OnHealthChange;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangeSignture OnMaxHealthChange;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangeSignture OnManaChange;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangeSignture OnMaxManaChange;
	
	UFUNCTION(BlueprintCallable)
	void AttributeChangeBoardCast(const FName Name, AActor* Instigator, float NewValue, float Change);

	template <class UserClass>
	UFUNCTION()
	void AttributeChangeBind(const FName Name, UserClass* Object,void (UserClass::*Function)(AActor*,UAWAttributeComp*, float, float),FString&& FuncName)
	{
	    // AttributeChangeBind("Health", this, &AAWAICharacter::OnHealthChange,"&AAWAICharacter::OnHealthChange");
		if (AllAttributeChangeMap.Contains(Name))
		{
			FName FunctionName = STATIC_FUNCTION_FNAME(*FuncName); // *FString is TCHAR
			AllAttributeChangeMap[Name].__Internal_AddUniqueDynamic(Object, Function,FunctionName);// Add the function to the delegate
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AttributeChangeBind: %s not found in AllAttributeChangeMap"),*Name.ToString());
		}
	}
		
};
