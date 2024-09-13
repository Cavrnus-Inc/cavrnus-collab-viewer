// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include <Containers/Map.h>
#include "../../Public/Types/CavrnusBinding.h"
#include "../../Public/Types/CavrnusCallbackTypes.h"

namespace Cavrnus
{
	class DataState
	{
	public:
		DataState();
		virtual ~DataState();

		FCavrnusAuthentication* CurrentAuthentication = nullptr;

		TArray<FCavrnusSpaceConnection> GetCurrentSpaceConnections();

		void AddSpaceConnection(const FCavrnusSpaceConnection& spaceConnection);

		void AwaitAnySpaceConnection(const CavrnusSpaceConnected& onConnected);

		void AddJoinableSpace(FCavrnusSpaceInfo space);
		void UpdateJoinableSpace(FCavrnusSpaceInfo space);
		void RemoveJoinableSpace(FCavrnusSpaceInfo space);

		UCavrnusBinding* BindJoinableSpaces(CavrnusSpaceInfoEvent spaceAdded, CavrnusSpaceInfoEvent spaceUpdated, CavrnusSpaceInfoEvent spaceRemoved);

	private:
		TArray<FCavrnusSpaceConnection> CurrentSpaceConnections;

		TArray<CavrnusSpaceConnected*> spaceConnectionBindings;

		TArray<FCavrnusSpaceInfo> CurrJoinableSpaces;

		TArray<CavrnusSpaceInfoEvent*> JoinableSpaceAddedBindings;
		TArray<CavrnusSpaceInfoEvent*> JoinableSpaceUpdatedBindings;
		TArray<CavrnusSpaceInfoEvent*> JoinableSpaceRemovedBindings;
	};

} // namespace CavrnusRelay