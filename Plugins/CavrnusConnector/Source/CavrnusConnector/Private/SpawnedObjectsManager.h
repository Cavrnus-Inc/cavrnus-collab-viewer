// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Types/CavrnusSpawnedObject.h"
#include "Types/PropertiesContainer.h"

class USpawnObjectHelpers;

/**
 * @brief Manages the lifecycle of spawned objects in the Cavrnus system.
 *
 * The SpawnedObjectsManager class is responsible for registering, tracking, and unregistering
 * spawned objects within the Cavrnus system. It maintains a map of spawned actors and provides
 * methods to manage their creation and removal.
 */
class CAVRNUSCONNECTOR_API SpawnedObjectsManager 
{
public:
	SpawnedObjectsManager();
	~SpawnedObjectsManager();

    /**
     * @brief Registers a spawned object in the Cavrnus system.
     *
     * This method creates and registers a new actor based on the provided spawned object data,
     * actor class, and world. It uses the specified spawn helpers to initialize the actor.
     *
     * @param SpawnedObject The spawned object data.
     * @param ActorClass The class of the actor to spawn.
     * @param world The world in which to spawn the actor.
     * @param spawnHelpers Helpers to initialize the spawned actor.
     * @return A pointer to the newly spawned actor.
     */
    AActor* RegisterSpawnedObject(const FCavrnusSpawnedObject& SpawnedObject, TSubclassOf<AActor> ActorClass, UWorld* world, USpawnObjectHelpers* spawnHelpers);
    
    /**
     * @brief Unregisters a spawned object from the Cavrnus system.
     *
     * This method removes the actor corresponding to the provided spawned object from the world
     * and unregisters it from the internal tracking map.
     *
     * @param SpawnedObject The spawned object data to unregister.
     * @param World The world from which to remove the actor.
     */
    void UnregisterSpawnedObject(const FCavrnusSpawnedObject& SpawnedObject, UWorld* World);

private:
    /** Map of spawned actors, keyed by a unique identifier string. */
    TMap<FPropertiesContainer, AActor*> spawnedActors;
};
