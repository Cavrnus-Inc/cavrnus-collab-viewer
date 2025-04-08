// Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LivePropertyUpdates/CavrnusLiveTransformPropertyUpdate.h"
#include "Types/CavrnusBinding.h"
#include "UObject/Object.h"

#include "CavrnusDataSmithTransformSync.generated.h"

struct FCavrnusSpaceConnection;

/**
 * Sync component that is placed on each staticMeshActor within Datasmith Actor
 */
UCLASS()
class CAVRNUSCVT_API UCavrnusDataSmithTransformSync : public UObject
{
	GENERATED_BODY()
public:
	void Setup(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainer, const FString& InProperty, AActor* InActor);

protected:
	virtual void BeginDestroy() override;
	
private:
	FString ContainerName = "";
	FString PropertyName = "";
	
	bool IgnoreTransformUpdate = false;
	
	FTimerHandle SetBindingHandle = FTimerHandle();
	FTimerHandle TransformUpdaterHandle = FTimerHandle();
	
	FCavrnusSpaceConnection SpaceConnection = FCavrnusSpaceConnection();
	FPropertyPostOptions PostOptions = FPropertyPostOptions();
	
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;
	UPROPERTY()
	TObjectPtr<UCavrnusBinding> Binding;
	UPROPERTY()
	TObjectPtr<UCavrnusLiveTransformPropertyUpdate> LiveUpdater = nullptr;
	
	void SetLocalBinding();
	void SetServerBinding();

	void TrySetLocalFinalizeTimer();
	void CancelLocalFinalizeTimer();
};
