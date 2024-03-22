// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/BaseEnemy.h"
#include "Enemy/EnemyAnimInstance.h"

void UEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Enemy == nullptr)
	{
		Enemy = Cast<ABaseEnemy>(TryGetPawnOwner());
	}

	if (Enemy)
	{
		FVector Velocity = Enemy->GetVelocity();
		Velocity.Z = 0.0f;
		speed = Velocity.Size();
	}
}
