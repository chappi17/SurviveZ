#include "Player/PlayerCharacter.h"
#include "Global.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Weapons/Item.h"
#include "Components/WidgetComponent.h"
#include "Weapons/Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Enemy/BaseEnemy.h"
#include "BulletHitInterface.h"
#include "Weapons/HealthItem.h"
#include "SurviveZ.h"
#include "Weapons/Ammo.h"


APlayerCharacter::APlayerCharacter()
	// 기본 세팅 
	:baseTurnRate(45.0f)
	,baseLookUpRate(45.0f)
	,bAiming(false)
	// 카메라 세팅
	,CameraFOV(0.f)
	,AimCameraFOV(50.f)
	,CameraCurrentFOV(0.f)
	,ZoomInterpSpeed(20.f)
	// 에임 세팅
	,AimTurnRate(90.f)
	,AimLookUpRate(90.f)
	,IdleLookUpRate(20.f)
	,IdleTurnRate(20.f)
	// 감도 세팅
	,MouseAimTurnRate(0.2f)
	,MouseAimLookUpRate(0.2f)
	,MouseIdleLookUpRate(1.0f)
	,MouseIdleTurnRate(1.0f)
	// 크로스헤어 세팅
	,CrosshairRunFactor(0.f)
	,CrosshairInAirFactor(0.f)
	,CrosshairAimFactor(0.f)
	,CrosshairFireFactor(0.f)
	,FireTimeDuration(0.05f)
	,bFiringBullet(false)
	,bShouldTraceForItems(false)
	// 발사 세팅
	,bFireButtonPressed(false)
	,bShouldFire(true)
	// 총알 세팅
	,Ammo9mmCapacity(85)
	,Ammo5mmCapacity(120)
	,Ammo12GaugeCapacity(30)
	,CombatState(ECombatState::ECS_Unocuupied)
	// Crouch
	,bCrouching(false)
	,CrouchMovementSpeed(300.f)
	,BaseMovementSpeed(600.f)
	,StandingCapsuleHalfHeight(88.f)
	,CrouchingCapsuleHalfHeight(44.f)
	,BaseGroundFriction(8.f)
	,CrouchingGroundFriction(100.f)
	// Inventory
	,HighlightSlot(-1)
	// 체력 세팅
	,Health(100.0f)
	,MaxHealth(100.0f)
	// Hit
	,StunChance(0.25f)
	,bHitReactTime(false)
	,HitReactTimerate(1.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	Helpers::CreateComponent<USpringArmComponent>(this, &SpringArm, "SpringArm", RootComponent);
	Helpers::CreateComponent<UCameraComponent>(this, &CameraBoom, "Camera", SpringArm);


	// Mesh와 springarm rotation 관련
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->AddRelativeRotation(FRotator(0, -90, 0));
	SpringArm->SetRelativeLocation(FVector(0, 0, 120));

	// SpringArm 설정
	SpringArm->bDoCollisionTest = false; // 카메라의 충돌을 허용할 것이냐 
	SpringArm->bUsePawnControlRotation = true; // 폰의 회전에 같이 회전할 것이냐
	SpringArm->TargetArmLength = 250.0f; // 카메라와 폰 과의 거리
	SpringArm->SocketOffset = FVector(0, 80, 30); // 상대적 거리 
	SpringArm->bEnableCameraLag = true;
	CameraBoom->bUsePawnControlRotation = false;

	// 움직임 설정
	GetCharacterMovement()->GravityScale = 1.0f;
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxAcceleration = 2400.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
	GetCharacterMovement()->BrakingFriction = 6.0f;
	GetCharacterMovement()->GroundFriction = 8.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1400.0f;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;


	// 회전 설정
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		enhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::Jump);
		enhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Triggered, this, &APlayerCharacter::AimingPressed);
		enhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Completed, this, &APlayerCharacter::AimingReleased);
		enhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ReloadPressed);
		enhancedInputComponent->BindAction(InterectionAction, ETriggerEvent::Completed, this, &APlayerCharacter::InterectPressed);
		enhancedInputComponent->BindAction(InterectionAction, ETriggerEvent::Completed, this, &APlayerCharacter::InterectReleased);
		enhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::FirePressed);
		enhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::FireReleased);
		enhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::CrouchPressed);
		enhancedInputComponent->BindAction(FAction, ETriggerEvent::Completed, this, &APlayerCharacter::FKeyPressed);
		enhancedInputComponent->BindAction(OneAction, ETriggerEvent::Completed, this, &APlayerCharacter::OneKeyPressed);
		enhancedInputComponent->BindAction(TwoAction, ETriggerEvent::Completed, this, &APlayerCharacter::TwoKeyPressed);
		enhancedInputComponent->BindAction(ThreeAction, ETriggerEvent::Completed, this, &APlayerCharacter::ThreeKeyPressed);
		enhancedInputComponent->BindAction(FourAction, ETriggerEvent::Completed, this, &APlayerCharacter::FourKeyPressed);
		enhancedInputComponent->BindAction(FiveAction, ETriggerEvent::Completed, this, &APlayerCharacter::FiveKeyPressed);
	}

}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.0f)
	{
		Health = 0.0f;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void APlayerCharacter::FinishReloding()
{
	if (CombatState == ECombatState::ECS_Stunned) return;
	CombatState = ECombatState::ECS_Unocuupied;
	if (EquippedWeapon == nullptr) return;

	if (AmmoMap.Contains(EquippedWeapon->GetAmmoType()))
	{
		//EquippedWeapon's carrying Ammo
		int32 CarriedAmmo = AmmoMap[EquippedWeapon->GetAmmoType()];
		// 장전 되어있는 총알 갯수
		const int32 rounds = EquippedWeapon->GetAmmoReloadCapacity() - EquippedWeapon->GetAmmo();

		if (rounds > CarriedAmmo)
		{
			// 들고 있는 탄환이 장전되어있는 탄보다 작을 때
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(EquippedWeapon->GetAmmoType(), CarriedAmmo);
		}
		else
		{
			// 들고 있는 탄환이 장전되어있는 탄보다 많을 때
			EquippedWeapon->ReloadAmmo(rounds);
			CarriedAmmo -= rounds;
			AmmoMap.Add(EquippedWeapon->GetAmmoType(), CarriedAmmo);
		}
	}
}

void APlayerCharacter::FinishEquipping()
{
	CombatState = ECombatState::ECS_Unocuupied;
}

void APlayerCharacter::FinishDodge()
{
	Logs::Print("FinishDodge");
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
}


AWeapon* APlayerCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void APlayerCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		WeaponToEquip->SetSinEffect(false);
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("BarrelSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
				
		if (EquippedWeapon == nullptr)
		{
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}
		else
		{
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
		}
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void APlayerCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTranformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTranformRules);
		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void APlayerCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
Logs::Print(EquippedWeapon->GetSlotIndex(),10);
	if (Inventory.Num() - 1 >= EquippedWeapon->GetSlotIndex())
	{
		Inventory[EquippedWeapon->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquippedWeapon->GetSlotIndex());
	}

	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void APlayerCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Ammo9mmCapacity);
	AmmoMap.Add(EAmmoType::EAT_5mm, Ammo5mmCapacity);
	AmmoMap.Add(EAmmoType::EAT_12gauge, Ammo12GaugeCapacity);
}

