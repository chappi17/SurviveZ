#include "Enemy/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyController.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Global.h"
#include "Player/PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerCharacter.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
	:
	Health(100.f)
	, MaxHealth(100.f)
	, HealthBarDisplayTime(5.0f)
	, bCanHitReact(true)
	, HitReactTimeMin(0.5f)
	, HitReactTimeMax(.75f)
	, DamageNumberDestroyTime(1.5f)
	, bStunned(false)
	, StunChance(0.f)
	, bAttackRange(false)
	, BaseDamage(20.0f)
	, LeftSocket(TEXT("LeftHandSocket"))
	, RightSocket(TEXT("RightHandSocket"))
	, bCanAttack(true)
	, AttackWaitTimerate(1.5f)
	, bDying(false)
	, DeathTime(3.0f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Helpers::CreateComponent<USphereComponent>(this, &AgroSphere, "AgroSphere");
	AgroSphere->SetupAttachment(GetRootComponent());
	Helpers::CreateComponent<USphereComponent>(this, &CombatSphere, "CombatSphere");
	CombatSphere->SetupAttachment(GetRootComponent());

	LeftHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Left Weapon Box"));
	LeftHandCollision->SetupAttachment(GetMesh(), FName("LeftHand"));
	RightHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Box"));
	RightHandCollision->SetupAttachment(GetMesh(), FName("RightHand"));
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::AgroSphereOverlap);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::AgroSphereEndOverlap);
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::CombatSphereOverlap);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::CombatSphereEndOverlap);
	LeftHandCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnLeftHandOverlap);
	RightHandCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnRightHandOverlap);

	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	EnemyController = Cast<AEnemyController>(GetController());
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
	}

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatolPoint);
	//DrawDebugSphere(GetWorld(), WorldPatrolPoint, 25.f, 12, FColor::Red, true);
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyController->RunBehaviorTree(BehaviorTree);
	}

}

void ABaseEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &ABaseEnemy::HideHealthBar, HealthBarDisplayTime);
}

void ABaseEnemy::Die()
{
	if (bDying) return;
	bDying = true;

	HideHealthBar();
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	if (Anim)
	{
		Anim->Montage_Play(HitMontage);
		//TArray<FName> a;
		//a.Add("DeathA");
		//a.Add("DeathB");
	//	int32 sectionInt = FMath::FRandRange(0.0f, a.Num() - 1);
	//Anim->Montage_JumpToSection(FName(a[sectionInt]));

		Anim->Montage_JumpToSection(FName("DeathB"));
	}
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
		EnemyController->StopMovement();
	}
}

void ABaseEnemy::ResetHitReactTimer()
{
	bCanHitReact = true;
}


void ABaseEnemy::PlayHitMontage(FName Section, float playrate)
{
	if (bCanHitReact)
	{
		UAnimInstance* Anim = GetMesh()->GetAnimInstance();
		if (Anim)
		{
			Anim->Montage_Play(HitMontage, playrate);
			Anim->Montage_JumpToSection(Section, HitMontage);
		}
		bCanHitReact = false;
		const float HitReactime = FMath::FRandRange(HitReactTimeMin, HitReactTimeMax);
		GetWorldTimerManager().SetTimer(HitReactTimer, this, &ABaseEnemy::ResetHitReactTimer, HitReactime);
	}
}

void ABaseEnemy::PlayAttackmontage(FName Section, float playrate)
{
	if (bCanHitReact)
	{
		UAnimInstance* Anim = GetMesh()->GetAnimInstance();
		if (Anim && AttackMontage)
		{
			Anim->Montage_Play(AttackMontage, playrate);
			Anim->Montage_JumpToSection(Section, AttackMontage);
		}
	}
	bCanAttack = false;
	GetWorldTimerManager().SetTimer(AttackWaitTimer, this, &ABaseEnemy::ResetCanAttack, AttackWaitTimerate);

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), false);
	}
}

void ABaseEnemy::StoreHitDamage(UUserWidget* Damage, FVector Location)
{
	DamageTexts.Add(Damage, Location);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitDamage"), Damage);
	GetWorld()->GetTimerManager().SetTimer(HitNumberTimer, HitNumberDelegate, DamageNumberDestroyTime, false);
}

