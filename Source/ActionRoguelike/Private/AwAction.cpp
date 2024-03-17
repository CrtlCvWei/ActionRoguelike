// Fill out your copyright notice in the Description page of Project Settings.


#include "AwAction.h"
#include "AwCharacter.h"

#include "AwActionComponent.h"

void UAwAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Running: %s"), *GetNameSafe(this));

	UAwActionComponent* Comp = GetOwningComponent();
	if (Comp)
	{
			if (!this->GrandTags.IsEmpty())
				Comp->ActiveGameplayTags.AppendTags(this->GrandTags);
			if (!this->BlockTags.IsEmpty())
				Comp->BlockGamePlayTags.AppendTags(this->BlockTags);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, FString("No Comp"));
	}
}


void UAwAction::StopAction_Implementation(AActor* Instigator)
{
	// UE_LOG(LogTemp, Warning, TEXT("Stopping: %s"), *GetNameSafe(this));

	UAwActionComponent* Comp = GetOwningComponent();
	if (ensure(Comp))
	{
		if (Comp->ActiveGameplayTags.HasAny(GrandTags))	
			Comp->ActiveGameplayTags.RemoveTags(GrandTags);
		if (Comp->BlockGamePlayTags.HasAny(BlockTags))
			Comp->BlockGamePlayTags.RemoveTags(BlockTags);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString("No Comp"));
	}
}

bool UAwAction::CheckActionAvailable(AActor* Instigator) const
{
	UAwActionComponent* Comp = Cast<UAwActionComponent>(GetOuter());
	if(Comp->ActiveGameplayTags.HasAny(BlockTags))
	{
		return false;
	}
	return  true;
}

UAwActionComponent* UAwAction::GetOwningComponent() const
{
	return Cast<UAwActionComponent>(GetOuter());
}

FName UAwAction::GetActionName() const
{
	return this->ActionName;
}

UWorld* UAwAction::GetWorld() const
{
	AActor* Actor = Cast<AActor>(GetOuter()->GetOuter());

	if (Actor)
	{
		return Actor->GetWorld();
	}
	return nullptr;
}
