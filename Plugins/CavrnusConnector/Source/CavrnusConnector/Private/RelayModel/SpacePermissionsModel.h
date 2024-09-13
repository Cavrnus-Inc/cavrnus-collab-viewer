// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include <Containers/Map.h>
#include "../../Public/Types/CavrnusBinding.h"
#include "../../Public/Types/CavrnusCallbackTypes.h"

namespace Cavrnus
{
	class SpacePermissionsModel
	{
	public:
		SpacePermissionsModel();
		virtual ~SpacePermissionsModel();

		void UpdatePolicyAllowed(FString policy, bool allowed);

		UCavrnusBinding* BindPolicyAllowed(FString policy, CavrnusPolicyUpdated callback);

	private:
		TMap<FString, bool> CurrPolicyAllowedValues;

		TMap<FString, TArray<CavrnusPolicyUpdated*>> PolicyBindings;
	};

} // namespace CavrnusRelay