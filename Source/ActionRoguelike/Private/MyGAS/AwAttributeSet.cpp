// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGAS/AwAttributeSet.h"
// #include "AbilitySystemComponent.h"
#include "AWPlayerState.h"
#include "GameplayEffectAggregator.h"
#include "Debug/DebugHelper.h"




/**/
const FAwAttributeData FAwAttributeData::ERROR = FAwAttributeData(std::numeric_limits<float>::lowest());
/**/


float FAwAttributeData::GetCurrentValue() const
{
	return CurrentVal;
}

void FAwAttributeData::SetCurrentValue(float NewValue)
{
	CurrentVal = NewValue;
}

float FAwAttributeData::GetBaseValue() const
{
	return BaseVal;
}

void FAwAttributeData::SetBaseValue(float NewValue)
{
	BaseVal = NewValue;
}

void UAwAttributeSet::CreateAttributeDataChangeDelegates()
{
	AttributeCurrValueChangeDelegates.Empty(); // init
	AttributeBaseValueChangeDelegates.Empty();
	for (TFieldIterator<FProperty> Prop(this->GetClass()); Prop; ++Prop)
	{
		if (Prop->GetCPPType().Equals(TEXT("FAwAttributeData")))
		{
			FName Name = Prop->GetFName();
			FOnAwGameplayAttributeValueChange CurrDelegate;
			FOnAwGameplayAttributeValueChange BaseDelegate;
			AttributeCurrValueChangeDelegates.Add(Name, CurrDelegate);
			AttributeBaseValueChangeDelegates.Add(Name, BaseDelegate);
		}
	}
}

void UAwAttributeSet::AttributeDataChangeDelegate(FName Name, float NewValue, float OldValue,
	AttributeChangedType Type)
{
	MultiCastAttributeDataChangeDelegate(Name, NewValue, OldValue, Type);
}

void UAwAttributeSet::MultiCastAttributeDataChangeDelegate_Implementation(FName Name, float NewValue, float OldValue,
	AttributeChangedType Type)
{
	if(Type == Current && AttributeCurrValueChangeDelegates.Contains(Name))
		AttributeCurrValueChangeDelegates[Name].Broadcast(NewValue, OldValue);
	if(Type == Base && AttributeBaseValueChangeDelegates.Contains(Name))
		AttributeBaseValueChangeDelegates[Name].Broadcast(NewValue, OldValue);
}

UAwAttributeSet::UAwAttributeSet()
{
	OwningActor = nullptr;

	InitFun();
}

void UAwAttributeSet::InitFun()
{
	this->OtherAttributes = TMap<FName,FAwAttributeData>();
	CreateAttributeDataChangeDelegates();
	InitMaxHealth(100);
	InitHealth(100);
	InitMaxMana(100);
	InitMana(100);
	InitAttack(0);
}

void UAwAttributeSet::OnRep_Health()
{
	Debug::Print("On Rep Health",FColor::Red,-1);
}

void UAwAttributeSet::OnRep_Mana(const FAwAttributeData& OldMana)
{
	Debug::Print("On Rep Mana",FColor::Red,-1);
}

void UAwAttributeSet::OnRep_OtherAttributes()
{
	if(!OwningActor || !OwningActor->HasAuthority())
	{
		// OnRep_ReplicaAttributeMap();
		return;
	}
	const TArray<TPair<FName,FAwAttributeData>> OldOtherAttributes = OtherAttributes.Array();
	for(auto& Pair : OldOtherAttributes)
	{
		FReplicaAttributesEntry_FName_FAwAttributeData Entry(Pair);
		ReplicaAttributesArray.Emplace(Entry);
	}
	// MARK_PROPERTY_DIRTY_FROM_NAME(UAwAttributeSet, ReplicaAttributesArray, this);
}

void UAwAttributeSet::OnRep_ReplicaAttributeMap()
{
	OtherAttributes.Reset();
	for(const auto& MapEntry : ReplicaAttributesArray)
	{
		OtherAttributes.Emplace(MapEntry.Key,MapEntry.Value);
	}
	UE_LOG(LogTemp, Warning, TEXT("Replicated OtherAttributes <--- TMap"));
}

TMap<FName, FAwAttributeData> UAwAttributeSet::GetAttributes() const
{
	return this->OtherAttributes;
}

bool UAwAttributeSet::ModifyAttributeBaseVal(FName AttributeName, float NewValue)
{
	if(OtherAttributes.Contains(AttributeName))
	{
		FAwAttributeData& AttributeData = OtherAttributes[AttributeName];
		// 可以在这里添加一些逻辑来处理属性修改前的操作
		AttributeData.SetBaseValue(NewValue);
		return true;
	}
	return false;
}

bool UAwAttributeSet::ModifyAttributeCurrVal(FName AttributeName, float NewValue)
{
	if(OtherAttributes.Contains(AttributeName))
	{
		FAwAttributeData& AttributeData = OtherAttributes[AttributeName];
		// 可以在这里添加一些逻辑来处理属性修改前的操作
		AttributeData.SetCurrentValue(NewValue);
		return true;
	}
	return false;
}

FOnAwGameplayAttributeValueChange& UAwAttributeSet::GetAttributeChangeDelegate(FName AttributeName,
	AttributeChangedType Type)
{
	if(Type == Current)
	{
		return AttributeCurrValueChangeDelegates[AttributeName];
	}
	else
	{
		return AttributeBaseValueChangeDelegates[AttributeName];
	}
}

bool UAwAttributeSet::RegisterAttribute(FName AttributeName, FAwAttributeData AttributeData)
{
	if(OtherAttributes.Find(AttributeName)!=nullptr)
	{
		return false;
	}
	OtherAttributes.Add(AttributeName,AttributeData);
	return true;
}

inline void UAwAttributeSet::SetOwningActor()
{
	AAWPlayerState* OwningPlayerState = Cast<AAWPlayerState>(GetOuter());
	if(OwningPlayerState)
	{
		OwningActor = OwningPlayerState->GetOwner();
	}
	else
	{
		OwningActor = Cast<AActor>( GetOuter());
	}
}

void AttributeDataChangeBroadcast(UAwAttributeSet* AttributeSet, FName AttributeName, float NewValue, float OldValue,
	AttributeChangedType Type)
{
	AttributeSet->AttributeDataChangeDelegate(AttributeName, NewValue, OldValue, Type);
}

void UAwAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAwAttributeSet,OwningActor);
	DOREPLIFETIME(UAwAttributeSet,Health);
}
