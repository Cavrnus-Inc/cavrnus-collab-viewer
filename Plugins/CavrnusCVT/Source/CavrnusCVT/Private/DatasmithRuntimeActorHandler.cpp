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
        return;

    ADatasmithRuntimeActor* Actor = TargetActor.Get();

    const bool bIsBuilding = Actor->bBuilding;
    const bool bIsReceiving = Actor->IsReceiving();

    UE_LOG(LogTemp, Warning, TEXT("%s %s"), bIsBuilding ? TEXT("B") : TEXT("NB"), bIsReceiving ? TEXT("R") : TEXT("NR"));
    if (bWaitingForLoadStart)
    {
        if (bIsBuilding || bIsReceiving)
        {
            bWaitingForLoadStart = false;
        }
        return;
    }

    if (bIsBuilding || bIsReceiving)
    {
        return;
    }

    UE_LOG(LogTemp, Error, TEXT("XXXX - TIMER FINISHED"));
    OnActorLoadedCallback.ExecuteIfBound(Actor);
    GetWorld()->GetTimerManager().ClearTimer(PollTimerHandle);


    MarkAsGarbage(); // Destroy Handler
}
