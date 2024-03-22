
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerCharacter.h"
#include "Player/WeaponType.h"
#include "PlayerAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),


	EOS_MAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class SURVIVEZ_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPlayerAnimInstance();

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float Deltatime);

	virtual void NativeInitializeAnimation() override;
	
private:


#pragma region Basic property

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Movement", meta = (AllowPrivateAccess = "true"))
	APlayerCharacter* Player;
		
	// The speed of the Character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float speed;

	// The pitch of the aim rotation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Turn In Place", meta = (AllowPrivateAccess = "true"))
	float pitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float movementOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float lastMovementOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Movement", meta = (AllowPrivateAccess = "true"))
	bool bAccelerating;

	float CharacterYaw;
	float CharacterYawLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Movement", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;

	float RotationCurve;
	float RotationCurveValueLastFrame;

protected:

	void TurnInPlace();

#pragma endregion
private:
#pragma region Action State
	// true or false that Character is in the jumping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	// True when reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category =Combat, meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	//True when croching
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category ="Movement", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category =Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category =Combat, meta = (AllowPrivateAccess = "true"))
	bool bEquipping;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category =Combat, meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;

#pragma endregion

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category =Combat, meta = (AllowPrivateAccess = "true"))
	EWeaponType EquippedWeaponType;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category =Combat, meta = (AllowPrivateAccess = "true"))
	bool bShouldUseFABRIK;
};
