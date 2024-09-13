// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include <Containers/Map.h>
#include "../../Public/Types/CavrnusBinding.h"
#include "../../Public/Types/CavrnusCallbackTypes.h"
#include "SpacePropertyModel.h"
#include <fstream>

namespace Cavrnus
{
	class CavrnusContentModel
	{
	public:
		CavrnusContentModel();
		virtual ~CavrnusContentModel();

		void RegisterContentCallbacks(FString contentId, const CavrnusContentProgressFunction& prog, const CavrnusContentFunction& complete);

		void HandleProgressCallback(FString contentId, float progress, FString step);
		void HandleCompletionCallback(FString contentId, FString filePath, FString fileNameWithExt);

	private:

		CaseSensitiveMap<FString, FString> CompletedContentPaths;
		CaseSensitiveMap<FString, FString> CompletedContentFileNames;

		CaseSensitiveMap<FString, TArray<const CavrnusContentProgressFunction*>> ProgressBindings;
		CaseSensitiveMap<FString, TArray<const CavrnusContentFunction*>> CompletionBindings;

		TArray<uint8> CompletionToStream(FString filePath);
	};

} // namespace CavrnusRelay