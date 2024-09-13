// Copyright(c) Cavrnus. All rights reserved.
#include "ValueSyncs/CavrnusValueSyncBase.h"
#include "CavrnusPropertiesContainer.h"
#include "LivePropertyUpdates/CavrnusLivePropertyUpdate.h"
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <Kismet/KismetSystemLibrary.h>
#include <TimerManager.h>
#include <CavrnusFunctionLibrary.h>
#include "RelayModel/CavrnusRelayModel.h"

const float UCavrnusValueSyncBase::PollingTimeSeconds = 0.1f;
const double UCavrnusValueSyncBase::SecondsToWaitBeforePosting = 0.2f;

//===========================================================
UCavrnusValueSyncBase::UCavrnusValueSyncBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}


//===========================================================
UCavrnusValueSyncBase::~UCavrnusValueSyncBase()
{
}

//===========================================================

void UCavrnusValueSyncBase::BeginPlay()
{
	Super::BeginPlay();

	CavrnusSpaceConnected spaceCallback = [this](const FCavrnusSpaceConnection& SpaceConn)
	{
		SpaceConnected(SpaceConn);
	};
	UCavrnusFunctionLibrary::AwaitAnySpaceConnection(spaceCallback);
}

void UCavrnusValueSyncBase::SpaceConnected(FCavrnusSpaceConnection SpaceConnection)
{
	SpaceConn = SpaceConnection;
}

void UCavrnusValueSyncBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	shouldSync = false;

	if (liveUpdater != nullptr)
		liveUpdater->Finalize();
	liveUpdater = nullptr;

	if(PropertyBinding)
		PropertyBinding->Unbind();
}

void UCavrnusValueSyncBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (SpaceConn.SpaceConnectionId == -1)
		return;

	// This can't go in SpaceConnected, cuz the container state is still invalid during component regen at that point.
	// We need to wait for a valid state to start polling/bindinge :P
	if (!shouldSync) 
	{
		if (!GetContainer() || GetContainerName().IsEmpty())
		{
			// This can be hit during start or if Unreal rebuilds the component tree
			return;
		}

		shouldSync = true;

		if (InitialSetupComplete && !(UCavrnusFunctionLibrary::GetGenericPropertyValue(SpaceConn, GetContainerName(), PropertyName) == GetPropertyValue()))
		{
			UCavrnusFunctionLibrary::PostGenericPropertyUpdate(SpaceConn, GetContainerName(), PropertyName, GetPropertyValue());
		}

		UCavrnusFunctionLibrary::DefineGenericPropertyDefaultValue(SpaceConn, GetContainerName(), PropertyName, GetPropertyValue(), false);
		if (RecvChanges)
		{
			CavrnusPropertyFunction propUpdateCallback = [this](const Cavrnus::FPropertyValue& Value, const FString& ContainerName, const FString& PropertyName)
			{
				if (liveUpdater == nullptr)
					SetPropertyValue(Value);
			};

			PropertyBinding = UCavrnusFunctionLibrary::BindGenericPropertyValue(SpaceConn, GetContainerName(), PropertyName, propUpdateCallback);
		}
		else
		{
			PropertyBinding = nullptr;
		}

		InitialSetupComplete = true;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!shouldSync || !SendChanges)
		return;

	if (PollingTimeRemaining > DeltaTime)
	{
		PollingTimeRemaining -= DeltaTime;
		return;
	}

	PollingTimeRemaining = PollingTimeSeconds;

	if (GIsReconstructingBlueprintInstances || GIsReinstancing)
		return;

	TrySendPropertyChanges();
}

void UCavrnusValueSyncBase::TrySendPropertyChanges()
{
	Cavrnus::FPropertyValue localValue = GetPropertyValue();
	Cavrnus::FPropertyValue serverValue = UCavrnusFunctionLibrary::GetGenericPropertyValue(SpaceConn, GetContainerName(), PropertyName);

	if (liveUpdater != nullptr)
		serverValue = lastSentPropValue;

	if (serverValue == localValue)
	{
		if (liveUpdater == nullptr)
			return;

		double GameTimeSeconds = FPlatformTime::Seconds();
		if (GameTimeSeconds <= liveUpdater->lastUpdatedTimeSec + SecondsToWaitBeforePosting)
			return;

		if (!GetContainerName().StartsWith("users/"))
		{
			lastSentPropValue = localValue;
			liveUpdater->Finalize(localValue);
			liveUpdater = nullptr;
		}
	}
	else
	{
		lastSentPropValue = localValue;

		if (liveUpdater == nullptr)
		{
			liveUpdater = new Cavrnus::CavrnusVirtualPropertyUpdate(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConn, FAbsolutePropertyId(GetContainerName(), PropertyName), lastSentPropValue);
		}
		else
		{
			liveUpdater->UpdateWithNewData(lastSentPropValue);
		}
	}
}

