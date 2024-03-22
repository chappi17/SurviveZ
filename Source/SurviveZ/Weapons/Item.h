// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity :uint8
{
	EIR_Basic UMETA(DisplayName = "Basic"), // 회색
	EIR_Common UMETA(DisplayName = "Common"), // 흰색
	EIR_UnCommon UMETA(DisplayName = "UnCommon"), // 초록색
	EIR_Rare UMETA(DisplayName = "Rare"), // 파랑색
	EIR_Legendary UMETA(DisplayName = "Legendary"), //주황색

	EIR_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemState :uint8
{
	EIS_PickUp UMETA(DisplayName = "PickUp"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIR_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemType :uint8
{
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_HealthPack UMETA(DisplayName = "HealthPack"),

	EIR_MAX UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FItemRarityTable : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FLinearColor GlowOutlineColor;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* IconBackGroundData;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 CustomDepthStencil;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MultyplyDamagebyRarity;
};

UCLASS()
class SURVIVEZ_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OverComp,int32 OtherBodyIndex
	, bool bFromSweep
	, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex);
	void SetChangeRarity();
	virtual void SetItemProperties(EItemState State);
	virtual void InitializeCustomDepth();
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	virtual void Tick(float DeltaTime) override;

	void SinEffect(float DeltaTime);
	void PlayPickupSound();
	void PlaySwapSound();

private:

	UPROPERTY(EditAnywhere, Category = "Item Property | Component", meta = (AllowPrivateAccess= "true"))
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, Category = "Item Property | Component", meta = (AllowPrivateAccess= "true"))
	class USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, Category = "Item Property | Component", meta = (AllowPrivateAccess= "true"))
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Property | Component", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickUpWidget;
	
	UPROPERTY(EditAnywhere, Category = "Item Property | Component", meta = (AllowPrivateAccess= "true"))
	class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Sounds", meta = (AllowPrivateAccess = "true"))
	class USoundCue* PickupSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Sounds", meta = (AllowPrivateAccess = "true"))
	USoundCue* SwapSound;


	bool bSinEffect;
	float currentTime;

	UPROPERTY(EditAnywhere, Category = "Item Property", meta = (AllowPrivateAccess= "true"))
	float waveSpeed;
	UPROPERTY(EditAnywhere, Category = "Item Property", meta = (AllowPrivateAccess= "true"))
	float waveHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Item Property", meta = (AllowPrivateAccess= "true"))
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Item Property", meta = (AllowPrivateAccess= "true"))
	int32 ItemCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Weapon", meta = (AllowPrivateAccess= "true"))
	EItemRarity ItemRarity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Item Property", meta = (AllowPrivateAccess= "true"))
	TArray<bool> ActiveColors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Item Property", meta = (AllowPrivateAccess= "true"))
	EItemState ItemState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Item Property", meta = (AllowPrivateAccess= "true"))
	EItemType ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "DataTable", meta = (AllowPrivateAccess= "true"))
	class UDataTable* ItemRarityDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "DataTable", meta = (AllowPrivateAccess= "true"))
	UTexture2D* IconBackgroundData;

	FTimerHandle EquipSoundTimer;

	bool bShouldPlayEquipSound;
	void ResetEquipSoundTimer();

	UPROPERTY(VisibleAnywhere,Category = "Widget",  meta = (AllowPrivateAccess = "true"))
	float EquipSoundResetTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Inventory ", meta = (AllowPrivateAccess= "true"))
	UTexture2D* IconBackground;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Inventory ", meta = (AllowPrivateAccess= "true"))
	UTexture2D* IconItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Inventory ", meta = (AllowPrivateAccess= "true"))
	UTexture2D* IconAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Inventory ", meta = (AllowPrivateAccess= "true"))
	int32 SlotIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	float MultyplyDamagebyRarity;

public:
	FORCEINLINE UWidgetComponent* GetPickUpWidget() const { return PickUpWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE void SetSinEffect(bool _bSinEffect) { bSinEffect = _bSinEffect; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE USoundCue* GetPickupSound() { return PickupSound; }
	FORCEINLINE void SetPickupSound(USoundCue* _equipsound) { PickupSound = _equipsound; }
	FORCEINLINE USoundCue* GetSwapSound() { return SwapSound; }
	FORCEINLINE void SetSwapSound(USoundCue* _swapsound) { SwapSound = _swapsound; }

	FORCEINLINE int32  GetItemCount() { return ItemCount; }
	FORCEINLINE EItemType GetItemType() { return ItemType; }
	FORCEINLINE EItemType SetItemType(EItemType Type) { return ItemType = Type; }
	FORCEINLINE EItemRarity GetItemRarity(EItemRarity Rarity) { return ItemRarity; }
	FORCEINLINE float GetItemRarityMultiplyDamage() { return MultyplyDamagebyRarity; }
	FORCEINLINE int32 GetSlotIndex() { return SlotIndex; }
	FORCEINLINE void SetSlotIndex(int32 _index) { SlotIndex = _index; }
	FORCEINLINE void SetItemName(FString _name) { ItemName = _name; }
	FORCEINLINE void SetItemIcon(UTexture2D* _icon) { IconItem = _icon; }
	FORCEINLINE void SetAmmoIcon(UTexture2D* _icon) { IconAmmo = _icon; }
	FORCEINLINE void SetItemRarity(EItemRarity _rarity) { ItemRarity = _rarity; }

	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	void StartEquipSoundTimer();

	void SetItemState(EItemState State);

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();


};
