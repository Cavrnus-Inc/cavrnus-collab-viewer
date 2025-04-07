// Copyright (c) 2024 Cavrnus. All rights reserved.

#include "CavrnusDataSmithTransformSync.h"
#include "CavrnusFunctionLibrary.h"

void UCavrnusDataSmithTransformSync::Setup(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainer, const FString& InProperty, AActor* InActor)
{
	if (InActor)
	{
		const UWorld* World = InActor->GetWorld();
		if (!World) return;

		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateWeakLambda(this, [this, InSpaceConn, InContainer, InProperty, InActor]
			{
				SetBinding(InSpaceConn, InContainer, InProperty, InActor);
			}));

		SetBinding(InSpaceConn, InContainer, InProperty, InActor);
	}
}

void UCavrnusDataSmithTransformSync::SetBinding(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainer, const FString& InProperty, AActor* InActor)
{
	bool bIsInitialized = false;

	UCavrnusFunctionLibrary::DefineTransformPropertyDefaultValue(InSpaceConn, InContainer, InProperty, InActor->GetRootComponent()->GetRelativeTransform());
	Binding = UCavrnusFunctionLibrary::BindTransformPropertyValue(InSpaceConn, InContainer, InProperty, [this, InActor, &bIsInitialized](const FTransform& Value, const FString&, const FString&)
	{
		IgnoreTransformUpdate = true;

		InActor->GetRootComponent()->SetRelativeTransform(Value);

		if (!bIsInitialized)
			bIsInitialized = true;
	});
	
	if (InActor)
	{
		InActor->GetRootComponent()->TransformUpdated.AddLambda([this, InActor, InSpaceConn, InContainer, InProperty, &bIsInitialized](const USceneComponent* UpdatedComponent, EUpdateTransformFlags, ETeleportType)
		{
			if (!bIsInitialized) return;

			if (IgnoreTransformUpdate)
			{
				IgnoreTransformUpdate = false;
				UE_LOG(LogTemp, Log, TEXT("Ignored feedback transform update."));
				return;
			}

			const FTransform& NewLocalTransform = UpdatedComponent->GetRelativeTransform();

			if (const UWorld* World = InActor->GetWorld())
				World->GetTimerManager().ClearTimer(TransformUpdaterHandle);
		
			if (LiveUpdater)
			{
				Cavrnus::FPropertyValue PropVal = Cavrnus::FPropertyValue();
				PropVal.PropType = Cavrnus::FPropertyValue::PropertyType::Transform;
				PropVal.TransformValue = NewLocalTransform;
				LiveUpdater->UpdateWithNewDataGeneric(PropVal);
			} else
			{
				Cavrnus::FPropertyValue PropVal = Cavrnus::FPropertyValue();
				PropVal.PropType = Cavrnus::FPropertyValue::PropertyType::Transform;
				PropVal.TransformValue = UpdatedComponent->GetRelativeTransform();
				LiveUpdater = UCavrnusFunctionLibrary::BeginTransientGenericPropertyUpdate(InSpaceConn, InContainer, InProperty,PropVal);
			}

			if (const UWorld* World = InActor->GetWorld())
			{
				World->GetTimerManager().SetTimer(TransformUpdaterHandle, [this, UpdatedComponent]
				{
					UE_LOG(LogTemp, Log, TEXT("Transform updates have stopped. Running final logic..."));
					if (LiveUpdater)
					{
						Cavrnus::FPropertyValue PropVal = Cavrnus::FPropertyValue();
						PropVal.PropType = Cavrnus::FPropertyValue::PropertyType::Transform;
						PropVal.TransformValue = UpdatedComponent->GetRelativeTransform();
						LiveUpdater->FinalizeGeneric(PropVal);
						LiveUpdater = nullptr;
					}
				}, 0.5f, false);
			}
		});
	} else
		UE_LOG(LogTemp, Error, TEXT("InActor is null!"));
}

void UCavrnusDataSmithTransformSync::BeginDestroy()
{
	UObject::BeginDestroy();
	TargetActor = nullptr;
}