bool APlayerCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

void APlayerCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unocuupied) return;
	if (EquippedWeapon == nullptr) return;

	bAiming = false;

	if (CarryingAmmo() && !EquippedWeapon->AmmoFull())
	{
		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (ReloadMontage && AnimInstance)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
	}
}

bool APlayerCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}

	return false;

}

void APlayerCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight;
	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}
	const float InterpHalfHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),TargetCapsuleHalfHeight,DeltaTime,20.f) };

	const float DeltaCapsuleHalfHeight = { InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	const FVector MeshOffset = { 0.f,0.f,-DeltaCapsuleHalfHeight };
	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

EPhysicalSurface APlayerCharacter::GetSurfaceType()
{
	FHitResult HitResult;
	const FVector Start = GetActorLocation();
	const FVector End = Start + FVector(0.f, 0.f, -400.f);
	FCollisionQueryParams QueryParms;
	QueryParms.bReturnPhysicalMaterial = true;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility,QueryParms);
	auto HitSurface = HitResult.PhysMaterial->SurfaceType;
	return 	UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

void APlayerCharacter::EndStun()
{
	CombatState = ECombatState::ECS_Unocuupied;
	Logs::Print("ChangeCombateState");
}

void APlayerCharacter::Die()
{
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	if (Anim && DeadMontage && DeathSound)
	{
		Anim->Montage_Play(DeadMontage);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	}
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void APlayerCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		DisableInput(PC);
	}
}

void APlayerCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange = { 0.f,600.f };
	FVector2D VelocityRatioRange = { 0.f,1.f };
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	//When character running, the velocity value will biggger and that is affect to Crosshair Spread
	CrosshairRunFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityRatioRange, Velocity.Size());

	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 2.25f, DeltaTime, 30.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	if (bFiringBullet)
	{
		CrosshairFireFactor = FMath::FInterpTo(CrosshairFireFactor, 0.3f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairFireFactor = FMath::FInterpTo(CrosshairFireFactor, 0.0f, DeltaTime, 60.f);
	}

	CrosshairSpreadMultiplier = 0.5f + CrosshairRunFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairFireFactor;
}

void APlayerCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(CrosshairFireTimer, this, &APlayerCharacter::FinishCrosshairBulletFire, FireTimeDuration);
}

void APlayerCharacter::StartFireTimer()
{
	if (EquippedWeapon == nullptr) return;

	CombatState = ECombatState::ECS_FireTimerInProgress;
	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &APlayerCharacter::AutoFireReset, EquippedWeapon->GetFireRate());
}

void APlayerCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void APlayerCharacter::AutoFireReset()
{
	if (CombatState == ECombatState::ECS_Stunned) return;
	CombatState = ECombatState::ECS_Unocuupied;

	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed)
		{
			Fire();
		}
	}
	else
	{
		// 재장전
		ReloadWeapon();
	}
}
bool APlayerCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult,FVector& OutHitLocation)
{
	FVector2D viewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}

	FVector2D CrosshairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);
	FVector CrossahairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation
		, CrossahairWorldPosition
		, CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		const FVector Start = { CrossahairWorldPosition };
		const FVector End = { Start + CrosshairWorldDirection * 50'000.f };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}


void APlayerCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot = GetEmptyInventorySlot();
	HighlightIconDelegate.Broadcast(EmptySlot, true);
	HighlightSlot = EmptySlot;
}

void APlayerCharacter::UnHighlightInventorySlot()
{
	HighlightIconDelegate.Broadcast(HighlightSlot, false);
	HighlightSlot = -1;
}

void APlayerCharacter::IncrementOverlappedItemCount()
{
	// Overlap된 아이템이 0보다 크다면 -> 하나라도 있으면 WidgetTrace 켜고 아니면 끄기
	OverlappedItemCount ++;
	
	if (OverlappedItemCount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		bShouldTraceForItems = true;
	}
}

void APlayerCharacter::DecrementOverlappedItemCount()
{
	if (OverlappedItemCount > 0)
	{
		OverlappedItemCount--;
	}
	if (OverlappedItemCount == 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CameraBoom)
	{
		CameraFOV = GetCameraBoom()->FieldOfView;
		CameraCurrentFOV = CameraFOV;
	}
	EquipWeapon(SpawnDefaultWeapon());
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);

	InitializeAmmoMap();
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	if (APlayerController* PlayerControllers = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerControllers->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	InterpFOV(DeltaTime);
	SetLookRates();
	CalculateCrosshairSpread(DeltaTime);
	TraceForItems();
	InterpCapsuleHalfHeight(DeltaTime);

	//Logs::Print(bShouldTraceForItems,1);
	if (TraceHitItem != nullptr)
	{
	//	Logs::Print(TraceHitItem->GetActorLabel(), 2);
	}
	if (TraceHitItemLastFrame != nullptr)
	{
	//	Logs::Print(TraceHitItemLastFrame->GetActorLabel(),3);
	}

}

