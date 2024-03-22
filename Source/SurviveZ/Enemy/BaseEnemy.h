// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "BaseEnemy.generated.h"

UCLASS()
class SURVIVEZ_API ABaseEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	ABaseEnemy();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	void Die();
	void ResetHitReactTimer();
	void PlayHitMontage(FName Section, float playrate = 1.0f);

	UFUNCTION(BlueprintCallable)
	void PlayAttackmontage(FName Section, float playrate = 1.0f);

	UFUNCTION(BlueprintCallable)
	void StoreHitDamage(UUserWidget* Damage, FVector Location);

	UFUNCTION()
	void DestroyHitDamage(UUserWidget* Damage);

	void UpdateHitDamage();

	UFUNCTION()
	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex,bool bFromSweep,const FHitResult& sweepResult);
	
	UFUNCTION()
	void AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex);


	UFUNCTION()
	void CombatSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex,bool bFromSweep,const FHitResult& sweepResult);
	
	UFUNCTION()
	void CombatSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex);

	UFUNCTION()
	void OnLeftHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex,bool bFromSweep,const FHitResult& sweepResult);
	
	UFUNCTION()
	void OnRightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex,bool bFromSweep,const FHitResult& sweepResult);

	void DoDamage(class APlayerCharacter* _other);
	void SpawnBloodEffect(APlayerCharacter* _character,FName SocketName);

	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);

	UFUNCTION(BlueprintCallable)
	void ActivateLeftCollision();
	UFUNCTION(BlueprintCallable)
	void DeActivateLeftCollision();
	UFUNCTION(BlueprintCallable)
	void ActivateRightCollision();
	UFUNCTION(BlueprintCallable)
	void DeActivateRightCollision();

	void StunCharacter(APlayerCharacter* player);

	void ResetCanAttack();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	UFUNCTION(BlueprintCallable)
	void DestroyEnemy();

	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Combat", meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"));
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Combat", meta = (AllowPrivateAccess = "true"));
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Combat", meta = (AllowPrivateAccess = "true"));
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"));
	FString HeadBone;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"));
	float HealthBarDisplayTime;

	FTimerHandle HealthBarTimer;
	FTimerHandle HitReactTimer;
	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Combat", meta = (AllowPrivateAccess = "true"));
	float DeathTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Combat", meta = (AllowPrivateAccess = "true"));
	UAnimMontage* HitMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Combat", meta = (AllowPrivateAccess = "true"));
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Combat", meta = (AllowPrivateAccess = "true"));
	float HitReactTimeMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Combat", meta = (AllowPrivateAccess = "true"));
	float HitReactTimeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Combat", meta = (AllowPrivateAccess = "true"));
	float DamageNumberDestroyTime;
	
	bool bCanHitReact;
	bool bDying;


	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Combat", meta = (AllowPrivateAccess = "true"));
	TMap<UUserWidget*, FVector> DamageTexts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"));
	FVector PatolPoint;
	
	TObjectPtr<class AEnemyController> EnemyController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	TObjectPtr<class USphereComponent> AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	bool bStunned;

	// 0: not stun 1 : stun
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	float StunChance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	bool bAttackRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	TObjectPtr<USphereComponent> CombatSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	class UBoxComponent* LeftHandCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	UBoxComponent* RightHandCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	FName LeftSocket;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	FName RightSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	bool bCanAttack;

	FTimerHandle AttackWaitTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "AI", meta = (AllowPrivateAccess = "true"));
	float AttackWaitTimerate;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;




	FORCEINLINE FString GetHeadBone() const { return HeadBone; }
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDamageText(int32 Damage, FVector HitLocation, bool bHeadShot);
};
