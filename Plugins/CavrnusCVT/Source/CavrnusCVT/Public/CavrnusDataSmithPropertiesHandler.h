// Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "CavrnusDataSmithTransformSync.h"
#include "Types/CavrnusSpaceConnection.h"
#include "UObject/Object.h"
#include "DatasmithRuntime.h"
#include "CavrnusDataSmithPropertiesHandler.generated.h"

class AStaticMeshActor;
struct FCavrnusSpaceConnection;

/**
 * This class handles traversing DatasmithRuntimeActor hierarchies and attaches sync objects to each StaticMesh.
 * Note ==> Called from DatasmithLoader Blueprint
 */
UCLASS(BlueprintType, Blueprintable)
class CAVRNUSCVT_API UCavrnusDataSmithPropertiesHandler : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Cavrnus")
	void Setup(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainerName, AActor* InDataSmithActor);
	
protected:
	virtual void BeginDestroy() override;
	
private:
	FString ContainerName = "";
	FCavrnusSpaceConnection SpaceConnection = FCavrnusSpaceConnection();
	
	FTimerHandle CheckHierarchyHandle = FTimerHandle();
	
	UFUNCTION(BlueprintCallable, Category = "Cavrnus")
	void FixMaterialsOnRuntimeDatasmithActor(ADatasmithRuntimeActor* DatasmithActor);

	int ProcessTwinmotionDatasmithChildUsingSlotNames(const AActor* Actor);

	UFUNCTION()
	void DoDebugFunction0();

	UPROPERTY()
	AActor* DataSmithActor;
	UPROPERTY()
	TArray<UCavrnusDataSmithTransformSync*> TransformSyncs;
	
	void CheckHierarchyExists();
	void ProcessStaticMeshHierarchy();
	
	void GetAllStaticMeshActorsRecursive(const AActor* InRoot, TArray<AStaticMeshActor*>& OutMeshActors);
};