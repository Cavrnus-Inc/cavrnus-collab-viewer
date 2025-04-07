// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CavrnusDataSmithTransformSync.h"
#include "Types/CavrnusSpaceConnection.h"
#include "UObject/Object.h"
#include "CavrnusDataSmithPropertiesHandler.generated.h"

class AStaticMeshActor;
struct FCavrnusSpaceConnection;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class CAVRNUSCVT_API UCavrnusDataSmithPropertiesHandler : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Cavrnus")
	void Setup(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainerName, AActor* DataSmithActor);

private:
	virtual void BeginDestroy() override;
	
	void StartPollingForHierarchy(AActor* DatasmithActor);
	void CheckStaticMeshHierarchyReady(AActor* DatasmithActor);
	void ProcessStaticMeshHierarchy(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainerName, AActor* DataSmithActor);

	void GetAllStaticMeshActorsRecursive(AActor* Root, TArray<AStaticMeshActor*>& OutMeshActors);

	FTimerHandle CheckHierarchyHandle = FTimerHandle();

	FCavrnusSpaceConnection SpaceConnection = FCavrnusSpaceConnection();
	FString ContainerName = "";

	UPROPERTY()
	TArray<UCavrnusDataSmithTransformSync*> TransformSyncs;
};