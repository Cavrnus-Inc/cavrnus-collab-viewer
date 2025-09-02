// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "DatasmithRuntimeActorHandler.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UDatasmithRuntimeActorHandler::Initialize(ADatasmithRuntimeActor* InActor, const FString& InSourcePath, const FOnDatasmithRuntimeActorLoaded& InCallback)
{
    TargetActor = InActor;
    SourcePath = InSourcePath;
    OnActorLoadedCallback = InCallback;

    if (!TargetActor.IsValid())
        return;

    TargetActor->LoadFile(SourcePath);

    UWorld* World = GetWorld();
    if (World)
    {
        World->GetTimerManager().SetTimer(PollTimerHandle, this, &UDatasmithRuntimeActorHandler::PollActorStatus, 0.1f, true);
    }
}

void UDatasmithRuntimeActorHandler::PollActorStatus()
{
    if (!TargetActor.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Datasmith Loader : Target Actor is no longer valid"));
        return;
    }

    ADatasmithRuntimeActor* Actor = TargetActor.Get();

    const bool bIsBuilding = Actor->bBuilding;
    const bool bIsReceiving = Actor->IsReceiving();

   
    if (bWaitingForLoadStart)
    {
        if (bIsBuilding || bIsReceiving)
        {
            UE_LOG(LogTemp, Warning, TEXT("Datasmith Loader : Waiting for Load to start"));
            bWaitingForLoadStart = false;
        }
        return;
    }
    if (bIsBuilding || bIsReceiving)
    {
        UE_LOG(LogTemp, Warning, TEXT("Datasmith Loader : Loading In Progress"))
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Datasmith Loader : Loading Finished"))

    UE_LOG(LogTemp, Error, TEXT("XXXX - TIMER FINISHED"));
    OnActorLoadedCallback.ExecuteIfBound(Actor);
    GetWorld()->GetTimerManager().ClearTimer(PollTimerHandle);

    RemoveFromRoot(); // Oh, this is likely to screw us eventually.  Should be managed in a subsystem.  Ugh
    MarkAsGarbage(); // Destroy Handler

}

void UDatasmithRuntimeActorHandler::BeginDestroy()
{
    UWorld* World = GetWorld();
    if (World)
    {
        World->GetTimerManager().ClearTimer(PollTimerHandle);
    }
    Super::BeginDestroy();
}