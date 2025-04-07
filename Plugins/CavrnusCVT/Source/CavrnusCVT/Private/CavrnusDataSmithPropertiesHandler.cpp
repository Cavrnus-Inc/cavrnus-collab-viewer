// Copyright (c) 2024 Cavrnus. All rights reserved.

#include "CavrnusDataSmithPropertiesHandler.h"

#include "CavrnusDataSmithTransformSync.h"
#include "Engine/StaticMeshActor.h"

FTimerHandle CheckHierarchyHandle;

void UCavrnusDataSmithPropertiesHandler::Setup(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainerName, AActor* DataSmithActor)
{
	SpaceConnection = InSpaceConn;
	ContainerName = InContainerName;
	
	StartPollingForHierarchy(DataSmithActor);
}

void UCavrnusDataSmithPropertiesHandler::BeginDestroy()
{
	UObject::BeginDestroy();
	TransformSyncs.Empty();
}

void UCavrnusDataSmithPropertiesHandler::StartPollingForHierarchy(AActor* DatasmithActor)
{
	if (!DatasmithActor) return;

	UWorld* World = DatasmithActor->GetWorld();
	if (!World) return;

	World->GetTimerManager().SetTimer(CheckHierarchyHandle, FTimerDelegate::CreateUObject(
		this,
		&UCavrnusDataSmithPropertiesHandler::CheckStaticMeshHierarchyReady,
		DatasmithActor
	), 0.5f, true); // every 0.5 seconds
}

void UCavrnusDataSmithPropertiesHandler::CheckStaticMeshHierarchyReady(AActor* DatasmithActor)
{
	TArray<AActor*> AttachedActors;
	DatasmithActor->GetAttachedActors(AttachedActors);

	if (AttachedActors.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Datasmith hierarchy is ready! Found %d actors."), AttachedActors.Num());

		// Stop polling
		if (UWorld* World = DatasmithActor->GetWorld())
			World->GetTimerManager().ClearTimer(CheckHierarchyHandle);

		ProcessStaticMeshHierarchy(SpaceConnection, ContainerName, DatasmithActor);
	}
	else
		UE_LOG(LogTemp, Verbose, TEXT("Waiting for Datasmith hierarchy to load..."));
}

void UCavrnusDataSmithPropertiesHandler::ProcessStaticMeshHierarchy(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainerName, AActor* DataSmithActor)
{
	if (DataSmithActor == nullptr)
		return;
	
	TArray<AStaticMeshActor*> AllMeshActors;
	GetAllStaticMeshActorsRecursive(DataSmithActor, AllMeshActors);

	TransformSyncs.Empty();  
	for (const auto MeshActor : AllMeshActors)
	{
		if (auto* SyncTransform = NewObject<UCavrnusDataSmithTransformSync>())
		{
			FString ActorName = MeshActor->GetName();
			const uint32 Hash = GetTypeHash(ActorName);
			FString UniqueName = FString::Printf(TEXT("%u_%s"), Hash, *ActorName);
			
			SyncTransform->Setup(InSpaceConn, InContainerName, UniqueName, MeshActor);
			TransformSyncs.Add(SyncTransform);
		}
	}
}

void UCavrnusDataSmithPropertiesHandler::GetAllStaticMeshActorsRecursive(AActor* Root, TArray<AStaticMeshActor*>& OutMeshActors)
{
	if (!Root)
		return;

	TArray<AActor*> AttachedActors;
	Root->GetAttachedActors(AttachedActors);
	
	for (auto* AA : AttachedActors)
	{
		if (!AA)
			continue;

		if (auto* SMA = Cast<AStaticMeshActor>(AA))
			OutMeshActors.Add(SMA);

		GetAllStaticMeshActorsRecursive(AA, OutMeshActors);
	}
}
