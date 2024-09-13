// Copyright(c) Cavrnus. All rights reserved.
#include "SpawnedObjectsManager.h"
#include "CavrnusConnectorModule.h"
#include "CavrnusSpatialConnector.h"
#include "CavrnusSpatialConnectorSubSystem.h"
#include "CavrnusFunctionLibrary.h"
#include "SpawnObjectHelpers.h"
#include "FlagComponents/CavrnusSpawnedObjectFlag.h"
#include "CavrnusPropertiesContainer.h"
#include "CoreMinimal.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
SpawnedObjectsManager::SpawnedObjectsManager()
{
}

SpawnedObjectsManager::~SpawnedObjectsManager()
{
}

AActor* SpawnedObjectsManager::RegisterSpawnedObject(const FCavrnusSpawnedObject& SpawnedObject, TSubclassOf<AActor> ActorClass, UWorld* world, USpawnObjectHelpers* spawnHelpers)
{
	auto actor = spawnHelpers->SpawnObjectAndSetup(world, ActorClass, SpawnedObject);

	spawnedActors.Add(FPropertiesContainer(SpawnedObject.PropertiesContainerName), actor);

	return actor;
}

void SpawnedObjectsManager::UnregisterSpawnedObject(const FCavrnusSpawnedObject& SpawnedObject, UWorld* World)
{
	if (!spawnedActors.Contains(FPropertiesContainer(SpawnedObject.PropertiesContainerName)))
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("Failed to destroy actor, could not find spawned object with Container Name %s"), *SpawnedObject.PropertiesContainerName);
		return;
	}

	spawnedActors[FPropertiesContainer(SpawnedObject.PropertiesContainerName)]->Destroy();
}