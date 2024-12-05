// Copyright (c) 2024 Cavrnus. All rights reserved.

#pragma once

#include <Containers/Map.h>
#include "../../Public/Types/CavrnusBinding.h"
#include "../../Public/Types/CavrnusCallbackTypes.h"

namespace Cavrnus
{
	class CavrnusDataCache
	{
	public:
		CavrnusDataCache();
		virtual ~CavrnusDataCache();

		void Setup(TArray<FString> keys, TArray<FString> vals);

		FString GetStringValue(FString key);
		void SetStringValue(FString key, FString val);

	private:
		TMap<FString, FString> StringValues;
	};
} // namespace Cavrnus
