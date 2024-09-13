// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include <Containers/Map.h>
#include "../../Public/Types/CavrnusBinding.h"
#include "../../Public/Types/CavrnusCallbackTypes.h"
#include "../../Public/Types/ChatEntry.h"
#include "../../Public/Types/AbsolutePropertyId.h"

namespace Cavrnus
{
	class SpaceChatModel
	{
	public:
		SpaceChatModel();
		virtual ~SpaceChatModel();

		void AddChat(FChatEntry chat);
		void UpdateChat(FChatEntry chat);
		void RemoveChat(FString chatId);

		UCavrnusBinding* BindChatEvents(CavrnusChatFunction chatAdded, CavrnusChatFunction chatUpdated, CavrnusChatRemovedFunction chatRemoved);

	private:
		TMap<FAbsolutePropertyId, FChatEntry> CurrChatEntries;

		TArray<CavrnusChatFunction*> ChatAddedBindings;
		TArray<CavrnusChatFunction*> ChatUpdatedBindings;
		TArray<CavrnusChatRemovedFunction*> ChatRemovedBindings;
	};

} // namespace CavrnusRelay