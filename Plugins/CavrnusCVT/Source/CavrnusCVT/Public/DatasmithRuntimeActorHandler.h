// Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DatasmithRuntime.h"
#include "Delegates/DelegateCombinations.h"

#include "DatasmithRuntimeActorHandler.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDatasmithRuntimeActorLoaded, ADatasmithRuntimeActor*, SpawnedActor);

UCLASS()
class UDatasmithRuntimeActorHandler : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "DatasmithRuntime")
    void Initialize(ADatasmithRuntimeActor* InActor, const FString& InSourcePath, const FOnDatasmithRuntimeActorLoaded& InCallback);

private:
    TWeakObjectPtr<ADatasmithRuntimeActor> TargetActor;
    FString SourcePath;
    FTimerHandle PollTimerHandle;
    bool bWaitingForLoadStart = true;
    bool bAutoDestroy = true;

    FOnDatasmithRuntimeActorLoaded OnActorLoadedCallback;

    void PollActorStatus();
};
