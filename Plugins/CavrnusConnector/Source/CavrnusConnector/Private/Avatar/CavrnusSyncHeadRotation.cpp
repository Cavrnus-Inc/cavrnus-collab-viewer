// Copyright(c) Cavrnus. All rights reserved.

#include "Avatar/CavrnusSyncHeadRotation.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include <GameFramework/Pawn.h>

#include "CavrnusFunctionLibrary.h"
#include "CavrnusPropertiesContainer.h"

#include <Types/CavrnusCallbackTypes.h>
#include <Types/CavrnusSpaceConnection.h>
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LivePropertyUpdates/CavrnusLiveVectorPropertyUpdate.h"

UCavrnusSyncHeadRotation::UCavrnusSyncHeadRotation()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCavrnusSyncHeadRotation::BeginPlay()
{
	Super::BeginPlay();
	
	this->SetComponentTickEnabled(false);
	
	if (AActor* Owner = this->GetOwner())
	{
		APawn* Pawn = Cast<APawn>(Owner);
		if (Pawn)
		{
			bIsLocalPawn = Pawn->IsLocallyControlled();
		}
	}
	CavrnusSpaceConnected spaceCallback = [this](const FCavrnusSpaceConnection& SpaceConn)
	{
		SpaceConnected(SpaceConn);
	};
	UCavrnusFunctionLibrary::AwaitAnySpaceConnection(spaceCallback);
}

void UCavrnusSyncHeadRotation::FrameDelay()
{
	if (const AActor* Owner = this->GetOwner())
	{
		if (Container.ContainerName.IsEmpty())
		{
			UCavrnusPropertiesContainer* ContainerFound = Owner->FindComponentByClass<UCavrnusPropertiesContainer>();
			if (ContainerFound)
			{
				Container.ContainerName = ContainerFound->ContainerName;
			}
		}
	}
	if (!bIsLocalPawn)
	{
		SetupRemotePlayer();
	}
	
	SetComponentTickEnabled(true);
}

void UCavrnusSyncHeadRotation::SpaceConnected(FCavrnusSpaceConnection SpaceConn)
{
	SpaceConnection = SpaceConn;

	FTimerHandle FrameDelayHandle;
	this->GetOwner()->GetWorldTimerManager().SetTimer(FrameDelayHandle, this, &UCavrnusSyncHeadRotation::FrameDelay, 0.1f, false);
}

void UCavrnusSyncHeadRotation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsLocalPawn)
	{
		if (AActor* Owner = this->GetOwner())
		{
			if (APawn* Pawn = Cast<APawn>(Owner))
			{
				// FRotator is (pitch, yaw, roll)
				HeadRotation = Pawn->GetControlRotation();
				const FVector HeadRotationVector = HeadRotation.Euler();

				if (!ValueUpdater) 
				{
					const FString PropertyName("HeadRotation");
					ValueUpdater = UCavrnusFunctionLibrary::BeginTransientVectorPropertyUpdate(SpaceConnection, Container, PropertyName, HeadRotationVector);
				}
				else 
				{
					ValueUpdater->UpdateWithNewData(HeadRotationVector);
				}
			}
		}
	}
	else
	{
		UpdateHeadLookAt();
	}
}

void UCavrnusSyncHeadRotation::BeginDestroy()
{
	Super::BeginDestroy();
	
	ValueUpdater = nullptr;
	AnimAccessor = nullptr;
}

void UCavrnusSyncHeadRotation::UpdateHeadLookAt()
{
	if (AnimAccessor)
	{
		AnimAccessor->SetHeadRotation(HeadRotation);
	}
}

void UCavrnusSyncHeadRotation::SetupRemotePlayer()
{
	if (const auto AnimComponent = GetOwner()->GetComponentByClass(UCavrnusPawnAnimationAccessor::StaticClass()))
	{
		AnimAccessor = Cast<UCavrnusPawnAnimationAccessor>(AnimComponent);
	} else
	{
		UE_LOG(LogCavrnusConnector, Log, TEXT("UNABLE TO FIND ANIMATION ACCESSOR!!!"));
		return;
	}
	
	const CavrnusVectorFunction PropertyUpdated = [this](const FVector& val, const FString& ContainerName, const FString& PropertyName)
		{
			HeadRotation = FRotator::MakeFromEuler(val);
		};

	const FString PropertyName("HeadRotation");
	UCavrnusFunctionLibrary::BindVectorPropertyValue(SpaceConnection, Container, PropertyName, PropertyUpdated);
}
