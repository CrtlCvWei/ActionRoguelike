// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\MyGAS/AWAttributeComp.h"

#include "AWGameModeBase.h"
#include "AWReward.h"
#include "GameFramework/PlayerState.h"


inline  FAwAttributeData UAWAttributeComp::GetAttributeData(const FName AttributeName) const
{
	// using reflection to get the attribute
	auto& Attr = this->AttributeSet;
	FProperty* Prop = FindFieldChecked<FProperty>(AttributeSet.GetClass(), AttributeName);
	if (Prop->GetCPPType().Equals(TEXT("FAwAttributeData")))
	{
		FAwAttributeData* Data = Prop->ContainerPtrToValuePtr<FAwAttributeData>(Attr);
		if(Data)
			return *Data;
	}
	// failed
	return FAwAttributeData::ERROR;
}

// Sets default values for this component's properties
UAWAttributeComp::UAWAttributeComp()
{
	// ...
	AttributeSet = CreateDefaultSubobject<UAwAttributeSet>(TEXT("AttributeSet"));
	CreateAllAttributeChangeMap();
}

void UAWAttributeComp::CreateAllAttributeChangeMap()
{
	AllAttributeChangeMap.Empty();
	for (TFieldIterator<FProperty> Prop(this->AttributeSet->GetClass()); Prop; ++Prop)
	{
		if (Prop->GetCPPType().Equals(TEXT("FAwAttributeData")))
		{
			FName Name = Prop->GetFName();
			FOnAttributeChangeSignture Delegate;
			AllAttributeChangeMap.Add(Name, Delegate);
		}
	}
}

bool UAWAttributeComp::SetHealth(const float v,AActor* Sourcer)
{
	if (AttributeSet->GetHealth() >= 0)
	{
		float NewHP = AttributeSet->GetHealth() + v;
		NewHP = NewHP > AttributeSet->GetMaxHealth() ? AttributeSet->GetMaxHealth() : NewHP;
		NewHP = NewHP < 0 ? 0 : NewHP;
		if (NewHP == 0)
		{
			AAWGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AAWGameModeBase>();
			if (ensure(GameMode))
			{
				if (APlayerState* PS = Cast<APlayerState>(GetOuter()))
				{
					//Player has PlayerState
					GameMode->ActorBeenKilled(PS->GetPawn(), Sourcer, nullptr);
				}
				else
				{
					//AIPlayer has not PlayerState but has AttributeComp
					AActor* Owner = GetOwner();
					UAWReward* Rewards = Cast<UAWReward>(Owner->GetComponentByClass(UAWReward::StaticClass()));
					GameMode->ActorBeenKilled(Owner, Sourcer, Rewards);
				}
			}
		}
		AttributeSet->SetHealth(NewHP);
		// trigger the event!
		AttributeChangeBoardCast("Health", Sourcer, AttributeSet->GetHealth(), v);
		return true;
	}
	return false;
}

bool UAWAttributeComp::SetAttribute(FName AttributeName, const float v,AActor* Sourcer)
{
	//TODO: MAKE SET ATTRIBUTE FUNCTION WORK
	if(AttributeName.ToString().Contains("Max"))
	{
		//error not in this funtion
		return false;
	}

	// Health Case
	if (AttributeName == "Health" || AttributeName == "health" )
	{
		return SetHealth(v,Sourcer);
	}
	// Other Case
	static  FAwAttributeData* Data = nullptr;
	auto& Attr = this->AttributeSet;
	FProperty* Prop = FindFieldChecked<FProperty>(AttributeSet.GetClass(), AttributeName);
	if(!Prop)
		return false;
	if (Prop->GetCPPType().Equals(TEXT("FAwAttributeData")))
	{
		Data = Prop->ContainerPtrToValuePtr<FAwAttributeData>(Attr);
		if(!Data)
			return false;
	}
	else
		return false;

	float NewValue = Data->GetCurrentValue() + v;
	const float MaxValue = GetAttributeCurrent(FName("Max"+AttributeName.ToString()));
	NewValue = NewValue > MaxValue ? MaxValue : NewValue;
	Data->SetCurrentValue(NewValue);
	
	check(NewValue == Data->GetCurrentValue());
	
	// trigger the event!
	AttributeChangeBoardCast(AttributeName, Sourcer, NewValue, v);
	return true;
}

bool UAWAttributeComp::isAlive() const
{
	return (AttributeSet->GetHealth() > 0.f && AttributeSet->GetMaxHealth() > 0.f);
}

inline  float UAWAttributeComp::GetAttributeBase(FName AttributeName) const
{
	auto Attr = GetAttributeData(AttributeName);
	if (Attr.IsNotVaild())
	{
		return -std::numeric_limits<float>::infinity();
	}
	// return the base value	
	return Attr.GetBaseValue();
}

inline  float UAWAttributeComp::GetAttributeCurrent(FName AttributeName) const
{
	auto Attr = GetAttributeData(AttributeName);
	if (Attr.IsNotVaild())
	{
		return -std::numeric_limits<float>::infinity();
	}
	// return the Current value	
	return Attr.GetCurrentValue();
}

void UAWAttributeComp::AttributeChangeBoardCast(const FName Name, AActor* Instigator, float NewValue, float Change)
{
	if (ensureAlways(AllAttributeChangeMap.Contains(Name))) // check if the map contains the key
	{
		AllAttributeChangeMap[Name].Broadcast(Instigator, this, NewValue, Change);
	}
}
