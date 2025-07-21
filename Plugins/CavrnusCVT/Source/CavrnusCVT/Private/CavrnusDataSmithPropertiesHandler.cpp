// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "CavrnusDataSmithPropertiesHandler.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/UObjectGlobals.h"
#include "CavrnusDataSmithTransformSync.h"
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
	//For debugging things without Cavrnus running:
	if (auto pc = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		pc->InputComponent->BindKey(EKeys::Slash, IE_Pressed, this, &UCavrnusDataSmithPropertiesHandler::DoDebugFunction0);
		//pc->InputComponent->BindKey(EKeys::O, IE_Pressed, this, &ACavrnusTwinmotionSetup::DoDebugFunction1);

		//BindReesourceCreationDestructionEvents();
	}
}

void UCavrnusDataSmithPropertiesHandler::BeginDestroy()
{
	UObject::BeginDestroy();
	TransformSyncs.Empty();
}

void UCavrnusDataSmithPropertiesHandler::CheckHierarchyExists()
{
	TArray<AActor*> AttachedActors;
	DataSmithActor->GetAttachedActors(AttachedActors);

	if (AttachedActors.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Datasmith hierarchy is ready! Found %d actors."), AttachedActors.Num());

		// Stop polling
		if (const UWorld* World = DataSmithActor->GetWorld())
			World->GetTimerManager().ClearTimer(CheckHierarchyHandle);

		ProcessStaticMeshHierarchy();
	}
	else
		UE_LOG(LogTemp, Verbose, TEXT("Waiting for Datasmith hierarchy to load..."));
}

int UCavrnusDataSmithPropertiesHandler::ProcessTwinmotionDatasmithChildUsingSlotNames(const AActor* Actor)
{
	if (!Actor)
		return 0;
	int FixCount = 0;

	TArray<UStaticMeshComponent*> MeshComponents;
	Actor->GetComponents<UStaticMeshComponent>(MeshComponents, true);

	UMaterialInterface* FallbackMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_Fallback.M_Fallback"));
	if (!FallbackMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("Fallback material not found!"));
		return 0;
	}

	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		if (!MeshComp)
			continue;
		MeshComp->SetForcedLodModel(1);

		TArray<FName> SlotNames = MeshComp->GetMaterialSlotNames();
		int32 NumMaterials = SlotNames.Num();

		bool bModified = false;

		for (int32 i = 0; i < NumMaterials; ++i)
		{
			UMaterialInterface* CurrentMat = MeshComp->GetMaterialByName(SlotNames[i]);
			UMaterialInterface* OtherMat = MeshComp->GetMaterial(i);
			if (OtherMat != CurrentMat)
			{
				int a = 1;
			}
			if (!CurrentMat)
			{
				UE_LOG(LogTemp, Error, TEXT("Missing Material with SlotName %s"), *SlotNames[i].ToString());
				UE_LOG(LogTemp, Warning, TEXT("Missing material on actor %s, component %s, slot %d"),
					*Actor->GetName(), *MeshComp->GetName(), i);
				for (int j = i + 1; j < NumMaterials; ++j)
				{
					if (SlotNames[i] == SlotNames[j])
					{
						MeshComp->SetMaterial(i, MeshComp->GetMaterial(j));
						bModified = true;
						FixCount++;
						break;
					}
				}
			}
		}

		if (bModified)
		{
			MeshComp->MarkRenderStateDirty();
		}
	}
	return FixCount;
}


void UCavrnusDataSmithPropertiesHandler::ProcessStaticMeshHierarchy()
{
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
			
			SyncTransform->Setup(SpaceConnection, ContainerName, UniqueName, MeshActor);
			TransformSyncs.Add(SyncTransform);
		}
	}
}

void UCavrnusDataSmithPropertiesHandler::FixMaterialsOnRuntimeDatasmithActor(ADatasmithRuntimeActor* DatasmithActor)
{
	if (!DatasmithActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("DatasmithActor is null."));
		return;
	}
	if (DatasmithActor->IsReceiving())
	{
		UE_LOG(LogTemp, Warning, TEXT("DatasmithActor is still loading."));
		return;
	}

	TArray<AStaticMeshActor*> ActorsToProcess;

	GetAllStaticMeshActorsRecursive(DatasmithActor, ActorsToProcess);

	UE_LOG(LogTemp, Error, TEXT("Found %d Datasmith Children on Actor %s"), ActorsToProcess.Num(), *DatasmithActor->GetName());

	int TotalFixed = 0;

	ProcessTwinmotionDatasmithChildUsingSlotNames(DatasmithActor);

	for (AActor* Actor : ActorsToProcess)
	{
		TotalFixed += ProcessTwinmotionDatasmithChildUsingSlotNames(Actor);
	}

	UE_LOG(LogTemp, Log, TEXT("Finished fixing materials. Total slots fixed: %d"), TotalFixed);
}


void UCavrnusDataSmithPropertiesHandler::DoDebugFunction0()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADatasmithRuntimeActor::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		ADatasmithRuntimeActor* DActor = Cast<ADatasmithRuntimeActor>(Actor);
		if (DActor)
		{
			FixMaterialsOnRuntimeDatasmithActor(DActor);
		}

	}
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