void APlayerCharacter::TraceForItems()
{
	FHitResult ItemTraceResult;
	FVector HitLocation;
	if (bShouldTraceForItems)
	{
		if (TraceUnderCrosshairs(ItemTraceResult, HitLocation))
		{
			if (ItemTraceResult.bBlockingHit)
			{
				TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
				const auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
				if (TraceHitWeapon)
				{
					if (HighlightSlot == -1)
					{
						HighlightInventorySlot();
					}
				}
				else
				{
					if (HighlightSlot != -1)
					{
						UnHighlightInventorySlot();
					}
				}

				if (TraceHitItem && TraceHitItem ==TraceHitItemLastFrame && TraceHitItem->GetPickUpWidget())
				{
					// 범위 밖에 있는걸 Trace 했을때 무시해야함

					TraceHitItem->GetPickUpWidget()->SetVisibility(true);
					TraceHitItem->EnableCustomDepth();
				}
				// 이전 프레임에 
				if (TraceHitItemLastFrame)
				{
					// 다른 결과가 들어오면
					if (TraceHitItem != TraceHitItemLastFrame)
					{
						//UI 끔	
						TraceHitItemLastFrame->GetPickUpWidget()->SetVisibility(false);
						TraceHitItemLastFrame->DisableCustomDepth();
					}
				}
				else
				{
					TraceHitItemLastFrame = TraceHitItem;
				}
			}
			else
			{
				TraceHitItem = nullptr;
			}
		}
	}
	else if (TraceHitItemLastFrame)
	{
		TraceHitItemLastFrame->GetPickUpWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();
	}
}

void APlayerCharacter::SetLookRates()
{
	if (bAiming)
	{
		baseTurnRate = AimTurnRate;
		baseLookUpRate = AimLookUpRate;
	}
	else
	{
		baseTurnRate = IdleTurnRate;
		baseLookUpRate = IdleLookUpRate;
	}
}

void APlayerCharacter::InterpFOV(float DeltaTime)
{
	bAiming ? CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, AimCameraFOV, DeltaTime, ZoomInterpSpeed) :
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraFOV, DeltaTime, ZoomInterpSpeed);
	GetCameraBoom()->SetFieldOfView(CameraCurrentFOV);
}

void APlayerCharacter::GetPickupItem(AItem* Item)
{
	auto Weapon = Cast<AWeapon>(Item);
	auto Ammo = Cast<AAmmo>(Item);
	auto Portion = Cast<AHealthItem>(Item);

	if (Weapon)
	{
		if (Inventory.Num() < INVENTORY_CAPACITY)
		{
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			Weapon->SetItemState(EItemState::EIS_PickedUp);
			Weapon->PlayPickupSound();
		}
		else // Inventory is full 
		{
			SwapWeapon(Weapon);
		}
	}

	if (Ammo)
	{
		PickupAmmo(Ammo);
	}

	if (Portion)
	{
		PickupHealth(Portion);
	}
}

void APlayerCharacter::PickupAmmo(AAmmo* Ammo)
{
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		int32 AmmoCount = { AmmoMap[Ammo->GetAmmoType()] };
		AmmoCount += Ammo->GetAmmoCount();
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
		Ammo->PlayPickupSound();
	}

	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		if (EquippedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}

	Ammo->Destroy();
}

void APlayerCharacter::PickupHealth(AHealthItem* _Health)
{
	if (Health < MaxHealth)
	{
		Health += _Health->GetresotrHealth();
		_Health->PlayPickupSound();
		_Health->Destroy();
	}
	
	if (Health >= MaxHealth)
	{
		Health = MaxHealth;
	}

}

