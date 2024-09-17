#pragma once

namespace Debug
{
	static void Print(const FString& Message,const FColor& Color = FColor::MakeRandomColor(),int Key = -1,UObject* Context = nullptr,const float Duration = 5.f)

	{
		FString NewMessage;
		if (Context)
		{
			FString NetPrix = Context->GetWorld()->IsNetMode(NM_Client) ? FString("Client") : FString("Server");
			NewMessage = NetPrix + " : " + Message;
		}
		else
			NewMessage = Message;
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(Key, Duration, Color, NewMessage);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *NewMessage);
	}
	
}
