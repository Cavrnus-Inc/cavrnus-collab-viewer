// Copyright(c) Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Types/CavrnusSpaceConnection.h>

#include "CavrnusPawnAnimationAccessor.h"
#include "LivePropertyUpdates/CavrnusLivePropertyUpdate.h"
#include "LivePropertyUpdates/CavrnusLiveVectorPropertyUpdate.h"

#include "CavrnusSyncHeadRotation.generated.h"

UCLASS(Blueprintable)
class CAVRNUSCONNECTOR_API UCavrnusSyncHeadRotation : public UActorComponent
{
	GENERATED_BODY()

public:
	UCavrnusSyncHeadRotation();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginDestroy() override;
	
	UFUNCTION()
	void FrameDelay();
	
protected:
	UPROPERTY()
	UCavrnusLiveVectorPropertyUpdate* ValueUpdater = nullptr;

	UPROPERTY()
	UCavrnusPawnAnimationAccessor* AnimAccessor = nullptr;

	FPropertiesContainer Container;
	FCavrnusSpaceConnection SpaceConnection;
	
	bool bIsLocalPawn = false;
	FRotator HeadRotation;

	void SpaceConnected(FCavrnusSpaceConnection SpaceConn);
	void SetupRemotePlayer();
	void UpdateHeadLookAt();
};
