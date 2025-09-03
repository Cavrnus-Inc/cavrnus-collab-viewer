// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "CavrnusAvatarManager.h"
#include "CavrnusConnectorModule.h"
#include "SpawnObjectHelpers.h"
#include "ValueSyncs/CavrnusValueSyncVector.h"
#include "ValueSyncs/CavrnusValueSyncFloat.h"
#include "FlagComponents/CavrnusLocalUserFlag.h"
#include <Engine/Engine.h>
#include <Engine/GameViewportClient.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/PlayerController.h>
#include "Engine/GameInstance.h"
#include "Engine/World.h" 

#include "CavrnusGCManager.h"

UCavrnusAvatarManager* UCavrnusAvatarManager::Instance = nullptr;

UCavrnusAvatarManager* UCavrnusAvatarManager::GetAvatarManager()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<UCavrnusAvatarManager>();
		CavrnusGCManager::GetGCManager()->TrackItem(Instance);
	}
	
	return Instance;
}

void UCavrnusAvatarManager::Kill()
{
	if (IsValid(Instance))
		CavrnusGCManager::GetGCManager()->UntrackItem(Instance);
	
	Instance = nullptr;
}

void UCavrnusAvatarManager::RegisterUser(const FCavrnusUser& User, UWorld* World)
{
	if (User.IsLocalUser && !HasSetupLocalUser)
	{
		CachedLocalUser = User;

		if (const auto GI = GetGameInstance(World))
		{
			GI->GetOnPawnControllerChanged().AddDynamic(this, &UCavrnusAvatarManager::LocalPawnControllerChanged);
			if (APlayerController* PlayerController = GI->GetFirstLocalPlayerController())
				PlayerController->OnPossessedPawnChanged.AddDynamic(this, &UCavrnusAvatarManager::LocalPawnPossessedChanged);
		}

		SetupLocalUserPawn(User);
		HasSetupLocalUser = true;
		
		return;
	}

	TSubclassOf<AActor> ActorClass = RemoteUserClassType;
	if (ActorClass == nullptr)
	{
		ActorClass = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/CavrnusConnector/Pawns/Blueprints/BP_Cavrnus_RemoteAvatarLoader.BP_Cavrnus_RemoteAvatarLoader_C"), nullptr, LOAD_None, nullptr);
	}
	auto actor = SpawnObjectHelpers::GetSpawnObjectHelpers()->SpawnUserAndSetup(World, ActorClass, User);

	FRemoteUserData Data;
	Data.Avatar = actor;

	UCavrnusFunctionLibrary::DefineBoolPropertyDefaultValue(User.SpaceConn, User.PropertiesContainerName, "AvatarVis", false);
	auto OnAvatarVisUpdated = [this, actor](bool bIsVisble, FString, FString)
	{
		actor->GetRootComponent()->SetVisibility(bIsVisble, true);
	};
	Data.VisBindingId = UCavrnusFunctionLibrary::BindBooleanPropertyValue(User.SpaceConn, User.PropertiesContainerName, "AvatarVis", OnAvatarVisUpdated)->BindingId;

	SpawnedAvatarData.Add(FPropertiesContainer(User.PropertiesContainerName), Data);
}

void UCavrnusAvatarManager::UnregisterUser(const FCavrnusUser& User, UWorld* World)
{
	if (User.IsLocalUser)
		return;

	FPropertiesContainer PropContainer(User.PropertiesContainerName);

	if (!SpawnedAvatarData.Contains(PropContainer))
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("Failed to destroy actor, could not find spawned object with Container Name %s"), *User.PropertiesContainerName);
		return;
	}

	FRemoteUserData Data = SpawnedAvatarData[PropContainer];

	UCavrnusFunctionLibrary::UnbindWithId(Data.VisBindingId);

	Data.Avatar->Destroy();

	SpawnedAvatarData.Remove(PropContainer);
}

void UCavrnusAvatarManager::Clear()
{
	for (auto data : SpawnedAvatarData) 
		data.Value.Avatar->Destroy();
	
	SpawnedAvatarData.Empty();
}

