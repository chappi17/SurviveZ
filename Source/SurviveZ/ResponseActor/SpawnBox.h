// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnBox.generated.h"

UCLASS()
class SURVIVEZ_API ASpawnBox : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnBox();

protected:
	virtual void BeginPlay() override;



public:	
	virtual void Tick(float DeltaTime) override;

};
