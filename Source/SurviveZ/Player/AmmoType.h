#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm UMETA(DisplayName = "9mm"),
	EAT_5mm UMETA(DisplayName = "5mm"),
	EAT_12gauge UMETA(DisplayName = "12gauge"),

	EAT_MAX UMETA(DisplayName = "DefaultMax")
};
