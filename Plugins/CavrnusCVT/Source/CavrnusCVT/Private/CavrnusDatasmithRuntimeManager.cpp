#include "CavrnusDatasmithRuntimeManager.h"
#include "CavrnusPropertyAssetProcessor.h"
#include "CavrnusCVTGameSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UCavrnusDatasmithRuntimeManager::Setup(const FCavrnusSpaceConnection& InSpaceConn)
{
	SpaceConnection = InSpaceConn;
	TransformSyncs.Empty();
	DatasmithActorInfo.Empty();
}

void UCavrnusDatasmithRuntimeManager::RegisterTransformSync(AActor*Actor, UCavrnusDataSmithTransformSync* TransformSync)
{
	UE_LOG(LogTemp, Error, TEXT("Registered Transform Sync for Target Actor %p"), Actor);
	TransformSyncs.Add(TransformSync);
}

/**
 * Checks the status of a Datasmith Actor.  Actors are only registered after loading is completed.
 */
bool UCavrnusDatasmithRuntimeManager::IsRegistered(ADatasmithRuntimeActor* DActor)
{
	if (!IsValid(DActor))
		return false;

	TWeakObjectPtr<ADatasmithRuntimeActor> WeakActor = DActor;

	if (UDatasmithTrackingInfo** DTI = DatasmithActorInfo.Find(WeakActor))
	{
		return (*DTI)->HasFlag(EDatasmithStatusFlags::isRegistered);
	}

	return false;
}



bool UCavrnusDatasmithRuntimeManager::FindDatasmithActorByPath(const FString& SourcePath, ADatasmithRuntimeActor*& DActor, UDatasmithTrackingInfo*& DTI)
{
	for (const TPair<TWeakObjectPtr<ADatasmithRuntimeActor>, UDatasmithTrackingInfo*>& Pair : DatasmithActorInfo)
	{
		const TWeakObjectPtr<ADatasmithRuntimeActor>& WeakActor = Pair.Key;
		UDatasmithTrackingInfo* TrackingInfo = Pair.Value;

		if (!TrackingInfo || !WeakActor.IsValid())
			continue;

		if (TrackingInfo->SourcePath == SourcePath)
		{
			DActor = WeakActor.Get();
			DTI = TrackingInfo;
			return true;
		}
	}

	DActor = nullptr;
	DTI = nullptr;
	return false;
}


/**
 * Given the path name, checks the status of a Datasmith Actor.  
 * Actors are only registered after loading is completed.
 */
bool UCavrnusDatasmithRuntimeManager::IsRegisteredCheckPath(const FString& SourcePath)
{
	ADatasmithRuntimeActor* DActor;
	UDatasmithTrackingInfo* DTI;

	if (FindDatasmithActorByPath(SourcePath, DActor, DTI))
	{
		return DTI->HasFlag(EDatasmithStatusFlags::isRegistered);
	}
	return false;
}

/**
 * Sets Register flag for actor that has been registered
 */