void ABaseEnemy::DestroyHitDamage(UUserWidget* Damage)
{
	DamageTexts.Remove(Damage);
	Damage->RemoveFromParent();
}

void ABaseEnemy::UpdateHitDamage()
{
	for (auto& HitPair : DamageTexts)
	{
		UUserWidget* Damage = HitPair.Key;
		const FVector Location = HitPair.Value;
		FVector2D ScreenPostion;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenPostion);
		Damage->SetPositionInViewport(ScreenPostion);
	}
}

void ABaseEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex, bool bFromSweep, const FHitResult& Sweepresult)
{
	if (OtherActor == nullptr) return;
	auto Character = Cast<APlayerCharacter>(OtherActor);
	if (Character)
	{
		if (EnemyController)
		{
			if (EnemyController->GetBlackboardComponent())
			{
				EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
			}
		}
	}

}

void ABaseEnemy::AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex)
{
	if (OtherActor == nullptr) return;
	auto Character = Cast<APlayerCharacter>(OtherActor);
	if (Character)
	{
		if (EnemyController)
		{
			if (EnemyController->GetBlackboardComponent())
			{
				EnemyController->GetBlackboardComponent()->ClearValue(TEXT("Target"));
			}
		}
	}
}

void ABaseEnemy::CombatSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (OtherActor == nullptr) return;
	auto Character = Cast<APlayerCharacter>(OtherActor);
	if (Character)
	{
		bAttackRange = true;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}
	}
}

void ABaseEnemy::CombatSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex)
{
	if (OtherActor == nullptr) return;
	auto Character = Cast<APlayerCharacter>(OtherActor);
	if (Character)
	{
		bAttackRange = false;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}

void ABaseEnemy::OnLeftHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	auto Character = Cast<APlayerCharacter>(OtherActor);
	if (Character)
	{
		DoDamage(Character);
		SpawnBloodEffect(Character, LeftSocket);
		StunCharacter(Character);
	}
}

void ABaseEnemy::OnRightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	auto Character = Cast<APlayerCharacter>(OtherActor);
	if (Character)
	{
		DoDamage(Character);
		SpawnBloodEffect(Character, RightSocket);
		StunCharacter(Character);
		DeActivateRightCollision();
	}
}

void ABaseEnemy::SpawnBloodEffect(APlayerCharacter* _Character, FName SocketName)
{
	const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName(SocketName);
	if (TipSocket)
	{
		const FTransform SocketTransform = TipSocket->GetSocketTransform(GetMesh());
		if (_Character->GetBloodEffect())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _Character->GetBloodEffect(), SocketTransform);
		}
	}
}

void ABaseEnemy::SetStunned(bool Stunned)
{
	bStunned = Stunned;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), Stunned);
	}
}

void ABaseEnemy::ActivateLeftCollision()
{
	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABaseEnemy::DeActivateLeftCollision()
{
	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseEnemy::ActivateRightCollision()
{
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABaseEnemy::DeActivateRightCollision()
{
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseEnemy::StunCharacter(APlayerCharacter* player)
{
	if (player)
	{
		player->Stun();
		//const float Stun = { FMath::FRandRange(0.f,1.0f) };
		//if (Stun <= player->GetStunChance())
		//{
		//	player->Stun();
		//}
	}
}

void ABaseEnemy::ResetCanAttack()
{
	bCanAttack = true;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
	}
}

void ABaseEnemy::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	GetWorldTimerManager().SetTimer(DeathTimer, this, &ABaseEnemy::DestroyEnemy, DeathTime);
}

void ABaseEnemy::DestroyEnemy()
{
	Destroy();
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHitDamage();
}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseEnemy::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}


}

float ABaseEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsObject(FName("Target"), DamageCauser);
	}
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.0f;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}

	if (bDying) return DamageAmount;

	ShowHealthBar();
	const float Stunned = FMath::FRandRange(0.0f, 1.0f);
	if (Stunned <= StunChance)
	{
		PlayHitMontage(FName("HitReact"));
		SetStunned(true);
	}
	return DamageAmount;
}

void ABaseEnemy::DoDamage(APlayerCharacter* _other)
{
	if (_other == nullptr) return;
	UGameplayStatics::ApplyDamage(_other, BaseDamage, EnemyController, this, UDamageType::StaticClass());
}

