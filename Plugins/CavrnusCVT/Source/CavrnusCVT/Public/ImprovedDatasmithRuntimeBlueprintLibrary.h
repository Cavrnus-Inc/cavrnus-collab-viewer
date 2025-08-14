#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DatasmithRuntimeActorHandler.h"
#include "DatasmithRuntime.h"
#include "Delegates/DelegateCombinations.h"

#include "ImprovedDatasmithRuntimeBlueprintLibrary.generated.h"

UCLASS()
class UImprovedDatasmithRuntimeBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    static void SpawnDatasmithRuntimeActor(UObject* WorldContextObject, const FString& FilePath, const FOnDatasmithRuntimeActorLoaded& OnLoadedCallback, bool bAutoDestroyHandler = true);
};
