#pragma once

namespace Debug
{
	static void Print(const FString& Message,const FColor& Color = FColor::MakeRandomColor(),int Key = -1)
	{
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(Key, 5.f, Color, Message);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
	}

}
