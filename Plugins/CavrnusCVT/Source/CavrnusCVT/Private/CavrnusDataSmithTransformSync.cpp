// Copyright (c) 2025 Cavrnus. All rights reserved.

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

bool AreTransformsApproximatelyEqual(const FTransform& A, const FTransform& B, float Tolerance = KINDA_SMALL_NUMBER)
{
	return A.GetLocation().Equals(B.GetLocation(), Tolerance) &&
		A.GetRotation().Equals(B.GetRotation(), Tolerance) &&
		A.GetScale3D().Equals(B.GetScale3D(), Tolerance);
}

void UCavrnusDataSmithTransformSync::SetLocalBinding()
{
	// LOCAL UPDATE
	TargetActor->GetRootComponent()->TransformUpdated.AddLambda([this](const USceneComponent* UpdatedComponent, EUpdateTransformFlags, ETeleportType)
	{
		if (IgnoreTransformUpdate)
			return;

		// Need check here to see if the local transform actually changed.  Is not changing when root is moved, but all relative transforms are posting
		FTransform JournalValue = UCavrnusFunctionLibrary::GetTransformPropertyValue(SpaceConnection, ContainerName, PropertyName);
		FTransform NewValue = UpdatedComponent->GetRelativeTransform();
		if (!AreTransformsApproximatelyEqual(JournalValue, NewValue))
		{
			CancelLocalFinalizeTimer();
			if (LiveUpdater)
				LiveUpdater->UpdateWithNewData(UpdatedComponent->GetRelativeTransform());
			else
				LiveUpdater = UCavrnusFunctionLibrary::BeginTransientTransformPropertyUpdate(SpaceConnection, ContainerName, PropertyName, UpdatedComponent->GetRelativeTransform(), PostOptions);

			TrySetLocalFinalizeTimer();
		}
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
	if (TargetActor && TransformUpdaterHandle.IsValid())
	{
		const UWorld* World = TargetActor->GetWorld();
		World->GetTimerManager().ClearTimer(TransformUpdaterHandle);
	}
}