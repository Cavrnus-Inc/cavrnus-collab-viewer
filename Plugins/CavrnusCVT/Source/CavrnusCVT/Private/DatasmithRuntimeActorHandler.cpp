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

    GetWorld()->GetTimerManager().ClearTimer(PollTimerHandle);
    OnActorLoadedCallback.ExecuteIfBound(Actor);

    MarkAsGarbage(); // Destroy Handler
}
