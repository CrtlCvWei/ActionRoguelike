// Fill out your copyright notice in the Description page of Project Settings.


#include "AWPlayerState.h"

#include "Debug/DebugHelper.h"
#include "Net/UnrealNetwork.h"

int32 AAWPlayerState::GetCredits() const
{
	return Credits;
}

int32 AAWPlayerState::GetScores() const
{
	return Scores;
}

void AAWPlayerState::AddCredits(int32 v)
{
	if (!ensure(v > 0.f))
	{
		return;
	}
	this->Credits += v;
	// 0 is the Credits
	StateChangeEvent.Broadcast(Cast<AAwCharacter>(GetOwner()), 0, Credits, v);
}

void AAWPlayerState::AddScores(int32 v)
{
	if (!ensure(v > 0.f))
	{
		return;
	}
	this->Scores += v;
	// 1 is the Scores
	StateChangeEvent.Broadcast(Cast<AAwCharacter>(GetOwner()), 1, Scores, v);
}

bool AAWPlayerState::RemoveCredits(int32 v)
{
	if (!ensure(v < 0.f))
	{
		return false;
	}
	if (Credits < v)
	{
		//not enough credits
		return false;
	}
	Credits -= v;
	StateChangeEvent.Broadcast(Cast<AAwCharacter>(GetOwner()), 0, Credits, v);
	return true;
}

bool AAWPlayerState::RemoveScores(int32 v)
{
	if (!ensure(v < 0.f))
	{
		return false;
	}
	if (Scores < v)
	{
		//not enough credits
		return false;
	}
	Scores -= v;
	StateChangeEvent.Broadcast(Cast<AAwCharacter>(GetOwner()), 1, Scores, v);
	return true;
}


UAWAttributeComp* AAWPlayerState::GetPlayerAttribute() const
{
	return PlayerAttributeComp;
}

UAwActionComponent* AAWPlayerState::GetPlayerAction() const
{
	return PlayerActionComp;
}

void AAWPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME(AAWPlayerState, PlayerActionComp);
	// DOREPLIFETIME(AAWPlayerState, PlayerAttributeComp);
}

void AAWPlayerState::OnRep_ActionComp()
{
	Debug::Print("AAAAAAAA", FColor::Red, -1.f, nullptr, 5.f);
}

void AAWPlayerState::OnRep_AttributeComp()
{
	Debug::Print("VVVVVVV", FColor::Red, -1.f, nullptr, 5.f);
}

AAWPlayerState::AAWPlayerState()
{
		PlayerAttributeComp = CreateDefaultSubobject<UAWAttributeComp>(TEXT("PlayerAttributeComp"));
		PlayerActionComp = CreateDefaultSubobject<UAwActionComponent>(TEXT("PlayerActionComp"));
		bReplicates = true;
		PlayerActionComp->SetOwningActor();
		PlayerAttributeComp->GetAttributeSet()->SetOwningActor();
		PlayerAttributeComp->SetOwningActor();
		//
}
