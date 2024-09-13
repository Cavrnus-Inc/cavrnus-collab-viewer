// Copyright(c) Cavrnus. All rights reserved.
#include "CavrnusAvatarManager.h"
#include "CavrnusConnectorModule.h"
#include "SpawnObjectHelpers.h"
#include "ValueSyncs/CavrnusValueSyncVector.h"
#include "ValueSyncs/CavrnusValueSyncFloat.h"
#include "ValueSyncs/CavrnusValueSyncString.h"
#include "CavrnusPropertiesContainer.h"
#include "CavrnusSpatialConnector.h"
#include "CavrnusSpatialConnectorSubSystem.h"
#include "FlagComponents/CavrnusUserAvatarFlag.h"
#include <Kismet/KismetSystemLibrary.h>
#include <MaterialTypes.h>
#include <Kismet/GameplayStatics.h>

//===========================================================
UCavrnusAvatarManager::UCavrnusAvatarManager()
{
}

//===========================================================
UCavrnusAvatarManager::~UCavrnusAvatarManager()
{
}

void UCavrnusAvatarManager::RegisterUser(const FCavrnusUser& User, TSubclassOf<AActor> ActorClass, UWorld* World, USpawnObjectHelpers* spawnHelpers)
{
	//Always ignore the local user
	if (User.IsLocalUser)
		return;

	auto actor = spawnHelpers->SpawnUserAndSetup(World, ActorClass, User);

	SpawnedAvatars.Add(FPropertiesContainer(User.PropertiesContainerName), actor);

	UCavrnusFunctionLibrary::DefineBoolPropertyDefaultValue(User.SpaceConn, User.PropertiesContainerName, "AvatarVis", false);
	auto OnAvatarVisUpdated = [this, actor](bool bIsVisble, FString, FString)
	{
		actor->GetRootComponent()->SetVisibility(bIsVisble, true);
	};
	AvatarVis = UCavrnusFunctionLibrary::BindBooleanPropertyValue(User.SpaceConn, User.PropertiesContainerName, "AvatarVis", OnAvatarVisUpdated);
}

void UCavrnusAvatarManager::UnregisterUser(const FCavrnusUser& User, UWorld* World)
{
	if (User.IsLocalUser)
		return;

	if (!SpawnedAvatars.Contains(FPropertiesContainer(User.PropertiesContainerName)))
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("Failed to destroy actor, could not find spawned object with Container Name %s"), *User.PropertiesContainerName);
		return;
	}

	if (AvatarVis)
		AvatarVis->Unbind();

	SpawnedAvatars[FPropertiesContainer(User.PropertiesContainerName)]->Destroy();
}