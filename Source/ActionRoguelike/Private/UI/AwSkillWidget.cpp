// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AwSkillWidget.h"

void UAwSkillWidget::UICoolDownStart(UAwAction* Action, float CoolDownTime)
{
	OnCoolDownStart.Broadcast(Action, CoolDownTime);
}

void UAwSkillWidget::UICoolDownEnd(UAwAction* Action)
{
	OnCoolDownEnd.Broadcast(Action);
}