bool UCavrnusDatasmithRuntimeManager::RegisterDatasmithActor(ADatasmithRuntimeActor* DActor)
{
	if (!IsValid(DActor))
		return false;

	TWeakObjectPtr<ADatasmithRuntimeActor> WeakActor = DActor;

	if (UDatasmithTrackingInfo** DTI = DatasmithActorInfo.Find(WeakActor))
	{
		(*DTI)->SetStatusFlag(EDatasmithStatusFlags::isRegistered);
		UE_LOG(LogTemp, Verbose, TEXT("Actor %s registered successfully"), *DActor->GetName());
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Failed to register actor %s — tracking info not found"), *DActor->GetName());
	return false;
}

/**
 * Unregister Actor from the Cavrnus Datasmith Runtime Manager.  
 * Does not Destroy Actor, but merely stops managing it.
 */
bool UCavrnusDatasmithRuntimeManager::UnregisterDatasmithActor(ADatasmithRuntimeActor* DActor)
{
	if (!IsValid(DActor))
		return false;

	TWeakObjectPtr<ADatasmithRuntimeActor> WeakActor = DActor;
	int32 RemovedCount = DatasmithActorInfo.Remove(WeakActor);

	if (RemovedCount > 0)
	{
		UE_LOG(LogTemp, Verbose, TEXT("Unregistered weak Datasmith actor: %s"), *DActor->GetName());
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to unregister weak actor: %s"), *DActor->GetName());
		return false;
	}
}

/**
 * Checks whether a Datasmith Actor is currently loading.
 * Note: Only valid after calling LoadFile(SourcePath).
 */
bool UCavrnusDatasmithRuntimeManager::IsLoading(ADatasmithRuntimeActor* DActor)
{
	if (!DActor)
		return false;

	TWeakObjectPtr<ADatasmithRuntimeActor> WeakActor = DActor;
	if (UDatasmithTrackingInfo** DTI = DatasmithActorInfo.Find(WeakActor))
	{
		return (*DTI)->HasFlag(EDatasmithStatusFlags::isLoading);
	}

	return false;
}

/**
 * Using the Path name, checks whether a Datasmith Actor is currently loading.
 * Note: Only valid after calling SpawnDatasmithActor(SourcePath).
 */
bool UCavrnusDatasmithRuntimeManager::IsLoadingCheckPath(const FString& SourcePath)
{
	ADatasmithRuntimeActor* DActor;
	UDatasmithTrackingInfo* DTI;

	if (FindDatasmithActorByPath(SourcePath, DActor, DTI))
	{
		return DTI->HasFlag(EDatasmithStatusFlags::isLoading);
	}

	return false;
}

/**
 * Checks whether a Datasmith Actor has completed loading.
 * Note: Only valid after calling LoadFile(SourcePath).
 */
bool UCavrnusDatasmithRuntimeManager::IsLoaded(ADatasmithRuntimeActor* DActor)
{
	if (!DActor)
		return false;

	TWeakObjectPtr<ADatasmithRuntimeActor> WeakActor = DActor;
	if (UDatasmithTrackingInfo** DTI = DatasmithActorInfo.Find(WeakActor))
	{
		return (*DTI)->HasFlag(EDatasmithStatusFlags::isLoaded);
	}

	return false;
}

/**
 * Using the Path name, Checks whether a Datasmith Actor has completed loading.
 * Note: Only valid after calling LoadFile(SourcePath).
 */
bool UCavrnusDatasmithRuntimeManager::IsLoadedCheckPath(const FString& SourcePath)
{
	ADatasmithRuntimeActor* DActor;
	UDatasmithTrackingInfo* DTI;

	if (FindDatasmithActorByPath(SourcePath, DActor, DTI))
	{
		return DTI->HasFlag(EDatasmithStatusFlags::isLoaded);
	}

	return false;
}
/**
* Adds Cavrnus properties to Datasmith Actor
*/
void UCavrnusDatasmithRuntimeManager::ProcessDatasmithActor(ADatasmithRuntimeActor* DActor, const FString& ContainerName)
{
	TWeakObjectPtr<UWorld> WeakWorld = GetWorld();
	if (!WeakWorld.IsValid() || !IsValid(DActor))
		return;

	UCavrnusCVTGameSubsystem* CVTSubsystem = WeakWorld->GetGameInstance()->GetSubsystem<UCavrnusCVTGameSubsystem>();
	TWeakObjectPtr<ADatasmithRuntimeActor> WeakActor = DActor;

	if (UDatasmithTrackingInfo** DTI = DatasmithActorInfo.Find(WeakActor))
	{
		if ((*DTI)->HasFlag(EDatasmithStatusFlags::isRegistered | EDatasmithStatusFlags::isLoaded))
		{
			CVTSubsystem->GetCPAP()->ProcessDatasmithRuntimeActorProperties(DActor, ContainerName);
			(*DTI)->SetStatusFlag(EDatasmithStatusFlags::isCavrnized);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TrackingInfo not found for Actor %s"), *DActor->GetName());
	}
}

bool DatasmithActorFinishedLoading(ADatasmithRuntimeActor* Actor, UDatasmithTrackingInfo** DTI)
{
	if (!IsValid(Actor))
		return false;
	// Need to latch here as it is possible for bBuilding and isReceiving() to return false early on load.
	if ((*DTI)->HasFlag(EDatasmithStatusFlags::isWaiting))
	{
		if (Actor->bBuilding || Actor->IsReceiving())
		{
			(*DTI)->ClearStatusFlag(EDatasmithStatusFlags::isWaiting);
			(*DTI)->SetStatusFlag(EDatasmithStatusFlags::isLoading);
		}
		return false;
	}
	if (Actor->bBuilding || Actor->IsReceiving())
	{
		return false;
	}
	else
	{
		TArray<AActor*> AttachedActors;
		Actor->GetAttachedActors(AttachedActors);
		UE_LOG(LogTemp, Error, TEXT("Load Complete %f %d %d"), Actor->Progress*100, AttachedActors.Num(), Actor->GetComponents().Num());
		return true;
	}
}

void UCavrnusDatasmithRuntimeManager::SpawnDatasmithActor(const FString& SourcePath, FOnDatasmithActorLoaded OnDatasmithActorLoaded)
{
	TWeakObjectPtr<UWorld> WeakWorld = GetWorld();
	if (!WeakWorld.IsValid())
		return;

	ADatasmithRuntimeActor* DatasmithActor = Cast<ADatasmithRuntimeActor>(WeakWorld->SpawnActor(ADatasmithRuntimeActor::StaticClass()));
	if (!DatasmithActor)
		return;

	LoadDatasmithActor(DatasmithActor, SourcePath, OnDatasmithActorLoaded);
}

/**
 * Loads the Datasmith file into the passed in Actor and Registers it with this Runtime Manager
 */
void UCavrnusDatasmithRuntimeManager::LoadDatasmithActor(ADatasmithRuntimeActor* DatasmithActor, const FString& SourcePath, FOnDatasmithActorLoaded OnDatasmithActorLoaded)
{
	UE_LOG(LogTemp, Warning, TEXT("LoadDatasmithActor called for %s"), *SourcePath);

	TWeakObjectPtr<UWorld> WeakWorld = GetWorld();
	if (!WeakWorld.IsValid())
		return;

	TWeakObjectPtr<ADatasmithRuntimeActor> WeakActor = DatasmithActor;

	UDatasmithTrackingInfo* NewDTI = NewObject<UDatasmithTrackingInfo>(this);
	NewDTI->SetStatusFlag(EDatasmithStatusFlags::isWaiting);
	NewDTI->SourcePath = SourcePath;

	// Store using the weak actor key
	DatasmithActorInfo.Add(WeakActor, NewDTI);

	WeakActor->LoadFile(SourcePath);

	TSharedPtr<FTimerHandle> HandlePtr = MakeShared<FTimerHandle>();

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, HandlePtr, WeakActor, OnDatasmithActorLoaded, WeakWorld]() mutable
		{
			if (!WeakWorld.IsValid() || !WeakActor.IsValid())
				return;

			ADatasmithRuntimeActor* Actor = WeakActor.Get();


			UDatasmithTrackingInfo** DTI = DatasmithActorInfo.Find(Actor);

			if (*DTI)
			{
				if (!DatasmithActorFinishedLoading(Actor, DTI))
					return;
				(*DTI)->ClearStatusFlag(EDatasmithStatusFlags::isLoading);
				(*DTI)->SetStatusFlag(EDatasmithStatusFlags::isLoaded);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to find TrackingInfo for %s"), *Actor->GetName());
			}

			RegisterDatasmithActor(Actor);
			OnDatasmithActorLoaded.ExecuteIfBound(Actor);

			WeakWorld->GetTimerManager().ClearTimer(*HandlePtr);
		});

	WeakWorld->GetTimerManager().SetTimer(*HandlePtr, TimerDelegate, 0.1f, true);
}


void UCavrnusDatasmithRuntimeManager::DestroyDatasmithActor(ADatasmithRuntimeActor* DActor)
{
	if (!IsValid(DActor))
		return;

	TWeakObjectPtr<ADatasmithRuntimeActor> WeakActor = DActor;

	if (UDatasmithTrackingInfo** DTI = DatasmithActorInfo.Find(WeakActor))
	{
		DActor->Destroy();
		DatasmithActorInfo.Remove(WeakActor);
		UE_LOG(LogTemp, Verbose, TEXT("Destroyed Datasmith actor: %s"), *DActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to destroy untracked actor: %s"), *DActor->GetName());
	}
}