void UCavrnusAvatarManager::CleanUpLocalUserOnExitSpace(const FCavrnusUser& User)
{
	if (User.IsLocalUser)
	{
		APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(GEngine->GameViewport->GetWorld());
		APawn* Pawn = PlayerController->GetPawn();
		if (!Pawn)
			return;

		PlayerController->OnPossessedPawnChanged.RemoveDynamic(this, &UCavrnusAvatarManager::LocalPawnPossessedChanged);
		if (const auto GI = GetGameInstance(GEngine->GameViewport->GetWorld()))
			GI->GetOnPawnControllerChanged().RemoveDynamic(this, &UCavrnusAvatarManager::LocalPawnControllerChanged);
		
		UCavrnusLocalUserFlag* LocalUserComponent = Cast<UCavrnusLocalUserFlag>(Pawn->GetComponentByClass(UCavrnusLocalUserFlag::StaticClass()));
		if (!LocalUserComponent)
			return;

		LocalUserComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		LocalUserComponent->UnregisterComponent();
		LocalUserComponent->DestroyComponent();
		
		HasSetupLocalUser = false;
	}
}

void UCavrnusAvatarManager::LocalPawnControllerChanged(APawn* InPawn, AController* InController)
{
	if (InController && InController->IsLocalPlayerController() && InPawn)
		SetupLocalUserPawn(CachedLocalUser);
}

void UCavrnusAvatarManager::LocalPawnPossessedChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (IsValid(NewPawn))
		SetupLocalUserPawn(CachedLocalUser);
}

UGameInstance* UCavrnusAvatarManager::GetGameInstance(const UWorld* World)
{
	if (!World)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("GetGameInstance failed: World is null."));
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
		UE_LOG(LogCavrnusConnector, Error, TEXT("GetGameInstance failed: GameInstance is null."));

	return GameInstance;
}

void UCavrnusAvatarManager::SetRemoteUserClassType(TSubclassOf<AActor> ClassType)
{
	RemoteUserClassType = ClassType;
}

void UCavrnusAvatarManager::SetupLocalUserPawn(const FCavrnusUser& User)
{
	APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(GEngine->GameViewport->GetWorld());
	if (PlayerController == nullptr)
		return;

	APawn* Pawn = PlayerController->GetPawn();
	if (!Pawn || Pawn->GetComponentByClass(UCavrnusLocalUserFlag::StaticClass()))
		return;

	USceneComponent* PawnRootComponent = Pawn->GetRootComponent();
	ensureAlwaysMsgf(PawnRootComponent != nullptr, TEXT("No root component on pawn when attaching local user component"));
	UCavrnusLocalUserFlag* LocalUserComponent = Cast<UCavrnusLocalUserFlag>(Pawn->AddComponentByClass(
		UCavrnusLocalUserFlag::StaticClass(),
		true,
		FTransform::Identity,
		false));

	ensureAlwaysMsgf(LocalUserComponent != nullptr, TEXT("No local user component to attach"));
	LocalUserComponent->AttachToComponent(PawnRootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	LocalUserComponent->RegisterComponent();

	FString PropertyPath = TEXT("users/");

	if (FCavrnusSpaceConnectionInfo* SpaceConnInfo = UCavrnusFunctionLibrary::GetCavrnusSpaceConnectionInfo(User.SpaceConn))
	{
		PropertyPath += SpaceConnInfo->LocalUserConnectionId;
	}

	SpawnObjectHelpers::ResetLiveHierarchyRootName(Pawn, PropertyPath);

	CavrnusSpaceUserEvent evt = [LocalUserComponent](const FCavrnusUser& user) {
		LocalUserComponent->LocalUser = user;
	};

	UCavrnusFunctionLibrary::BeginTransientBoolPropertyUpdate(User.SpaceConn, PropertyPath, "AvatarVis", true);

	UCavrnusFunctionLibrary::AwaitLocalUser(User.SpaceConn, evt);

	UCavrnusFunctionLibrary::AwaitAnySpaceExited([this, User]()
	{
		this->CleanUpLocalUserOnExitSpace(User);
	});
}