UCavrnusPropertiesContainer* UCavrnusValueSyncBase::GetContainer() const
{
	/*
		Search up the hierarchy towards the root to find a
		container component
	*/
	UCavrnusPropertiesContainer* ContainerComponent = nullptr;
	USceneComponent* Parent = GetAttachParent();
	while (Parent != nullptr && !ContainerComponent)
	{
		TArray<USceneComponent*> Children;
		Parent->GetChildrenComponents(false, Children);
		for (auto Child : Children)
		{
			if (Child->IsA(UCavrnusPropertiesContainer::StaticClass()))
			{
				ContainerComponent = Cast<UCavrnusPropertiesContainer>(Child);
				break;
			}
		}

		Parent = Parent->GetAttachParent();
	}

	// If no container found then try to get one from anywhere on the actor
	if (!ContainerComponent)
	{
		if (AActor* Owner = GetOwner())
		{
			if (UActorComponent* FoundComponent = Owner->GetComponentByClass(UCavrnusPropertiesContainer::StaticClass()))
			{
				ContainerComponent = Cast<UCavrnusPropertiesContainer>(FoundComponent);
			}
		}
	}

	return ContainerComponent;
}

FString UCavrnusValueSyncBase::GetContainerName() const
{
	return GetContainer()->ContainerName;
}

FString UCavrnusValueSyncBase::ReportOwnerName() const
{
	FString OwnerName = TEXT("No Owner");
	if (AActor* Owner = GetOwner())
	{
		OwnerName = Owner->GetName();
	}

	return OwnerName;
}



#pragma region EditorScript

// Overridden to automatically force the owning actor to
// have a UCavrnusPropertiesContainer at the same level as the
// UCavrnusValueSyncBase component. Called when adding an instance component from the details
// panel in the level editor. Overriding OnComponentCreated() rather than OnAttachmentChanged()
// so that the component is attached to its parent and the parent's AttachChildren are synced.
void UCavrnusValueSyncBase::OnComponentCreated()
{
	Super::OnComponentCreated();

	AutoAttachPropertiesContainer();
}

void UCavrnusValueSyncBase::AutoAttachPropertiesContainer()
{
	if (ShouldAutoAddPropertiesContainer())
	{
		AActor* Owner = GetOwner();
		UCavrnusPropertiesContainer* PropertiesContainer = Cast<UCavrnusPropertiesContainer>(Owner->AddComponentByClass(
			UCavrnusPropertiesContainer::StaticClass(),
			true,
			FTransform::Identity,
			false));
		if (PropertiesContainer)
		{
			PropertiesContainer->ContainerName = GetGeneratedContainerName();
			PropertiesContainer->AttachToComponent(GetAttachParent(), FAttachmentTransformRules::KeepRelativeTransform);
			Owner->AddInstanceComponent(PropertiesContainer);
		}
	}
}

bool UCavrnusValueSyncBase::ShouldAutoAddPropertiesContainer() const
{
	if (USceneComponent* Parent = GetAttachParent())
	{
		if (AActor* Owner = Parent->GetOwner())
		{
			if (Owner->IsTemplate())
				return false;

			if (UWorld* World = Owner->GetWorld())
			{
				if (World->bIsTearingDown)
					return false;

				TArray<USceneComponent*> ParentChildren;
				Parent->GetChildrenComponents(false, ParentChildren);

				// Need to call at later point if this is the case
				if (!ParentChildren.Contains(this))
					return false;

				return ParentChildren.FilterByPredicate([](USceneComponent* SceneComponent) {
					return SceneComponent->IsA<UCavrnusPropertiesContainer>();
				}).IsEmpty();
			}
		}
	}

	return false;
}

FString UCavrnusValueSyncBase::GetGeneratedContainerName() const
{
	USceneComponent* CurrentComponent = GetAttachParent();
	FString PropertiesContainerName;

	do
	{
		FString ObjectName = CurrentComponent->GetName();
		if (PropertiesContainerName.IsEmpty())
		{
			PropertiesContainerName = ObjectName;
		}
		else
		{
			PropertiesContainerName = ObjectName + "/" + PropertiesContainerName;
		}

		CurrentComponent = CurrentComponent->GetAttachParent();
	} while (CurrentComponent != nullptr);

	// Add owner prefix which will be replaced with the spawned object ID 
	// when the actor is spawned
	if (AActor* Owner = GetOwner())
	{
		FString ActorClassName = Owner->GetClass()->GetName();
		ActorClassName.RemoveFromEnd(TEXT("_C"));
		PropertiesContainerName = ActorClassName + "/" + PropertiesContainerName;
	}

	return PropertiesContainerName;
}

#pragma endregion

