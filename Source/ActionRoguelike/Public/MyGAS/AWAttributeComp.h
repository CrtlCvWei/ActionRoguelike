// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Delegates/DelegateInstancesImpl.h"
#include "Components/ActorComponent.h"
#include "MyGAS/AwAttributeSet.h"
                        #include "AWAttributeComp.generated.h"

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChangeSignture, AActor*, InstigatorActor, UAWAttributeComp*,
                                              AttributeComp, float, NewHealth, float, Change);





UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API UAWAttributeComp : public UActorComponent
{
	GENERATED_BODY()


private:

	UPROPERTY()
	AActor* OwningActor = nullptr;
	
	UPROPERTY(BlueprintCallable)
	FOnAttributeChangeSignture OnChange;

	// Sets default values for this component's properties

	UPROPERTY(Replicated,VisibleAnywhere, Blueprintable, Category="Attributes")
	TObjectPtr<UAwAttributeSet> AttributeSet;
	
	UPROPERTY()
	TMap<FName, FOnAttributeChangeSignture> AllAttributeChangeMapBase;

	UPROPERTY()
	TMap<FName, FOnAttributeChangeSignture> AllAttributeChangeMapCurr;

	UFUNCTION()
	FORCEINLINE  FAwAttributeData GetAttributeData(const FName AttributeName) const;
	
public:
	UAWAttributeComp();
	void CreateAllAttributeChangeMap();

	UFUNCTION(BlueprintCallable)
	bool SetHealth(const float v, AActor* Sourcer);

	UFUNCTION(BlueprintCallable)
	bool SetAttributeBase( FName AttributeName, const float v,AActor* Sourcer);

	UFUNCTION(BlueprintCallable)
	bool SetAttributeCurr( FName AttributeName, const float v,AActor* Sourcer);
	
	UFUNCTION(BlueprintCallable)
	bool isAlive() const;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const
	{
		return AttributeSet->GetHealthBase() + AttributeSet->GetHealthCurrent();
	}

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return AttributeSet->GetMaxHealthBase() + AttributeSet->GetMaxHealthCurrent(); }
	
	UFUNCTION(BlueprintCallable)
	float GetMana() const { return AttributeSet->GetManaBase() + AttributeSet->GetManaCurrent(); }

	
	UFUNCTION(BlueprintCallable)
	float GetMaxMana() const { return AttributeSet->GetMaxManaBase() + AttributeSet->GetMaxManaCurrent(); }

	
	UFUNCTION(BlueprintCallable)
	inline  float GetAttributeBase(FName AttributeName) const;

	UFUNCTION(BlueprintCallable)
	inline  float GetAttributeCurrent(FName AttributeName) const;

	UFUNCTION(BlueprintCallable)
	float GetActualAttribute(FName AttributeName) const;

	UFUNCTION()
	AActor* GetOwningActor() const { return OwningActor; }
	
	UFUNCTION(Blueprintable)
	bool SetOwningActor();
	
	
	UFUNCTION(BlueprintCallable)
	UAwAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	
	UFUNCTION(NetMulticast,Reliable,BlueprintCallable)
	void AttributeChangeBoardCast(const FName Name, AActor* Instigator, float NewValue, float Change, bool bIsBase);

	virtual void BeginPlay() override;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma region AttributeChangeBind
	template <class UserClass>
	UFUNCTION()
	void AttributeChangeBindBase(const FString Name, UserClass* Object,void (UserClass::*Function)(AActor*,UAWAttributeComp*, float, float),FString&& FuncName)
	{
		if( Name== FString() || !Function)
			return;
	    // AttributeChangeBind("Health", this, &AAWAICharacter::OnHealthChange,"&AAWAICharacter::OnHealthChange");
		if (AllAttributeChangeMapBase.Contains(FName(*Name)))
		{
			FName FunctionName = STATIC_FUNCTION_FNAME(*FuncName); // *FString is TCHAR
			AllAttributeChangeMapBase[FName(*Name)].__Internal_AddUniqueDynamic(Object, Function,FunctionName);// Add the function to the delegate
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AttributeChangeBind: %s not found in AllAttributeChangeMap"),*Name);
		}
	}

	template <class UserClass>
	UFUNCTION()
	void AttributeChangeBindCurr(const FString Name, UserClass* Object,void (UserClass::*Function)(AActor*,UAWAttributeComp*, float, float),FString&& FuncName)
	{
		if( Name== FString() || !Function)
			return;
		// AttributeChangeBind("Health", this, &AAWAICharacter::OnHealthChange,"&AAWAICharacter::OnHealthChange");
		if (AllAttributeChangeMapCurr.Contains(FName(*Name)))
		{
			FName FunctionName = STATIC_FUNCTION_FNAME(*FuncName); // *FString is TCHAR
			AllAttributeChangeMapCurr[FName(*Name)].__Internal_AddUniqueDynamic(Object, Function,FunctionName);// Add the function to the delegate
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AttributeChangeBind: %s not found in AllAttributeChangeMap"),*Name);
		}
	}
#pragma endregion
};
