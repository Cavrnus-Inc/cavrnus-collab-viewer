// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include <Containers/Map.h>
#include "Types/CavrnusBinding.h"
#include "Types/CavrnusCallbackTypes.h"
#include "Translation/CavrnusProtoTranslation.h"
#include "DataState.h"

namespace ServerData
{
	class RelayRemoteMessage;
}

namespace Cavrnus
{
	class CavrnusRelayModel;

	class RelayCallbackModel
	{
	public:
		RelayCallbackModel(CavrnusRelayModel* model);
		virtual ~RelayCallbackModel();

		void HandleServerCallback(int callbackId, const ServerData::RelayRemoteMessage& msg);

		int RegisterLoginPasswordCallback(CavrnusAuthRecv onSuccess, CavrnusError onFailure);
		int RegisterLoginGuestCallback(CavrnusAuthRecv onSuccess, CavrnusError onFailure);

		void RegisterAuthCallback(CavrnusAuthRecv onAuth);

		void RegisterBeginLoadingSpaceCallback(CavrnusSpaceBeginLoading onBeginLoading);
		void HandleSpaceBeginLoading(FString spaceId);

		int RegisterCreateSpaceCallback(CavrnusSpaceCreated onCreated, CavrnusError onFailure);
		int RegisterJoinSpaceCallback(CavrnusSpaceConnected onConnected, CavrnusError onFailure);

		int RegisterFetchAvailableSpacesCallback(CavrnusAllSpacesInfoEvent onAllSpacesArrived);

		int RegisterFetchAudioInputs(CavrnusAvailableInputDevices onRecvDevices);
		int RegisterFetchAudioOutputs(CavrnusAvailableOutputDevices onRecvDevices);
		int RegisterFetchVideoInputs(CavrnusAvailableVideoInputDevices onRecvDevices);

		int RegisterFetchAllAvailableContent(CavrnusRemoteContentFunction onfetchedContent);

		int RegisterUploadContent(CavrnusUploadCompleteFunction onUploadComplete);

	private:
		CavrnusRelayModel* relayModel;

		int currReqId = 0;

		TArray<CavrnusAuthRecv*> AuthCallbacks;
		void HandleAuthRecv(FCavrnusAuthentication auth);

		TMap<int, CavrnusAuthRecv*> LoginPasswordSuccessCallbacks;
		TMap<int, CavrnusError*> LoginPasswordErrorCallbacks;
		void HandleLoginPasswordResponse(int callbackId, ServerData::AuthenticateResp resp);

		TMap<int, CavrnusAuthRecv*> LoginGuestSuccessCallbacks;
		TMap<int, CavrnusError*> LoginGuestErrorCallbacks;
		void HandleLoginGuestResponse(int callbackId, ServerData::AuthenticateGuestResp resp);

		TArray<CavrnusSpaceBeginLoading*> BeginLoadingSpaceCallbacks;

		TMap<int, CavrnusSpaceCreated*> CreateSpaceSuccessCallbacks;
		TMap<int, CavrnusError*> CreateSpaceErrorCallbacks;
		void HandleCreateSpaceResponse(int callbackId, ServerData::CreateSpaceResp resp);
		
		TMap<int, CavrnusSpaceConnected*> JoinSpaceSuccessCallbacks;
		TMap<int, CavrnusError*> JoinSpaceErrorCallbacks;
		void HandleJoinSpaceResponse(int callbackId, ServerData::JoinSpaceFromIdResp resp);

		TMap<int, CavrnusAllSpacesInfoEvent*> AllSpacesInfoCallbacks;
		void HandleJoinableSpacesRecv(int callbackId, ServerData::AllJoinableSpacesResp resp);

		TMap<int, CavrnusAvailableInputDevices*> FetchAudioInputsCallbacks;
		void HandleAudioInputsResponse(int callbackId, ServerData::GetAudioInputDevicesResp resp);

		TMap<int, CavrnusAvailableOutputDevices*> FetchAudioOutputsCallbacks;
		void HandleAudioOutputsResponse(int callbackId, ServerData::GetAudioOutputDevicesResp resp);

		TMap<int, CavrnusAvailableVideoInputDevices*> FetchVideoInputsCallbacks;
		void HandleVideoInputsResponse(int callbackId, ServerData::GetVideoInputDevicesResp resp);

		TMap<int, CavrnusRemoteContentFunction*> AllRemoteContentCallbacks;
		void HandleAllRemoteContentRecv(int callbackId, ServerData::FetchAllUploadedContentResp resp);

		TMap<int, CavrnusUploadCompleteFunction*> AllUploadContentCallbacks;
		void HandleUploadComplete(int callbackId, ServerData::UploadLocalFileResp resp);
	};

}