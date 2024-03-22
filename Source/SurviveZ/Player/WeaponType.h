#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Pistol UMETA (DisplayName = "Pistol"),
	EWT_Rifle UMETA (DisplayName = "Rifle"),
	EWT_ShotGun UMETA(DisplayName = "ShotGun"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};
