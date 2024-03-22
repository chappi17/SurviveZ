#pragma once

#include "CoreMinimal.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

class SURVIVEZ_API Helpers
{
public:
	template<typename T>
	static void GetAsset(T** OutObject, FString Inpath)
	{
		ConstructorHelpers::FObjectFinder<T> asset(*InPath);
		verityf(asset.Succeeded(), L"asset.Succeeded()");

		*OutObject = asset.Object;
	}

	template<typename T>
	static void CreateComponent(AActor* InActor, T** InComponent, FName InName, USceneComponent* InParent = NULL)
	{
		*InComponent = InActor->CreateDefaultSubobject<T>(InName);

		if (!!InParent)
		{
			(*InComponent)->SetupAttachment(InParent);

			return;
		}

		InActor->SetRootComponent((*InComponent));
	}

	template<typename T>
	static void CreateComponent(AActor* InActor, TObjectPtr<T>* InComponent, FName InName, USceneComponent* InParent = NULL)
	{
		*InComponent = InActor->CreateDefaultSubobject<T>(InName);
	}

	template<typename T>
	static T* GetComponent(AActor* InActor)
	{
		return Cast<T>(InActor->GetComponentByClass(T::StaticClass()));
	}

	template<typename T>
   static void CreateActorComponent(AActor* InActor, T** InComponent, FName InName)
   {
       *InComponent = InActor->CreateDefaultSubobject<T>(InName);
   }

    template<typename T>
   static void FindActors(class UWorld* InWorld, TArray<T*>& OutActors)
   {
       OutActors.Empty();

       TArray<AActor*> actors;
       UGameplayStatics::GetAllActorsOfClass(InWorld, T::StaticClass(), actors);

       for (AActor* actor : actors)
           OutActors.Add(Cast<T>(actor));
   }

     template<typename T>
   static void GetClass(TSubclassOf<T>* OutClass, FString InPath)
   {
       ConstructorHelpers::FClassFinder<T> asset(*InPath);
       verifyf(asset.Succeeded(), L"asset.Succeeded()");

       *OutClass = asset.Class;
   }
};
