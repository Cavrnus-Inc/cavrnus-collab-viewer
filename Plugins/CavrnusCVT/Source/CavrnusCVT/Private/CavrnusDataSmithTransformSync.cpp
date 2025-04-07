// Copyright (c) 2024 Cavrnus. All rights reserved.

#include "CavrnusDataSmithTransformSync.h"
#include "CavrnusFunctionLibrary.h"

void UCavrnusDataSmithTransformSync::Setup(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainer, const FString& InProperty, AActor* InActor)
{
	SpaceConnection = InSpaceConn;
	ContainerName = InContainer;
	PropertyName = InProperty;
	TargetActor = InActor;
	
	SetLocalBinding();
	SetServerBinding();
}

void UCavrnusDataSmithTransformSync::BeginDestroy()
{
	UObject::BeginDestroy();
	ContainerName = "";
	PropertyName = "";
	TargetActor = nullptr;
}

void UCavrnusDataSmithTransformSync::SetLocalBinding()
{
	// LOCAL UPDATE
	TargetActor->GetRootComponent()->TransformUpdated.AddLambda([this](const USceneComponent* UpdatedComponent, EUpdateTransformFlags, ETeleportType)
	{
		if (IgnoreTransformUpdate)
			return;
		
		CancelLocalFinalizeTimer();
		
		if (LiveUpdater)
			LiveUpdater->UpdateWithNewData(UpdatedComponent->GetRelativeTransform());
		else
			LiveUpdater = UCavrnusFunctionLibrary::BeginTransientTransformPropertyUpdate(SpaceConnection, ContainerName, PropertyName, UpdatedComponent->GetRelativeTransform(), PostOptions);
		
		TrySetLocalFinalizeTimer();
	});
}

void UCavrnusDataSmithTransformSync::SetServerBinding()
{
	// SERVER UPDATE
	UCavrnusFunctionLibrary::DefineTransformPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, TargetActor->GetRootComponent()->GetRelativeTransform());
	Binding = UCavrnusFunctionLibrary::BindTransformPropertyValue(SpaceConnection, ContainerName, PropertyName, [this](const FTransform& Value, const FString&, const FString&)
	{
		IgnoreTransformUpdate = true;
		TargetActor->GetRootComponent()->SetRelativeTransform(Value);
		IgnoreTransformUpdate = false;
	});
}

void UCavrnusDataSmithTransformSync::TrySetLocalFinalizeTimer()
{
	if (const UWorld* World = TargetActor->GetWorld())
	{
		World->GetTimerManager().SetTimer(TransformUpdaterHandle, [this]
		{
			UE_LOG(LogTemp, Log, TEXT("Transform updates have stopped. Post finalized transform!"));
			if (LiveUpdater)
			{
				LiveUpdater->Finalize(TargetActor->GetRootComponent()->GetRelativeTransform());
				LiveUpdater = nullptr;
			}
		}, 0.1f, false);
	}
}

void UCavrnusDataSmithTransformSync::CancelLocalFinalizeTimer()
{
	if (const UWorld* World = TargetActor->GetWorld())
		World->GetTimerManager().ClearTimer(TransformUpdaterHandle);
}