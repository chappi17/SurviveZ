// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Item.h"
#include "HealthItem.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVEZ_API AHealthItem : public AItem
{
	GENERATED_BODY()
public:
	AHealthItem();
	
protected:
		virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OverComp, int32 OtherBodyIndex
		, bool bFromSweep
		, const FHitResult& SweepResult) override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Health", meta = (AllowPrivateAccess= "true"))
	UStaticMeshComponent* HealthMesh;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Health", meta = (AllowPrivateAccess= "true"))
	float restoreHealth;

public:

	float GetresotrHealth() const { return restoreHealth; }
	
};
