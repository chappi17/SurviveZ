#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Global.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Character.h"
#include "ResponseActor/Expolsive.h"

AExpolsive::AExpolsive()
	:Damage(100.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	SetRootComponent(ExplosiveMesh);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
}

void AExpolsive::BeginPlay()
{
	Super::BeginPlay();
	
}

void AExpolsive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExpolsive::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for (auto Actor : OverlappingActors)
	{
		Logs::Print(Actor->GetName());
		UGameplayStatics::ApplyDamage(Actor, Damage, ShooterController, Shooter, UDamageType::StaticClass());
	}


	Destroy();
}

