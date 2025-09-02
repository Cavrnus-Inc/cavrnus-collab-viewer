// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "ImprovedDatasmithRuntimeBlueprintLibrary.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

void UImprovedDatasmithRuntimeBlueprintLibrary::SpawnDatasmithRuntimeActor(UObject* WorldContextObject, const FString& FilePath, const FOnDatasmithRuntimeActorLoaded& OnLoadedCallback)
{
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
    UDatasmithRuntimeActorHandler* OutHandler;

    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid world context"));
        OutHandler = nullptr;
        return;
    }

    ADatasmithRuntimeActor* Actor = World->SpawnActor<ADatasmithRuntimeActor>();
    if (!Actor)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn Datasmith runtime actor"));
        OutHandler = nullptr;
        return;
    }

    OutHandler = NewObject<UDatasmithRuntimeActorHandler>(WorldContextObject);
    OutHandler->AddToRoot();
    OutHandler->Initialize(Actor, FilePath, OnLoadedCallback);

}
