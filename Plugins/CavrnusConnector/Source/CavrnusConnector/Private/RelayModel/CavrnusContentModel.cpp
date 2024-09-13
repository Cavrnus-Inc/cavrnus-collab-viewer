// Copyright(c) Cavrnus. All rights reserved.
#include "RelayModel/CavrnusContentModel.h"
#include <Misc/FileHelper.h>

namespace Cavrnus
{

	CavrnusContentModel::CavrnusContentModel()
	{
	}

	CavrnusContentModel::~CavrnusContentModel()
	{
		// Clean up ProgressBindings's callback arrays
		for (auto kvp : ProgressBindings)
		{
			for (auto callback : kvp.Value)
			{
				delete callback;
			}
			kvp.Value.Empty();
		}

		// Clean up CompletionBindings's callback arrays
		for (auto kvp : CompletionBindings)
		{
			for (auto callback : kvp.Value)
			{
				delete callback;
			}
			kvp.Value.Empty();
		}
	}

	void CavrnusContentModel::RegisterContentCallbacks(FString contentId, const CavrnusContentProgressFunction& prog, const CavrnusContentFunction& complete)
	{
		if (CompletedContentPaths.Contains(contentId))
		{
			complete(CompletionToStream(CompletedContentPaths[contentId]), CompletedContentFileNames[contentId]);
			return;
		}

		using progFunction = const CavrnusContentProgressFunction;
		progFunction* ProgCallbackPtr = new progFunction(prog);
		ProgressBindings.FindOrAdd(contentId);
		ProgressBindings[contentId].Add(ProgCallbackPtr);

		using completedFunction = const CavrnusContentFunction;
		completedFunction* CompletedCallbackPtr = new completedFunction(complete);
		CompletionBindings.FindOrAdd(contentId);
		CompletionBindings[contentId].Add(CompletedCallbackPtr);
	}

	void CavrnusContentModel::HandleProgressCallback(FString contentId, float progress, FString step)
	{
		if (!ProgressBindings.Contains(contentId))
			return;

		for (int i = 0; i < ProgressBindings[contentId].Num(); i++)
			(*ProgressBindings[contentId][i])(progress, step);
	}

	void CavrnusContentModel::HandleCompletionCallback(FString contentId, FString filePath, FString fileNameWithExt)
	{
		if (!CompletionBindings.Contains(contentId))
			return;

		// Call the CompletionBindings callbacks for this contentId now, and clean them up
		for (auto callback : CompletionBindings[contentId])
		{
			(*callback)(CompletionToStream(filePath), fileNameWithExt);
			delete callback;
		}
		CompletionBindings.Remove(contentId);

		// Also discard the ProgressBindings callbacks for this contentId
		for (auto callback : ProgressBindings[contentId])
		{
			delete callback;
		}
		ProgressBindings.Remove(contentId);

		CompletedContentPaths.Add(contentId, filePath);
		CompletedContentFileNames.Add(contentId, fileNameWithExt);
	}

	TArray<uint8> CavrnusContentModel::CompletionToStream(FString filePath)
	{
		TArray<uint8> res;
		FFileHelper::LoadFileToArray(res, *filePath, 0);

		return res;
	}
}