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
		{
			IgnoreTransformUpdate = false;
			UE_LOG(LogTemp, Log, TEXT("Ignored feedback transform update."));
			
			return;
		}
		
		CancelLocalFinalizeTimer();
		
		if (LiveUpdater)
			LiveUpdater->UpdateWithNewDataGeneric(GetTransformPropValue(UpdatedComponent->GetRelativeTransform()));
		else
		{
			const auto PropValue = GetTransformPropValue(UpdatedComponent->GetRelativeTransform());
			LiveUpdater = UCavrnusFunctionLibrary::BeginTransientGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName,PropValue);
		}
		
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
	});
}

Cavrnus::FPropertyValue UCavrnusDataSmithTransformSync::GetTransformPropValue(const FTransform& NewTransform)
{
	Cavrnus::FPropertyValue PropVal = Cavrnus::FPropertyValue();
	PropVal.PropType = Cavrnus::FPropertyValue::PropertyType::Transform;
	PropVal.TransformValue = NewTransform;

	return PropVal;
}

void UCavrnusDataSmithTransformSync::TrySetLocalFinalizeTimer()
{
	if (const UWorld* World = TargetActor->GetWorld())
	{
		World->GetTimerManager().SetTimer(TransformUpdaterHandle, [this]
		{
			UE_LOG(LogTemp, Log, TEXT("Transform updates have stopped. Running final logic..."));
			if (LiveUpdater)
			{
				Cavrnus::FPropertyValue PropVal = Cavrnus::FPropertyValue();
				PropVal.PropType = Cavrnus::FPropertyValue::PropertyType::Transform;
				PropVal.TransformValue = TargetActor->GetRootComponent()->GetRelativeTransform();
				LiveUpdater->FinalizeGeneric(PropVal);
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