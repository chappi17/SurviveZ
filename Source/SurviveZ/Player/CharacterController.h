// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVEZ_API ACharacterController : public APlayerController
{
	GENERATED_BODY()
public:
	ACharacterController();
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "Widget", meta = (AllowPrivateAccess= "true"))
	TSubclassOf<class UUserWidget> HUDOverlayClass;


	UPROPERTY(VisibleAnywhere,Category= "Widget", meta = (AllowPrivateAccess= "true"))
	UUserWidget* HUDOverlay;
	
};
