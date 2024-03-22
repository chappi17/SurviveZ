#include "Global.h"
#include "Weapons/Ammo.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Player/PlayerCharacter.h"

AAmmo::AAmmo()
	:AmmoCount(10)
{
	Helpers::CreateComponent<UStaticMeshComponent>(this, &AmmoMesh, FName(TEXT("AmmoMesh")));
	SetRootComponent(AmmoMesh);

	GetCollisionBox()->SetupAttachment(GetRootComponent());
	GetPickUpWidget()->SetupAttachment(GetRootComponent());
	GetAreaSphere()->SetupAttachment(GetRootComponent());
}

void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();

	SetItemType(EItemType::EIT_Ammo);
}

void AAmmo::SetItemProperties(EItemState State)
{
	Super::SetItemProperties(State);

	switch (State)
	{
	case EItemState::EIS_PickUp:
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_EquipInterping:
		break;
	case EItemState::EIS_PickedUp:
		break;
	case EItemState::EIS_Equipped:
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AmmoMesh->SetSimulatePhysics(true);
		AmmoMesh->SetEnableGravity(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		break;
	case EItemState::EIR_MAX:
		break;
	default:
		break;
	}
}

void AAmmo::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OverComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OverComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			Player->GetPickupItem(this);
		}
	}
}

void AAmmo::EnableCustomDepth()
{
	AmmoMesh->SetRenderCustomDepth(true);
}

void AAmmo::DisableCustomDepth()
{
	AmmoMesh->SetRenderCustomDepth(false);
}
