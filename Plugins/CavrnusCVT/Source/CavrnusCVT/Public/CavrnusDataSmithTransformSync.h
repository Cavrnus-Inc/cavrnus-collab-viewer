// Copyright (c) 2024 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LivePropertyUpdates/CavrnusLivePropertyUpdate.h"
#include "Types/CavrnusBinding.h"
#include "UObject/Object.h"

#include "CavrnusDataSmithTransformSync.generated.h"

struct FCavrnusSpaceConnection;
/**
 * 
 */
UCLASS()
class CAVRNUSCVT_API UCavrnusDataSmithTransformSync : public UObject
{
	GENERATED_BODY()
public:
	void Setup(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainer, const FString& InProperty, AActor* InActor);
	
	virtual void BeginDestroy() override;
	
private:
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;
	
	UPROPERTY()
	TObjectPtr<UCavrnusBinding> Binding;
	
	FTimerHandle SetBindingHandle = FTimerHandle();
	FTimerHandle TransformUpdaterHandle = FTimerHandle();
	
	UPROPERTY()
	TObjectPtr<UCavrnusLivePropertyUpdate> LiveUpdater = nullptr;

	bool IgnoreTransformUpdate = false;
	
	void SetBinding(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainer, const FString& InProperty, AActor* InActor);
};
