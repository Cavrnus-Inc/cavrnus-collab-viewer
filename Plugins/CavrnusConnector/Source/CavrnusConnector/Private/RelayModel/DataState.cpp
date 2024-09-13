// Copyright(c) Cavrnus. All rights reserved.
#include "RelayModel/DataState.h"

namespace Cavrnus
{

	DataState::DataState()
	{
	}

	DataState::~DataState()
	{

	}

	TArray<FCavrnusSpaceConnection> DataState::GetCurrentSpaceConnections()
	{
		return CurrentSpaceConnections;
	}

	void DataState::AddSpaceConnection(const FCavrnusSpaceConnection& spaceConnection)
	{
		CurrentSpaceConnections.Add(spaceConnection);

		for (int i = 0; i < spaceConnectionBindings.Num(); i++)
		{
			(*spaceConnectionBindings[i])(spaceConnection);
		}
		spaceConnectionBindings.Empty();
	}

	void DataState::AwaitAnySpaceConnection(const CavrnusSpaceConnected& onConnected)
	{
		if (CurrentSpaceConnections.Num() > 0)
		{
			onConnected(CurrentSpaceConnections[0]);
		}
		else
		{
			CavrnusSpaceConnected* CallbackPtr = new CavrnusSpaceConnected(onConnected);
			spaceConnectionBindings.Add(CallbackPtr);
		}
	}

	void DataState::AddJoinableSpace(FCavrnusSpaceInfo space)
	{
		CurrJoinableSpaces.Add(space);

		for (int i = 0; i < JoinableSpaceAddedBindings.Num(); i++)
			(*JoinableSpaceAddedBindings[i])(space);
	}

	void DataState::UpdateJoinableSpace(FCavrnusSpaceInfo space)
	{
		int indexToRem = -1;
		for (int i = 0; i < CurrJoinableSpaces.Num(); i++) {
			if (CurrJoinableSpaces[i].SpaceId == space.SpaceId)
			{
				indexToRem = i;
				break;
			}
		}
		if (indexToRem != -1)
		{
			CurrJoinableSpaces.RemoveAt(indexToRem);
		}
		CurrJoinableSpaces.Add(space);

		for (int i = 0; i < JoinableSpaceUpdatedBindings.Num(); i++)
			(*JoinableSpaceUpdatedBindings[i])(space);
	}

	void DataState::RemoveJoinableSpace(FCavrnusSpaceInfo space)
	{
		CurrJoinableSpaces.Remove(space);

		for (int i = 0; i < JoinableSpaceRemovedBindings.Num(); i++)
			(*JoinableSpaceRemovedBindings[i])(space);
	}

	UCavrnusBinding* DataState::BindJoinableSpaces(CavrnusSpaceInfoEvent spaceAdded, CavrnusSpaceInfoEvent spaceUpdated, CavrnusSpaceInfoEvent spaceRemoved)
	{
		CavrnusSpaceInfoEvent* added = new CavrnusSpaceInfoEvent(spaceAdded);
		CavrnusSpaceInfoEvent* updated = new CavrnusSpaceInfoEvent(spaceUpdated);
		CavrnusSpaceInfoEvent* removed = new CavrnusSpaceInfoEvent(spaceRemoved);

		JoinableSpaceAddedBindings.Add(added);
		JoinableSpaceUpdatedBindings.Add(updated);
		JoinableSpaceRemovedBindings.Add(removed);

		for (int i = 0; i < CurrJoinableSpaces.Num(); i++)
			spaceAdded(CurrJoinableSpaces[i]);

		UCavrnusBinding* binding;
		binding = NewObject<UCavrnusBinding>();
		binding->SetupUnbind([this, added, updated, removed]()
		{
			JoinableSpaceAddedBindings.Remove(added);
			JoinableSpaceUpdatedBindings.Remove(updated);
			JoinableSpaceRemovedBindings.Remove(removed);
		});

		return binding;
	}

}