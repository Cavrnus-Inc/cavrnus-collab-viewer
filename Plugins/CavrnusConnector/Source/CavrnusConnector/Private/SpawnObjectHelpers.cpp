// Copyright(c) Cavrnus. All rights reserved.
#include "SpawnObjectHelpers.h"
#include "CavrnusConnectorModule.h"
#include "CavrnusFunctionLibrary.h"
#include "FlagComponents\CavrnusUserAvatarFlag.h"
#include "FlagComponents\CavrnusSpawnedObjectFlag.h"
#include <Engine/World.h>
#include <Kismet/GameplayStatics.h>

USpawnObjectHelpers::USpawnObjectHelpers()
{
}

AActor* USpawnObjectHelpers::SpawnUserAndSetup(UWorld* World, TSubclassOf<AActor> ActorClass, const FCavrnusUser& User)
{
	FTransform SpawnTransform = UCavrnusFunctionLibrary::GetTransformPropertyValue(User.SpaceConn, User.PropertiesContainerName, "Transform");

	AActor* SpawnedActor = World->SpawnActorDeferred<AActor>(ActorClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	userSpawns.Add(SpawnedActor, User);

	UGameplayStatics::FinishSpawningActor(SpawnedActor, SpawnTransform);

	// Complete setup for Actors that did not set up via an UCavrnusPropertiesContainer
	if (!SpawnedActor->GetComponentByClass(UCavrnusUserAvatarFlag::StaticClass()))
	{
		InitializeCavrnusActor(SpawnedActor);
	}

	return SpawnedActor;
}

AActor* USpawnObjectHelpers::SpawnObjectAndSetup(UWorld* World, TSubclassOf<AActor> ActorClass, const FCavrnusSpawnedObject& SpawnedObject)
{
	FTransform SpawnTransform = UCavrnusFunctionLibrary::GetTransformPropertyValue(SpawnedObject.SpaceConnection, SpawnedObject.PropertiesContainerName, "Transform");

	AActor* SpawnedActor = World->SpawnActorDeferred<AActor>(ActorClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	objectSpawns.Add(SpawnedActor, SpawnedObject);

	UGameplayStatics::FinishSpawningActor(SpawnedActor, SpawnTransform);

	// Complete setup for Actors that did not set up via an UCavrnusPropertiesContainer
	if (!SpawnedActor->GetComponentByClass(UCavrnusSpawnedObjectFlag::StaticClass()))
	{
		InitializeCavrnusActor(SpawnedActor);
	}

	return SpawnedActor;
}

void USpawnObjectHelpers::InitializeCavrnusActor(AActor* InActor)
{
	// Need to remove from list before calling Handlers, otherwise we'll get infinite looping
	if (userSpawns.Contains(InActor))
	{
		const FCavrnusUser CavrnusUser = userSpawns[InActor];
		userSpawns.Remove(InActor);
		HandleSpawnedUser(InActor, CavrnusUser);
	}
	else if (objectSpawns.Contains(InActor))
	{
		const FCavrnusSpawnedObject CavrnusSpawnedObject = objectSpawns[InActor];
		objectSpawns.Remove(InActor);
		HandleSpawnedObject(InActor, CavrnusSpawnedObject);
	}
}

void USpawnObjectHelpers::HandleSpawnedUser(AActor* actor, const FCavrnusUser& user)
{
	UCavrnusUserAvatarFlag* ActorComponent = Cast<UCavrnusUserAvatarFlag>(actor->AddComponentByClass(
		UCavrnusUserAvatarFlag::StaticClass(),
		true,
		FTransform::Identity,
		false));

	ActorComponent->CavrnusUser = user;
	ActorComponent->AttachToComponent(actor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	SetupObjectPropsContainers(actor, user.PropertiesContainerName);
}

void USpawnObjectHelpers::HandleSpawnedObject(AActor* actor, const FCavrnusSpawnedObject& object)
{
	UCavrnusSpawnedObjectFlag* ActorComponent = Cast<UCavrnusSpawnedObjectFlag>(actor->AddComponentByClass(
		UCavrnusSpawnedObjectFlag::StaticClass(),
		true,
		FTransform::Identity,
		false));

	ActorComponent->SpawnedObject = object;
	ActorComponent->AttachToComponent(actor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	SetupObjectPropsContainers(actor, object.PropertiesContainerName);
}

AActor* USpawnObjectHelpers::SetupObjectPropsContainers(AActor* spawnedActor, const FString& propsContainerName)
{
	if (spawnedActor == nullptr)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("Failed to successfully spawn actor"));
		return nullptr;
	}

	TArray<USceneComponent*> PropertiesContainers;
	USceneComponent* ActorRoot = spawnedActor->GetRootComponent();

	if (ActorRoot == nullptr)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("Failed to successfully detect spawned actor root"));
		return nullptr;
	}

	TArray<USceneComponent*> RootChildren;
	ActorRoot->GetChildrenComponents(false, RootChildren);
	PropertiesContainers = RootChildren.FilterByPredicate([](USceneComponent* SceneComponent) {
		return SceneComponent->IsA<UCavrnusPropertiesContainer>();
	});

	if (PropertiesContainers.IsEmpty())
	{
		// Add a properties container to the root if none found
		UCavrnusPropertiesContainer* RootContainer = Cast<UCavrnusPropertiesContainer>(spawnedActor->AddComponentByClass(
			UCavrnusPropertiesContainer::StaticClass(),
			true,
			FTransform::Identity,
			false));

		if (RootContainer)
		{
			RootContainer->AttachToComponent(ActorRoot, FAttachmentTransformRules::KeepRelativeTransform);
		}

		ActorRoot->GetChildrenComponents(false, RootChildren);
		PropertiesContainers = RootChildren.FilterByPredicate([](USceneComponent* SceneComponent) {
			return SceneComponent->IsA<UCavrnusPropertiesContainer>();
		});
	}

	checkf(PropertiesContainers.Num() == 1, TEXT("Expected a single properties container component on the root of the actor"));
	for (USceneComponent* Component : PropertiesContainers)
	{
		if (UCavrnusPropertiesContainer* PropertiesContainer = Cast<UCavrnusPropertiesContainer>(Component))
		{
			PropertiesContainer->ContainerName = propsContainerName;
		}
	}

	USpawnObjectHelpers::ResetLiveHierarchyRootName(spawnedActor, propsContainerName);

	return spawnedActor;
}

