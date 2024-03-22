// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BulletHitInterface.generated.h"

UINTERFACE(MinimalAPI)
class UBulletHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SURVIVEZ_API IBulletHitInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void BulletHit(FHitResult HitResult,AActor* Shooter, AController* ShooterController);
};
