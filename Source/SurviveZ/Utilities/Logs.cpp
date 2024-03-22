#include "Utilities/Logs.h"
#include "Engine.h"
#include "Logs.h"


void Logs::Print(int32 InValue, int32 InKey, float Duration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, FString::FromInt(InValue)); // 화면에 디버그 띄우는거
}

void Logs::Print(float InValue, int32 InKey, float Duration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, FString::SanitizeFloat(InValue));
}

void Logs::Print(const FString& InValue, int32 InKey, float Duration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, InValue);
}

void Logs::Print(const FVector& InValue, int32 InKey, float Duration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, InValue.ToString());
}

void Logs::Print(const FRotator& InValue, int32 InKey, float Duration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, InValue.ToString());

}
void Logs::Print(const FVector2D& InValue, int32 InKey, float Duration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, InValue.ToString());
}
