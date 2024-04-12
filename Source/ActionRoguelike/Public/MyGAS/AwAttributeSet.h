// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AwAttributeSet.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAwGameplayAttributeValueChange,float,float);

UENUM()
enum AttributeChangedType
{
	Current,
	Base
};
/*
 * Handle MY Macro !!!!
 * 
 */
//


#define AWGAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
FAwAttributeData Get##PropertyName##Attribute() \
{ \
    FProperty* Prop = FindFieldChecked<FProperty>(ClassName::StaticClass(), GET_MEMBER_NAME_CHECKED(ClassName, PropertyName)); \
    if (ensure(Prop)) \
    { \
     	FAwAttributeData Result;\
		Prop->GetValue_InContainer(this, &Result);\
		return Result;\
	}\
	return FAwAttributeData::ERROR;\
}

#define AWGAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
FORCEINLINE float Get##PropertyName() const \
{ \
return PropertyName.GetCurrentValue(); \
}

#define AWGAMEPLAYATTRIBUTE_VALUE_BASE_GETTER(ClassName, PropertyName) \
FAwAttributeData Get##PropertyName##Base() \
{ \
FProperty* Prop = FindFieldChecked<FProperty>(ClassName::StaticClass(), GET_MEMBER_NAME_CHECKED(ClassName, PropertyName)); \
if (ensure(Prop)) \
{ \
FAwAttributeData Target;\
Prop->GetValue_InContainer(this, &Target);\
return Target.GetBaseValue();\
}\
return FAwAttributeData::ERROR;\
}

#define AWGAMEPLAYATTRIBUTE_VALUE_INITTER(ClassName,PropertyName) \
FORCEINLINE void Init##PropertyName(float NewVal) \
{ \
float CurrentOld = PropertyName.GetCurrentValue(); \
float BaseOld = PropertyName.GetBaseValue(); \
PropertyName.SetBaseValue(NewVal); \
PropertyName.SetCurrentValue(NewVal); \
AttributeDataChangeDelegate(GET_MEMBER_NAME_CHECKED(ClassName,PropertyName), NewVal, BaseOld,Base); \
AttributeDataChangeDelegate(GET_MEMBER_NAME_CHECKED(ClassName,PropertyName), NewVal, CurrentOld,Current); \
}

#define AWGAMEPLAYATTRIBUTE_VALUE_SETTER(ClassName,PropertyName) \
FORCEINLINE void Set##PropertyName(float NewVal) \
{ \
float CurrentOld = PropertyName.GetCurrentValue(); \
PropertyName.SetCurrentValue(NewVal); \
AttributeDataChangeDelegate(GET_MEMBER_NAME_CHECKED(ClassName,PropertyName), NewVal, CurrentOld,Current); \
};

#define AWGAMEPLAYATTRIBUTE_VALUE_BASE_SETTER(ClassName,PropertyName) \
FORCEINLINE void Effect##PropertyName##Base(float NewVal) \
{ \
float BaseOld = PropertyName.GetBaseValue(); \
PropertyName.SetBaseValue(NewVal); \
AttributeDataChangeDelegate(GET_MEMBER_NAME_CHECKED(ClassName,PropertyName), NewVal, BaseOld,Base); \
};

#define AWATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	AWGAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	AWGAMEPLAYATTRIBUTE_VALUE_INITTER(ClassName,PropertyName)\
	AWGAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	AWGAMEPLAYATTRIBUTE_VALUE_BASE_GETTER(ClassName, PropertyName) \
	AWGAMEPLAYATTRIBUTE_VALUE_SETTER(ClassName,PropertyName) \
	AWGAMEPLAYATTRIBUTE_VALUE_BASE_SETTER(ClassName,PropertyName)
/*
 *
 * 
 */

USTRUCT(BlueprintType)
struct FAwAttributeData
{
	GENERATED_BODY()

	static const FAwAttributeData ERROR;

	FAwAttributeData(const FAwAttributeData& Other)
		: BaseVal(Other.BaseVal),
		  CurrentVal(Other.CurrentVal)
	{
	}

	FAwAttributeData(FAwAttributeData&& Other) noexcept
		: BaseVal(Other.BaseVal),
		  CurrentVal(Other.CurrentVal)
	{
	}

	FAwAttributeData& operator=(const FAwAttributeData& Other)
	{
		if (this == &Other)
			return *this;
		BaseVal = Other.BaseVal;
		CurrentVal = Other.CurrentVal;
		return *this;
	}