void USpawnObjectHelpers::ResetLiveHierarchyRootName(AActor* Actor, const FString& NewRootName)
{
	//What is the ContainerName of the top-level props container?  That is what we'll be replacing.
	TArray<UCavrnusPropertiesContainer*> RootComponent;
	Actor->GetComponents(RootComponent);
	//If nothing on the root, do nothing
	if (RootComponent.Num() == 0)
		return;
	FString rootString = RootComponent[0]->ContainerName;

	TArray<UCavrnusPropertiesContainer*> PropertiesContainers;
	Actor->GetComponents(PropertiesContainers, true);
	for (UCavrnusPropertiesContainer* PropertiesContainer : PropertiesContainers)
	{
		ReplacePlaceholderInPropertiesContainer(PropertiesContainer, rootString, NewRootName);
	}
}

void USpawnObjectHelpers::ReplacePlaceholderInPropertiesContainer(UCavrnusPropertiesContainer* PropertiesContainer, const FString& Placeholder, const FString& ReplacementString)
{
	FString ContainerName = PropertiesContainer->ContainerName;
	if (ContainerName.IsEmpty())
	{
		PropertiesContainer->ContainerName = ReplacementString;
	}
	else if (ContainerName.StartsWith(Placeholder))
	{
		ContainerName.RemoveFromStart(Placeholder);
		ContainerName.InsertAt(0, ReplacementString);
		PropertiesContainer->ContainerName = ContainerName;
	}
}
