// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVEZ_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"));
	float speed;

	TObjectPtr<class ABaseEnemy> Enemy;
};
