// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Item.h"
#include "Player/AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVEZ_API AAmmo : public AItem
{
	GENERATED_BODY()
	
public:
	AAmmo();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetItemProperties(EItemState State) override;
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OverComp, int32 OtherBodyIndex
		, bool bFromSweep
		, const FHitResult& SweepResult) override;


private:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Ammo", meta = (AllowPrivateAccess= "true"))
	UStaticMeshComponent* AmmoMesh;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Ammo", meta = (AllowPrivateAccess= "true"))
	EAmmoType AmmoType;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Ammo", meta = (AllowPrivateAccess= "true"))
	int32 AmmoCount;

public:

	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE int32 GetAmmoCount() const { return AmmoCount; }

	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;
};
