// Fill out your copyright notice in the Description page of Project Settings.

#include "Global.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "Weapons/Weapon.h"

AWeapon::AWeapon():
ThrowWeaponTime(0.7f)
,bFalling(false)
,Ammo(20)
,AmmoCapacity(20)
,WeaponType(EWeaponType::EWT_Pistol)
,AmmoType(EAmmoType::EAT_9mm)
,ReloadMontageSection(FName(TEXT("Reload")))
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation = { 0.f,GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation,false,nullptr,ETeleportType::TeleportPhysics);
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetItemType(EItemType::EIT_Weapon);
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation = { 0.0f,GetItemMesh()->GetComponentRotation().Yaw,0.0f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward = { GetItemMesh()->GetForwardVector() };
	const FVector MeshRight = { GetItemMesh()->GetRightVector() };
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);

	float RandomRotation = 30.0f;
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection*= 500.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::DecrementAmmo()
{
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		Ammo--;
	}
}

void AWeapon::ReloadAmmo(int32 Amount)
{
	if (AmmoFull())
	{
		Logs::Print("Ammo Full");
	}

	Ammo += Amount;
}

bool AWeapon::AmmoFull()
{
	return Ammo >= AmmoReloadCapacity;
}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_PickUp);
}

void AWeapon::StartFire()
{
	UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();
	if (FireMontage)
	{
		AnimInstance->Montage_Play(FireMontage);
	}
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	const FString WeaponTablePath = TEXT("'/Game/Assets/DataTable/DT_Weapon.DT_Weapon'");
	UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));
	if (WeaponTableObject)
	{
			FWeaponDataTable* WeaponDataRow = nullptr;
		switch (WeaponType)
		{
		case EWeaponType::EWT_Pistol:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("Pistol"), TEXT(""));
			break;
		case EWeaponType::EWT_Rifle:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("Rifle"), TEXT(""));
			break;
		case EWeaponType::EWT_ShotGun:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("ShotGun"), TEXT(""));
			break;
		case EWeaponType::EWT_MAX:
			break;
		default:
			break;
		}

		if (WeaponDataRow)
		{
			AmmoType = WeaponDataRow->AmmoType;
			Ammo = WeaponDataRow->WeaponAmmo;
			AmmoCapacity = WeaponDataRow->AmmoCapacity;
			SetPickupSound(WeaponDataRow->PickupSound);
			SetSwapSound(WeaponDataRow->SwapSound);
			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);
			SetItemName(WeaponDataRow->ItemName);
			SetItemIcon(WeaponDataRow->InventoryIcon);
			SetAmmoIcon(WeaponDataRow->AmmoIcon);
			SetReloadMontageSection(WeaponDataRow->ReloadMontageSection);
			AutoFireRate = WeaponDataRow->AutoFireRate;
			MuzzleFlash = WeaponDataRow->MuzzleFlash;
			SetFireSound(WeaponDataRow->FireSound);
			MuzzleEffects = WeaponDataRow->MuzzleEffects;
			Damage = WeaponDataRow->Damage * GetItemRarityMultiplyDamage();
			HeadShotDamage = WeaponDataRow->HeadShotDamage * GetItemRarityMultiplyDamage();
			AmmoReloadCapacity = WeaponDataRow->AmmoReloadCapacity;
		}
	}
}
