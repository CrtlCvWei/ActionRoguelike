// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\MyGAS/AWAttributeComp.h"
#include "AWPlayerState.h"
#include "AWGameModeBase.h"
#include "AWReward.h"
#include "GameFramework/PlayerState.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,text)

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
	if (AttributeSet->GetHealthBase() + AttributeSet->GetHealthCurrent() >= 0)	
	{
		// Base Health Base
		float Actural = v;
		if (AttributeSet->GetHealthCurrent() + v >= 0 && v < 0 )
		{
			// AttributeSet->SetHealthCurrent(AttributeSet->GetHealthCurrent() + v);
			
			Actural = 0;
		}
		else if(AttributeSet->GetHealthCurrent() + v < 0 && v < 0)
		{
			// AttributeSet->SetHealthCurrent(0);
			Actural = v + AttributeSet->GetHealthCurrent();
		}
		float NewHP = AttributeSet->GetHealthBase() + Actural;
		NewHP = NewHP > AttributeSet->GetMaxHealthBase() + AttributeSet->GetMaxHealthCurrent() ? AttributeSet->GetMaxHealthBase() + AttributeSet->GetMaxHealthCurrent() : NewHP;
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
		AttributeSet->SetHealthBase(NewHP);
		// trigger the event!
		AttributeChangeBoardCast("Health", Sourcer, AttributeSet->GetHealthBase() + AttributeSet->GetHealthCurrent(), v);
		return true;
	}
	return false;
}

bool UAWAttributeComp::SetAttributeBase(FName AttributeName, const float v,AActor* Sourcer)
{
	//TODO: MAKE SET ATTRIBUTE FUNCTION WORK
	if(AttributeName.ToString().Contains("Max"))
	{
		//	TODO : Max Attribute Case
		return false;
	}
	
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

	float NewValue = Data->GetBaseValue() + v;
	const float MaxValue = GetAttributeBase(FName("Max"+AttributeName.ToString()));
	NewValue = NewValue > MaxValue ? MaxValue : NewValue;
	NewValue = NewValue < 0 ? 0 : NewValue;
	
	Data->SetBaseValue(NewValue);
	AttributeDataChangeBroadcast(AttributeSet, AttributeName,  Data->GetBaseValue(),NewValue - v,AttributeChangedType::Base);
	
	check(NewValue == Data->GetBaseValue());
	
	// trigger the event!
	AttributeChangeBoardCast(AttributeName, Sourcer, NewValue, v);
	return true;
}

bool UAWAttributeComp::isAlive() const
{
	if(GetMaxHealth() > 0.f)
		if(GetHealth() > 0.f )
			return true;
	return false;
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

bool UAWAttributeComp::SetOwningActor()
{
	AAWPlayerState* OwningPlayerState = Cast<AAWPlayerState>(GetOuter());
	if (OwningPlayerState)
	{
		OwningActor = OwningPlayerState->GetPawn();
		return true;
	}

	if (AActor* Actor = Cast<AActor>(GetOuter()))
	{
		OwningActor = Actor;
		return true;
	}

	return false;
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

float UAWAttributeComp::GetActualAttribute(FName AttributeName) const
{
	return GetAttributeBase(AttributeName) + GetAttributeCurrent(AttributeName);
}

void UAWAttributeComp::AttributeChangeBoardCast(const FName Name, AActor* Instigator, float NewValue, float Change)
{
	if (ensureAlways(AllAttributeChangeMap.Contains(Name))) // check if the map contains the key
	{
		AllAttributeChangeMap[Name].Broadcast(Instigator, this, NewValue, Change);
	}
}