int32 APlayerCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			return i;
		}
	}
	if (Inventory.Num() < INVENTORY_CAPACITY)
	{
		return Inventory.Num();
	}

	return -1;
}

void APlayerCharacter::Move(const struct FInputActionValue& _value)
{
	const FVector2D MovementValue = _value.Get<FVector2D>();

	if (Controller)
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector FowardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(FowardDirection, MovementValue.Y);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MovementValue.X);
	}

}

void APlayerCharacter::Look(const FInputActionValue& _value)
{
	if (Controller)
	{
		float lookUpScaleFactor{};
		float TurnScaleFactor{};

		TurnScaleFactor = baseTurnRate;
		lookUpScaleFactor = baseLookUpRate;

		if (bAiming)
		{
			TurnScaleFactor = MouseAimTurnRate;
			lookUpScaleFactor = MouseAimLookUpRate;
		}
		else
		{
			TurnScaleFactor = MouseIdleTurnRate;
			lookUpScaleFactor = MouseIdleLookUpRate;
		}

		const FVector2D LookAxisVector = _value.Get<FVector2D>();

		AddControllerYawInput(LookAxisVector.X * TurnScaleFactor * GetWorld()->GetDeltaSeconds());
		AddControllerPitchInput(LookAxisVector.Y *lookUpScaleFactor * GetWorld()->GetDeltaSeconds());
	}
}

void APlayerCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else
	{
		ACharacter::Jump();
	}
}

void APlayerCharacter::AimingPressed()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed-150;
}

void APlayerCharacter::AimingReleased()
{
	bAiming = false;
	if (!bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void APlayerCharacter::ReloadPressed()
{
	ReloadWeapon();
}


void APlayerCharacter::Fire()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unocuupied) return;

	if (WeaponHasAmmo())
	{
		PlayFireSound();
		SendBullet();
		PlayFireAnimation();
		StartCrosshairBulletFire();
		EquippedWeapon->DecrementAmmo();
		StartFireTimer();
	}

}

void APlayerCharacter::SendBullet()
{
	const USkeletalMeshSocket* HandSocket = EquippedWeapon->GetItemMesh()->GetSocketByName(FName("Muzzle"));
	if (HandSocket)
	{
		const FTransform SocketTransform = HandSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());
		SpawnMuzzleEffect(SocketTransform);

		FHitResult BeamHitResult;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResult);

		if (bBeamEnd)
		{
			if (BeamHitResult.GetActor())
			{
				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.GetActor());
				if (BulletHitInterface)
				{
					BulletHitInterface->BulletHit_Implementation(BeamHitResult, this, GetController());
				}
				ABaseEnemy* HitEnemy = Cast<ABaseEnemy>(BeamHitResult.GetActor());
				if (HitEnemy)
				{
					int32 Damage;
					
					if (BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone())
					{
						Damage = EquippedWeapon->GetHeadShotDamage();
						UGameplayStatics::ApplyDamage(BeamHitResult.GetActor(), EquippedWeapon->GetHeadShotDamage(), GetController(), this, UDamageType::StaticClass());

						HitEnemy->ShowDamageText(Damage, BeamHitResult.Location, true);
					}
					else
					{
						Damage = EquippedWeapon->GetDamage();
						UGameplayStatics::ApplyDamage(BeamHitResult.GetActor(), EquippedWeapon->GetDamage(), GetController(), this, UDamageType::StaticClass());
						HitEnemy->ShowDamageText(Damage, BeamHitResult.Location, false);
					}
					SpawnbulletTraceEffect(BeamHitResult.Location);
				}
			}
			SpawnbulletTraceEffect(BeamHitResult.Location);
			SpawnsmokeBeamEffect(SocketTransform, BeamHitResult.Location);
		}
	}
}

void APlayerCharacter::FirePressed()
{
	bFireButtonPressed = true;
	Fire();
}

void APlayerCharacter::FireReleased()
{
	bFireButtonPressed = false;
}

