// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "AmmoType.h"
#include "PlayerCharacter.generated.h"


UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unocuupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),
	ECS_Stunned UMETA(DisplayName = "Stunned"),

	EAT_MAX UMETA(DisplayName = "DefaultMax")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlowIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);

UCLASS()
class SURVIVEZ_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	APlayerCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
#pragma region Camera
	//Components
	UPROPERTY(EditAnywhere, Category = "Basic State | Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "Basic State | Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat | Camera", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Camera", meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	float CameraFOV;
	float AimCameraFOV;
	float CameraCurrentFOV;



#pragma endregion


#pragma region Weapon & Bullet

	//UPROPERTY(EditAnywhere, Category ="Weapon", meta = (AllowPrivateAccess = "true"))
	//class USkeletalMeshComponent* Pistol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoCapacityMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 Ammo9mmCapacity;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 Ammo5mmCapacity;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 Ammo12GaugeCapacity;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	UPROPERTY(EditAnywhere, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;
	
	UPROPERTY(EditAnywhere, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;

	UPROPERTY(EditAnywhere, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GetHitMontage;

	UPROPERTY(EditAnywhere, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeadMontage;

	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	float StunChance;
	
	UFUNCTION(BlueprintCallable)
	void FinishReloding();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void FinishDodge();

public:
	UFUNCTION(BlueprintCallable)
	void FinishDeath();
protected:
	AWeapon* SpawnDefaultWeapon();
	void EquipWeapon(AWeapon* WeaponToEquip);
	void DropWeapon();
	void SwapWeapon(AWeapon* WeaponToSwap);
	void InitializeAmmoMap();
	bool WeaponHasAmmo();
	void ReloadWeapon();
	bool CarryingAmmo();

#pragma endregion
private:

#pragma region Sounds

	UPROPERTY(EditAnywhere, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	class USoundCue* HitSound;
	
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundCue* DeathSound;

#pragma endregion 


#pragma region Effests

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* bulletTrace;

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* smokeBeam;
	
	//UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	//class UNiagaraSystem* BulletTarceEffect;
	//UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	//UNiagaraSystem* SmokeBeamEffect;
	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodEffect;
#pragma endregion

#pragma region Anim & Crouch

	UPROPERTY(EditAnywhere, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollingMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	UPROPERTY(VisibleAnywhere, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	float CrouchMovementSpeed;

	UPROPERTY(EditAnywhere, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	float CurrentCapsuleHalfHeight;

	UPROPERTY(EditAnywhere, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;

	UPROPERTY(EditAnywhere, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;

	UPROPERTY(EditAnywhere, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction;

	UPROPERTY(EditAnywhere, Category = "Crouch", meta = (AllowPrivateAccess = "true"))
	float CrouchingGroundFriction;

protected:
	void InterpCapsuleHalfHeight(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

	UFUNCTION(BlueprintCallable)
	void EndStun();
	
	void Die();


#pragma endregion
private:
#pragma region Crosshair & Fire
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairRunFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairFireFactor;

	void CalculateCrosshairSpread(float DeltaTime);

	float FireTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairFireTimer;

	bool bFireButtonPressed;
	bool bShouldFire;
	FTimerHandle AutoFireTimer;

	bool bHitReactTime;
	float HitReactTimerate;
	FTimerHandle HitReactTimer;


protected:

	void StartCrosshairBulletFire();
	void StartFireTimer();

	UFUNCTION()
	void FinishCrosshairBulletFire();
	UFUNCTION()
	void AutoFireReset();

#pragma endregion
private:

#pragma region Widget & Inventory
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	bool bShouldTraceForItems;

	int8 OverlappedItemCount;

	UPROPERTY(VisibleAnywhere, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;

	const int32 INVENTORY_CAPACITY = 6;

	UPROPERTY(BlueprintAssignable, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	FHighlightIconDelegate HighlightIconDelegate;

	UPROPERTY(VisibleAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 HighlightSlot;

	void HighlightInventorySlot();
public:
	void UnHighlightInventorySlot();
	void IncrementOverlappedItemCount();
	void DecrementOverlappedItemCount();
	void GetPickupItem(AItem* Item);
	void PickupAmmo(class AAmmo* Ammo);
	void PickupHealth(class AHealthItem* _Health);

protected:

	int32 GetEmptyInventorySlot();
#pragma endregion
#pragma region Property
	// Property *생성자에서 초기화 시켜줘야함
private:

	UPROPERTY(VisibleAnywhere, Category = "Basic State | Mouse Input",meta = (AllowPrivateAccess = "true"))
	float baseTurnRate;
	UPROPERTY(VisibleAnywhere, Category = "Basic State | Mouse Input",meta = (AllowPrivateAccess = "true"))
	float baseLookUpRate;

	UPROPERTY(VisibleAnywhere, Category = "Basic State | Mouse Input", meta = (AllowPrivateAccess = "true"),meta = (ClampMin = "0.0",ClampMax = "1.0",UIMin = "0.0", UIMax = "1.0"))
	float AimTurnRate;
	UPROPERTY(VisibleAnywhere, Category = "Basic State | Mouse Input", meta = (AllowPrivateAccess = "true"),meta = (ClampMin = "0.0",ClampMax = "1.0",UIMin = "0.0", UIMax = "1.0"))
	float AimLookUpRate;
	UPROPERTY(VisibleAnywhere, Category = "Basic State | Mouse Input", meta = (AllowPrivateAccess = "true"),meta = (ClampMin = "0.0",ClampMax = "1.0",UIMin = "0.0", UIMax = "1.0"))
	float IdleTurnRate;
	UPROPERTY(VisibleAnywhere, Category = "Basic State | Mouse Input", meta = (AllowPrivateAccess = "true"),meta = (ClampMin = "0.0",ClampMax = "1.0",UIMin = "0.0", UIMax = "1.0"))
	float IdleLookUpRate;

	UPROPERTY(VisibleAnywhere, Category = "Basic State | Mouse Input",meta = (AllowPrivateAccess = "true"))
	float MouseAimTurnRate;
	UPROPERTY(VisibleAnywhere, Category = "Basic State | Mouse Input",meta = (AllowPrivateAccess = "true"))
	float MouseAimLookUpRate;
	UPROPERTY(VisibleAnywhere, Category = "Basic State | Mouse Input",meta = (AllowPrivateAccess = "true"))
	float MouseIdleTurnRate;
	UPROPERTY(VisibleAnywhere, Category = "Basic State | Mouse Input",meta = (AllowPrivateAccess = "true"))
	float MouseIdleLookUpRate;

	UPROPERTY(VisibleAnywhere, Category = "Basic State | Movement",meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Basic State | Properties", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Basic State | Properties", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;
	
#pragma endregion

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void TraceForItems();
	void SetLookRates();
	void InterpFOV(float DeltaTime);


#pragma region InputClass
	//Input Class 

	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	class UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	class UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* AimingAction;

	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* ReloadAction;
	
	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* FireAction;
	
	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* InterectionAction;
	
	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* FAction;

	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* OneAction;

	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* TwoAction;

	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* ThreeAction;

	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* FourAction;

	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* FiveAction;
	
	UPROPERTY(EditAnywhere, Category ="Basic State | Input")
	UInputAction* RollingAction;
#pragma endregion
	
#pragma region Input Action
	//Input Action
	void Move(const struct FInputActionValue& _value);
	void Look(const FInputActionValue& _value);
	virtual void Jump() override;
	void AimingPressed();
	void AimingReleased();
	void ReloadPressed();
	void Fire();
	void SendBullet();
	void FirePressed();
	void FireReleased();
	void InterectPressed();
	void InterectReleased();
	void CrouchPressed();
	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();

	void ExchageInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);
#pragma endregion

#pragma region LineTrace
	void PlayFireSound();
	void SpawnsmokeBeamEffect(const FTransform& SocketTransform, const FVector& BeamEndPoint);
	void SpawnMuzzleEffect(const FTransform& SocketTransform);
	void SpawnbulletTraceEffect(const FVector& BeamEndPoint);
	void PlayFireAnimation();
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);
#pragma endregion

public:

	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	FORCEINLINE UCameraComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE bool GetAiming() const { return bAiming; }
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }
	FORCEINLINE AItem* GetTraceHitLastFrame() const { return TraceHitItemLastFrame; }
	FORCEINLINE void SetTraceHitLastFrame(AItem* lastFrame) { TraceHitItemLastFrame = lastFrame; }
	FORCEINLINE AItem* GetTraceHit() const { return TraceHitItem; }
	FORCEINLINE void SetTraceHit(AItem* lastFrame) { TraceHitItem = lastFrame; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE UParticleSystem* GetBloodEffect() const { return BloodEffect; }
	FORCEINLINE float GetStunChance() const { return StunChance; }
	void Stun();

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;



};