	FAwAttributeData& operator=(FAwAttributeData&& Other) noexcept
	{
		if (this == &Other)
			return *this;
		BaseVal = Other.BaseVal;
		CurrentVal = Other.CurrentVal;
		return *this;
	}

	bool operator==(const FAwAttributeData& Other) const
	{
		return BaseVal == Other.BaseVal && CurrentVal == Other.CurrentVal;
	}
	
	bool operator==(const FAwAttributeData* Other) const
	{
		return BaseVal == Other->BaseVal && CurrentVal == Other->CurrentVal;
	}

	bool operator!=(const FAwAttributeData& Other) const
	{
		return BaseVal != Other.BaseVal && CurrentVal != Other.CurrentVal;
	}
	
	bool operator!=(const FAwAttributeData* Other) const
	{
		return BaseVal != Other->BaseVal && CurrentVal != Other->CurrentVal;
	}


	FAwAttributeData(): BaseVal(0.0f), CurrentVal(0.f)
	{
	};

	FAwAttributeData(float v): BaseVal(v), CurrentVal(v)
	{
	}

	virtual ~FAwAttributeData()
	{
	}

	/** Returns the current value, which includes temporary buffs */
	float GetCurrentValue() const;

	/** Modifies current value, normally only called by ability system or during initialization */
	virtual void SetCurrentValue(float NewValue);

	/** Returns the base value which only includes permanent changes */
	float GetBaseValue() const;

	/** Modifies the permanent base value, normally only called by ability system or during initialization */
	virtual void SetBaseValue(float NewValue);

	bool IsNotVaild() {return *this == &ERROR;}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Property")
	float BaseVal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Property")
	float CurrentVal;
};

USTRUCT()
struct FReplicaAttributesEntry_FName_FAwAttributeData
{
	GENERATED_BODY()
	FName Key;
	FAwAttributeData Value;

	FReplicaAttributesEntry_FName_FAwAttributeData(): Key(NAME_None), Value(FAwAttributeData::ERROR)
	{
	}

	FReplicaAttributesEntry_FName_FAwAttributeData(const TPair<FName, FAwAttributeData>& A):
		Key(std::move(A.Key)), Value(std::move(A.Value))
	{
	}
};

UCLASS()
class ACTIONROGUELIKE_API UAwAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	virtual ~UAwAttributeSet() override
	{
	};

protected:
	UPROPERTY(BlueprintReadOnly)
	AActor* OwningActor;

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FAwAttributeData Health;

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FAwAttributeData MaxHealth;

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FAwAttributeData Mana;

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FAwAttributeData MaxMana;
	
	
	TMap<FName, FOnAwGameplayAttributeValueChange> AttributeCurrValueChangeDelegates;
	
	TMap<FName, FOnAwGameplayAttributeValueChange> AttributeBaseValueChangeDelegates;
	
	
	void CreateAttributeDataChangeDelegates();
	
	void AttributeDataChangeDelegate(FName Name, float NewValue, float OldValue,AttributeChangedType Type);

public:
	UAwAttributeSet();
	void InitFun();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	AWATTRIBUTE_ACCESSORS(UAwAttributeSet, Health);
	AWATTRIBUTE_ACCESSORS(UAwAttributeSet, MaxHealth);
	AWATTRIBUTE_ACCESSORS(UAwAttributeSet, Mana);
	AWATTRIBUTE_ACCESSORS(UAwAttributeSet, MaxMana);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	TMap<FName, FAwAttributeData> OtherAttributes;

	UPROPERTY(ReplicatedUsing = OnRep_OtherAttributes, VisibleDefaultsOnly, Category = "Attributes")
	TArray<FReplicaAttributesEntry_FName_FAwAttributeData> ReplicaAttributesArray;

	UFUNCTION()
	void OnRep_Health(const FAwAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_Mana(const FAwAttributeData& OldMana);

	UFUNCTION()
	void OnRep_OtherAttributes();

	UFUNCTION()
	void OnRep_ReplicaAttributeMap();

	UFUNCTION(BlueprintCallable)
	TMap<FName, FAwAttributeData> GetAttributes() const;

	UFUNCTION(BlueprintCallable)
	bool RegisterAttribute(FName AttributeName, FAwAttributeData AttributeData);

	UFUNCTION()
	bool ModifyAttributeBaseVal(FName AttributeName, float NewValue);

	UFUNCTION()
	bool ModifyAttributeCurrVal(FName AttributeName, float NewValue);
	
	FOnAwGameplayAttributeValueChange& GetAttributeChangeDelegate(FName AttributeName, AttributeChangedType Type);
	
	UFUNCTION()
	inline void SetOwningActor();
};
