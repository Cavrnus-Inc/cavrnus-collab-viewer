// Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CavrnusFunctionLibrary.h"
#include <Components/SceneComponent.h>
#include <Materials/MaterialInstanceDynamic.h>

#include "CavrnusAvatarManager.generated.h"

class UGameInstance;
class USpawnObjectHelpers;
UCLASS()
class CAVRNUSCONNECTOR_API UCavrnusAvatarManager : public UObject
{
	GENERATED_BODY()

public:
    static UCavrnusAvatarManager* GetAvatarManager();

    static void Kill();

    /**
     * @brief Registers a new user avatar as a spawned object in the Cavrnus system.
     *
     * This method creates and registers a new actor based on the provided spawned object representing an avatar,
     * actor class, and world. It uses the specified spawn helpers to initialize the actor.
     *
     * @param SpawnedObject The spawned user avatar.
     * @param ActorClass The class of the actor to spawn.
     * @param world The world in which to spawn the avatar.
     * @param spawnHelpers Helpers to initialize the spawned actor.
     * @return A pointer to the newly spawned actor.
     */
    void RegisterUser(const FCavrnusUser& User, UWorld* World);

    /**
     * @brief Unregisters a spawned avatar from the Cavrnus system.
     *
     * This method removes the actor corresponding to the provided spawned avatar from the world
     * and unregisters it from the internal tracking map.
     *
     * @param SpawnedObject The spawned object data to unregister.
     * @param World The world from which to remove the actor.
     */
    void UnregisterUser(const FCavrnusUser& User, UWorld* World);

    void Clear();

	void SetRemoteUserClassType(TSubclassOf<AActor> ClassType);

private:
	struct FRemoteUserData
	{
		/** Spawned actor */
		AActor* Avatar = nullptr;

		/** Binding to track visibility of avatars in the Cavrnus system. */
		FString VisBindingId;
	};
	
	static UCavrnusAvatarManager* Instance;
	
	TSubclassOf<AActor> RemoteUserClassType = nullptr;

	FCavrnusUser CachedLocalUser = FCavrnusUser();
	bool HasSetupLocalUser = false;
	
	TMap<FPropertiesContainer, FRemoteUserData> SpawnedAvatarData;
	
	void SetupLocalUserPawn(const FCavrnusUser& User);
	void CleanUpLocalUserOnExitSpace(const FCavrnusUser& User);

	UFUNCTION()
	void LocalPawnControllerChanged(APawn* InPawn, AController* InController);

	UFUNCTION()
	void LocalPawnPossessedChanged(APawn* OldPawn, APawn* NewPawn);

	UGameInstance* GetGameInstance(const UWorld* World);
};