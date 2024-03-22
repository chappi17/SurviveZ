#include "Weapons/WeaponComponent.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

