// Fill out your copyright notice in the Description page of Project Settings.

#include "Global.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Player/PlayerCharacter.h"
#include "Weapons/HealthItem.h"

AHealthItem::AHealthItem()
	:restoreHealth(30)
{
	Helpers::CreateComponent<UStaticMeshComponent>(this, &HealthMesh, FName(TEXT("HealthMesh")));
	SetRootComponent(HealthMesh);

	GetCollisionBox()->SetupAttachment(GetRootComponent());
	GetPickUpWidget()->SetupAttachment(GetRootComponent());
	GetAreaSphere()->SetupAttachment(GetRootComponent());
}

void AHealthItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OverComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
