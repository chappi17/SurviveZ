#include "Player/PlayerAnimInstance.h"
#include "Global.h"
#include "Player/PlayerCharacter.h"
#include "Weapons/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"

UPlayerAnimInstance::UPlayerAnimInstance()
	:speed(0.f)
	, bIsInAir(false)
	, bAiming(false)
	, pitch(0.f)
	, bReloading(false)
	, bCrouching(false)
	, bAccelerating(false)
	, movementOffset(0.f)
	, lastMovementOffset(0.f)
	, CharacterYaw(0.f)
	, CharacterYawLastFrame(0.f)
	, RootYawOffset(0.f)
	, OffsetState(EOffsetState::EOS_Hip)
	, EquippedWeaponType(EWeaponType::EWT_MAX)
	, bShouldUseFABRIK(false)
{
}

void UPlayerAnimInstance::UpdateAnimationProperties(float Deltatime)
{
	if (Player == nullptr)
	{
		Player = Cast<APlayerCharacter>(TryGetPawnOwner());
	}
	if (Player)
	{
		bCrouching = Player->GetCrouching();
		bReloading = Player->GetCombatState() == ECombatState::ECS_Reloading;
		bEquipping = Player->GetCombatState() == ECombatState::ECS_Equipping;
		bShouldUseFABRIK = Player->GetCombatState() == ECombatState::ECS_Unocuupied || Player->GetCombatState() ==ECombatState::ECS_FireTimerInProgress;
		FVector Velocity = Player->GetVelocity();
		Velocity.Z = 0;
		speed = Velocity.Size();

		bIsInAir = Player->GetCharacterMovement()->IsFalling();
		bAccelerating = Player->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;

		FRotator AimRotation = Player->GetBaseAimRotation();
		FString RotationMessage = FString::Printf(TEXT("Aim Rotation: %f"), AimRotation.Yaw);

		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Player->GetVelocity());
		FString MovementRotationMessage = FString::Printf(TEXT("Movement Rotation: %f"), MovementRotation.Yaw);

		movementOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		if (Player->GetVelocity().Size() > 0.f)
		{
			lastMovementOffset = movementOffset;
		}

		bAiming = Player->GetAiming();

		//Logs::Print(RotationMessage);
		//Logs::Print(MovementRotationMessage);
		//Logs::Print(movementOffset);

		if (bReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if (bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (Player->GetAiming())
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}

		if (Player->GetEquippedWeapon())
		{
			EquippedWeaponType = Player->GetEquippedWeapon()->GetWeaponType();
		}
	}
	TurnInPlace();
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Player = Cast<APlayerCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::TurnInPlace()
{
	if (Player == nullptr) return;
	pitch = Player->GetBaseAimRotation().Pitch;

	// 속도가 0 일때 + 점프중일때는 체크 안함
	if (speed > 0 || bIsInAir)
	{
		RootYawOffset = 0.f;
		CharacterYaw = Player->GetActorRotation().Yaw;
		CharacterYawLastFrame = CharacterYaw;
		RotationCurveValueLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	// 
	else
	{
		CharacterYawLastFrame = CharacterYaw;
		CharacterYaw = Player->GetActorRotation().Yaw;
		const float YawDiff = { CharacterYaw - CharacterYawLastFrame };

		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDiff);

		const float Turning = { GetCurveValue(TEXT("Turning")) };

		if (Turning > 0)
		{
			RotationCurveValueLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("DistanceCurve"));
			const float DeltaRotation = { RotationCurve - RotationCurveValueLastFrame };

			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			const float ABSRootYawOffset = FMath::Abs(RootYawOffset);

			if (ABSRootYawOffset > 90.f)
			{
				const float YawExcess = { ABSRootYawOffset - 90.f };
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		//Logs::Print(YawDiff, 1);
		//Logs::Print(CharacterYaw,2);
		//Logs::Print(RootYawOffset,3);
		//Logs::Print(Turning,4);
	}
}
