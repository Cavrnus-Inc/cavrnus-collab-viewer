// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "CavrnusDataSmithPropertiesHandler.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/UObjectGlobals.h"
#include "CavrnusDataSmithTransformSync.h"
#include "CavrnusFunctionLibrary.h"
#include "CavrnusPropertyAssetProcessor.h"

#include "CavrnusCVTGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"



void UCavrnusDataSmithPropertiesHandler::Setup(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainerName, AActor* InDataSmithActor)
{
	SpaceConnection = InSpaceConn;
	ContainerName = InContainerName;
	DataSmithActor = InDataSmithActor;
	
	if (const UWorld* World = DataSmithActor->GetWorld())
	{
		World->GetTimerManager().SetTimer(CheckHierarchyHandle, FTimerDelegate::CreateUObject(this,
		&UCavrnusDataSmithPropertiesHandler::CheckHierarchyExists), 0.5f, true);
	}

	if (auto pc = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		pc->InputComponent->BindKey(EKeys::Slash, IE_Pressed, this, &UCavrnusDataSmithPropertiesHandler::DatasmithLoadComplete);
		//pc->InputComponent->BindKey(EKeys::O, IE_Pressed, this, &ACavrnusTwinmotionSetup::DoDebugFunction1);

		//BindReesourceCreationDestructionEvents();
	}
}

// Currently triggered by keypress as we don't have an event for the finish of all datasmith assets
void UCavrnusDataSmithPropertiesHandler::DatasmithLoadComplete()
{
	ProcessStaticMeshHierarchy();
}

void UCavrnusDataSmithPropertiesHandler::BeginDestroy()
{
	UObject::BeginDestroy();
	TransformSyncs.Empty();
}

void UCavrnusDataSmithPropertiesHandler::CheckHierarchyExists()
{
	TArray<AActor*> AttachedActors;
	if (Cast<ADatasmithRuntimeActor>(DataSmithActor)->IsReceiving())
	{
		UE_LOG(LogTemp, Verbose, TEXT("Waiting for Datasmith hierarchy to load..."));
		return;
	}
	DataSmithActor->GetAttachedActors(AttachedActors);
	if (AttachedActors.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("FAILURE!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Datasmith hierarchy is ready! Found %d actors."), AttachedActors.Num());

	// Stop polling
	if (const UWorld* World = DataSmithActor->GetWorld())
		World->GetTimerManager().ClearTimer(CheckHierarchyHandle);

	//ProcessStaticMeshHierarchy();
}



void UCavrnusDataSmithPropertiesHandler::ProcessStaticMeshHierarchy()
{
	TArray<AStaticMeshActor*> AllMeshActors;
	GetAllStaticMeshActorsRecursive(DataSmithActor, AllMeshActors);

	if (auto* SyncTransform = NewObject<UCavrnusDataSmithTransformSync>())
	{
		FString ActorName = DataSmithActor->GetName();	
		SyncTransform->Setup(SpaceConnection, ContainerName, "Transform", DataSmithActor);
	}
	UCavrnusCVTGameSubsystem* CVTSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UCavrnusCVTGameSubsystem>();
	CVTSubsystem->GetCPAP()->ProcessRuntimeDatasmithActorProperties(DataSmithActor, ContainerName);

}


void UCavrnusDataSmithPropertiesHandler::GetAllStaticMeshActorsRecursive(const AActor* InRoot, TArray<AStaticMeshActor*>& OutMeshActors)
{
	if (!InRoot)
		return;

	TArray<AActor*> AttachedActors;
	InRoot->GetAttachedActors(AttachedActors);

	for (auto* AA : AttachedActors)
	{
		if (!AA)
			continue;

		if (auto* Sma = Cast<AStaticMeshActor>(AA))
			OutMeshActors.Add(Sma);

		GetAllStaticMeshActorsRecursive(AA, OutMeshActors);
	}
}