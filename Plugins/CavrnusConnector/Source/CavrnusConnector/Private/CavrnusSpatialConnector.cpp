// Copyright(c) Cavrnus. All rights reserved.
#include "CavrnusSpatialConnector.h"

#include "CavrnusAvatarManager.h"
#include "CavrnusConnectorModule.h"
#include "CavrnusFunctionLibrary.h"
#include "CavrnusSpatialConnectorSubSystem.h"
#include "SpawnedObjectsManager.h"
#include <Engine/GameInstance.h>
#include <EngineUtils.h>

// Sets default values
ACavrnusSpatialConnector::ACavrnusSpatialConnector()
{
	// Disabled to improve performance since this actor does not need to update every frame. Set this actor to call Tick() every frame if you need it.
	PrimaryActorTick.bCanEverTick = false;

	AuthMethod = ECavrnusAuthMethod::JoinAsMember;	
	MemberLoginMethod = ECavrnusMemberLoginMethod::EnterMemberLoginCredentials;		
	GuestLoginMethod = ECavrnusGuestLoginMethod::EnterNameBelow;
	SpaceJoinMethod = ECavrnusSpaceJoinMethod::SpacesList;
}

void ACavrnusSpatialConnector::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (UCavrnusSpatialConnectorSubSystemProxy* SubProxy = UCavrnusFunctionLibrary::GetCavrnusSpatialConnectorSubSystemProxy())
	{
		SubProxy->RegisterCavrnusSpatialConnector(this);
	}
}

// Called when the game starts or when spawned
void ACavrnusSpatialConnector::BeginPlay()
{
	Super::BeginPlay();
	CavrnusBeginPlay();
}

void ACavrnusSpatialConnector::CavrnusBeginPlay()
{
	UWorld* WorldPtr = GetWorld();
	if (!WorldPtr)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("No world in ACavrnusSpatialConnector::BeginPlay()"));
		return;
	}

	if (UCavrnusSpatialConnectorSubSystemProxy* SubProxy = UCavrnusFunctionLibrary::GetCavrnusSpatialConnectorSubSystemProxy())
	{
		SubProxy->AuthenticateAndJoin();
	}
}

void ACavrnusSpatialConnector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CavrnusEndPlay();
}

void ACavrnusSpatialConnector::CavrnusEndPlay()
{
}
