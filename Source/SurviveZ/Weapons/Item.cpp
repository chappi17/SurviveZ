#include "Weapons/Item.h"
#include "Global.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "Player/PlayerCharacter.h"


AItem::AItem()
	:waveHeight(10.f)
	, waveSpeed(5.f)
	, currentTime(0.0f)
	, ItemName(FString("Default"))
	, ItemCount(0)
	, ItemRarity(EItemRarity::EIR_Basic)
	, bSinEffect(true)
	, ItemState(EItemState::EIS_PickUp)
	, bShouldPlayEquipSound(true)
	, EquipSoundResetTime(0.2f)
	, SlotIndex(0)
{
	PrimaryActorTick.bCanEverTick = true;
	
	Helpers::CreateComponent<USkeletalMeshComponent>(this, &ItemMesh, "ItemMesh");
	Helpers::CreateComponent<UStaticMeshComponent>(this, &StaticMesh, "StaticMesh",ItemMesh);
	Helpers::CreateComponent<UBoxComponent>(this, &CollisionBox, "CollisionBox",ItemMesh);
	Helpers::CreateComponent<UWidgetComponent>(this, &PickUpWidget, "PickUpInfomation",ItemMesh);
	Helpers::CreateComponent<USphereComponent>(this, &AreaSphere, "AreaSphere",ItemMesh);

	//SetRootComponent(ItemMesh);
	//CollisionBox->SetupAttachment(ItemMesh);
	//PickUpWidget->SetupAttachment(ItemMesh);
	//AreaSphere->SetupAttachment(ItemMesh);
	//StaticMesh->SetupAttachment(ItemMesh);

	PickUpWidget->SetVisibility(false);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	SetChangeRarity();
	SetItemProperties(ItemState);
	InitializeCustomDepth();
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OverComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			Player->IncrementOverlappedItemCount();
		}
	}

}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtehrBodyIndex)
{
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			Player->DecrementOverlappedItemCount();
			Player->SetTraceHitLastFrame(nullptr);
			PickUpWidget->SetVisibility(false);
			DisableCustomDepth();
			Player->UnHighlightInventorySlot();
		}
	}
}

void AItem::SetChangeRarity()
{
	for (int32 i = 0; i <= 6; i++)
	{
		ActiveColors.Add(false);
	}

	switch (ItemRarity)
	{
	case EItemRarity::EIR_Basic:
		ActiveColors[1] = true;
		break;
	case EItemRarity::EIR_Common:
		ActiveColors[1] = true;
		ActiveColors[2] = true;
		break;
	case EItemRarity::EIR_UnCommon:
		ActiveColors[1] = true;
		ActiveColors[2] = true;
		ActiveColors[3] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveColors[1] = true;
		ActiveColors[2] = true;
		ActiveColors[3] = true;
		ActiveColors[4] = true;
		break;
	case EItemRarity::EIR_Legendary:
		ActiveColors[1] = true;
		ActiveColors[2] = true;
		ActiveColors[3] = true;
		ActiveColors[4] = true;
		ActiveColors[5] = true;
		break;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_PickUp:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EItemState::EIS_EquipInterping:
		break;
	case EItemState::EIS_PickedUp:
		PickUpWidget->SetVisibility(false);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Equipped:
		PickUpWidget->SetVisibility(false);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState::EIR_MAX:
		break;
	default:
		break;
	}
}

void AItem::PlayPickupSound()
{	
	if (ShouldPlayEquipSound())
	{
		StartEquipSoundTimer();
		if (PickupSound)
		{
			UGameplayStatics::PlaySound2D(this, PickupSound);
		}
	}
}

void AItem::PlaySwapSound()
{
	if (SwapSound)
	{
		UGameplayStatics::PlaySound2D(this, SwapSound);
	}
}

void AItem::EnableCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(true);
}

void AItem::DisableCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(false);
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}

void AItem::OnConstruction(const FTransform& Transform)
{
	FString RarityTablePath(TEXT("'/Game/Assets/DataTable/DT_RarityItem.DT_RarityItem'"));	
	UDataTable* RarityTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *RarityTablePath));

	if (RarityTableObject)
	{
		FItemRarityTable* RarityRow = nullptr;
		switch (ItemRarity)
		{
			case EItemRarity::EIR_Basic:
				RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Basic"), TEXT(""));
				break;
			case EItemRarity::EIR_Common:
				RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Common"), TEXT(""));
				break;
			case EItemRarity::EIR_UnCommon:
				RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("UnCommon"), TEXT(""));
				break;
			case EItemRarity::EIR_Rare:
				RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Rare"), TEXT(""));
				break;
			case EItemRarity::EIR_Legendary:
				RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Legendary"), TEXT(""));
				break;
		}

		if (RarityRow)
		{
			IconBackground = RarityRow->IconBackGroundData;
			MultyplyDamagebyRarity = RarityRow->MultyplyDamagebyRarity;
			if (GetItemMesh())
			{
				GetItemMesh()->SetCustomDepthStencilValue(RarityRow->CustomDepthStencil);
			}
		}
	}
}

void AItem::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

void AItem::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AItem::ResetEquipSoundTimer, EquipSoundResetTime);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SinEffect(DeltaTime);
}

void AItem::SinEffect(float DeltaTime)
{
	if (bSinEffect)
	{
		//Sine wave 이용해서 Item 떠다니는 효과 만들기
		currentTime += DeltaTime;
		FVector NewLocation = GetActorLocation();
		NewLocation.Z += waveHeight * sinf(waveSpeed * currentTime) * DeltaTime;
		SetActorLocation(NewLocation);
	}
}

void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

