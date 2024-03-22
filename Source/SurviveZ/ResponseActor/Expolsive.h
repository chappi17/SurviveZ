
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletHitInterface.h"
#include "Expolsive.generated.h"

UCLASS()
class SURVIVEZ_API AExpolsive : public AActor, public IBulletHitInterface
{
	GENERATED_BODY()
	
public:	
	AExpolsive();

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"));
	class USoundCue* ImpactSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ExplosiveMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float Damage;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController) override;

};