void APlayerCharacter::PlayFireAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireMontage)
	{
		AnimInstance->Montage_Play(FireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool APlayerCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
{
	FVector OutBeamLocation;
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult,OutBeamLocation);

	if (bCrosshairHit)
	{
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else
	{

	}
	
	const FVector WeaponTraceStart = MuzzleSocketLocation;
	const FVector StartToEnd = { OutBeamLocation - MuzzleSocketLocation };
	const FVector WeaponTraceEnd = MuzzleSocketLocation + StartToEnd * 1.25f;

	GetWorld()->LineTraceSingleByChannel(OutHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);
	if (!OutHitResult.bBlockingHit)
	{
		OutHitResult.Location = OutBeamLocation;
		return false;
	}
	return true;
}



void APlayerCharacter::Stun()
{
	if (Health <= 0.0f) return;
	CombatState = ECombatState::ECS_Stunned;

	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	if (Anim && HitSound)
	{
		Anim->Montage_Play(GetHitMontage);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
	}
}

float APlayerCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void APlayerCharacter::SpawnsmokeBeamEffect(const FTransform& SocketTransform, const FVector& BeamEndPoint)
{
	if (smokeBeam)
	{
	//	UNiagaraComponent* spawn = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SmokeBeamEffect, BeamEndPoint);
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), smokeBeam, SocketTransform);
		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
		}
	}
}

void APlayerCharacter::SpawnbulletTraceEffect(const FVector& BeamEndPoint)
{
	if (bulletTrace)
	{
		//UNiagaraComponent* spawn = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTarceEffect, BeamEndPoint);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletTrace, BeamEndPoint);
	}
}

void APlayerCharacter::SpawnMuzzleEffect(const FTransform& SocketTransform)
{
	if (EquippedWeapon->GetMuzzleFlash())
	{
	//	UNiagaraComponent* spawn = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EquippedWeapon->GetMuzzleEffects(), SocketTransform.GetLocation());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->GetMuzzleFlash(), SocketTransform);
	}
}

void APlayerCharacter::PlayFireSound()
{
	if (EquippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetFireSound());
	}
}


void APlayerCharacter::InterectPressed()
{
	if (CombatState != ECombatState::ECS_Unocuupied) return;
	if (TraceHitItem)
	{
		GetPickupItem(TraceHitItem);
		TraceHitItem = nullptr;
	}
}

void APlayerCharacter::InterectReleased()
{
}


void APlayerCharacter::CrouchPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}
	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void APlayerCharacter::FKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 0) return;
	ExchageInventoryItems(EquippedWeapon->GetSlotIndex(), 0);
}

void APlayerCharacter::OneKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 1) return;
	ExchageInventoryItems(EquippedWeapon->GetSlotIndex(), 1);
}

void APlayerCharacter::TwoKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 2) return;
	ExchageInventoryItems(EquippedWeapon->GetSlotIndex(), 2);
}

void APlayerCharacter::ThreeKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 3) return;
	ExchageInventoryItems(EquippedWeapon->GetSlotIndex(), 3);
}

void APlayerCharacter::FourKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 4) return;
	ExchageInventoryItems(EquippedWeapon->GetSlotIndex(), 4);
}

void APlayerCharacter::FiveKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 5) return;
	ExchageInventoryItems(EquippedWeapon->GetSlotIndex(), 5);
}


void APlayerCharacter::ExchageInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	if (CurrentItemIndex != NewItemIndex && NewItemIndex < Inventory.Num() && CombatState == ECombatState::ECS_Unocuupied || CombatState == ECombatState::ECS_Equipping)
	{
		auto OldEquippedWeapon = EquippedWeapon;
		auto NewWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);
		EquipWeapon(NewWeapon);

		OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
		NewWeapon->SetItemState(EItemState::EIS_Equipped);

		CombatState = ECombatState::ECS_Equipping;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && EquipMontage)
		{
			AnimInstance->Montage_Play(EquipMontage);
			AnimInstance->Montage_JumpToSection(FName("Equip"));
			NewWeapon->PlaySwapSound();
		}
	}
}
