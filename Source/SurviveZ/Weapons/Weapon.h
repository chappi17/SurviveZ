// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Item.h"
#include "Player/AmmoType.h"
#include "Engine/DataTable.h"
#include "Player/WeaponType.h"
#include "Weapon.generated.h"



USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmoCapacity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmoReloadCapacity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* SwapSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ItemMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InventoryIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* AmmoIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageSection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem* MuzzleEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeadShotDamage;
};

UCLASS()
class SURVIVEZ_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
protected:
	void StopFalling();
	void StartFire();

	virtual void OnConstruction(const FTransform& Transform) override;

private:

	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 AmmoCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection;
	
	UPROPERTY(EditAnywhere, Category ="Weapon", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMesh;
	
	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapon DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapon DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* RarityTable;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	float AutoFireRate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* MuzzleEffects;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USoundCue* FireSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	float HeadShotDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 AmmoReloadCapacity;

public:
	void ThrowWeapon();
	void DecrementAmmo();
	void ReloadAmmo(int32 Amount);
	bool AmmoFull();
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetAmmoCapacity() const { return AmmoCapacity; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
	FORCEINLINE UAnimMontage* GetFireMontage() const { return FireMontage; }
	FORCEINLINE USoundCue* GetFireSound() const { return FireSound; }
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
	FORCEINLINE UNiagaraSystem* GetMuzzleEffects() const { return MuzzleEffects; }
	FORCEINLINE void SetReloadMontageSection(FName _section) { ReloadMontageSection = _section; }
	FORCEINLINE float GetFireRate() const { return AutoFireRate; }
	FORCEINLINE void SetFireSound(USoundCue* _sound) { FireSound = _sound; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }
	FORCEINLINE int32 GetAmmoReloadCapacity() const { return AmmoReloadCapacity; }
	
};
