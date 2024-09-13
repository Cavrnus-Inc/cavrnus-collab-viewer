// Copyright(c) Cavrnus. All rights reserved.
/**
 * @file CavrnusCallbackTypes.h
 * @brief This file defines various callback types used in the Cavrnus Spatial Connector plugin for Unreal Engine 5.
 *
 * The CavrnusCallbackTypes file contains type definitions for various callbacks used within the Cavrnus plugin.
 * These callback types handle different events and actions, such as authentication, space connections, property updates,
 * and user interactions. The callbacks are used to facilitate asynchronous operations and event-driven programming
 * within the plugin.
 */
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include <Containers/Map.h>
#include <Delegates/Delegate.h>
#include "Types/CavrnusInputDevice.h"
#include "Types/CavrnusOutputDevice.h"
#include "Types/CavrnusVideoInputDevice.h"
#include "Types/CavrnusAuthentication.h"
#include "Types/CavrnusSpaceConnection.h"
#include "Types/CavrnusSpaceInfo.h"
#include "Types/CavrnusUser.h"
#include "Types/CavrnusBinding.h"
#include "Types/CavrnusSpawnedObject.h"
#include "Types/CavrnusPropertyValue.h"
#include "Types/ChatEntry.h"
#include "CavrnusRemoteContent.h"

#include "CavrnusCallbackTypes.generated.h"		// Always last

 /**
  * @brief Callback type for handling errors.
  *
  * @param FString The error message.
  */
typedef TFunction<void(const FString&)> CavrnusError;

/**
 * @brief Callback type for handling authentication events.
 *
 * @param FCavrnusAuthentication The authentication data.
 */
typedef TFunction<void(const FCavrnusAuthentication&)> CavrnusAuthRecv;

/**
 * @brief Callback type for handling events with all spaces information.
 *
 * @param TArray<FCavrnusSpaceInfo> Array of space information.
 */
typedef TFunction<void(const TArray<FCavrnusSpaceInfo>&)> CavrnusAllSpacesInfoEvent;

/**
 * @brief Callback type for handling events with a single space information.
 *
 * @param FCavrnusSpaceInfo The space information.
 */
typedef TFunction<void(const FCavrnusSpaceInfo&)> CavrnusSpaceInfoEvent;

/**
 * @brief Callback type for handling space begin loading events.
 *
 * @param FString The ID of the space being loaded.
 */
typedef TFunction<void(const FString&)> CavrnusSpaceBeginLoading;

/**
 * @brief Callback type for handling space connected events.
 *
 * @param FCavrnusSpaceConnection The space connection data.
 */
typedef TFunction<void(const FCavrnusSpaceConnection&)> CavrnusSpaceConnected;

typedef TFunction<void(const FCavrnusSpaceInfo&)> CavrnusSpaceCreated;

/**
 * @brief Callback type for handling property updates.
 *
 * @param Cavrnus::FPropertyValue The updated property value.
 * @param FString The container name.
 * @param FString The property name.
 */
typedef TFunction<void(const Cavrnus::FPropertyValue&, const FString&, const FString&)> CavrnusPropertyFunction;

/**
 * @brief Callback type for handling string property updates.
 *
 * @param FString The updated string value.
 * @param FString The container name.
 * @param FString The property name.
 */
typedef TFunction<void(const FString&, const FString&, const FString&)> CavrnusStringFunction;

/**
 * @brief Callback type for handling boolean property updates.
 *
 * @param bool The updated boolean value.
 * @param FString The container name.
 * @param FString The property name.
 */
typedef TFunction<void(bool, const FString&, const FString&)> CavrnusBoolFunction;

/**
 * @brief Callback type for handling float property updates.
 *
 * @param float The updated float value.
 * @param FString The container name.
 * @param FString The property name.
 */
typedef TFunction<void(float, const FString&, const FString&)> CavrnusFloatFunction;

/**
 * @brief Callback type for handling color property updates.
 *
 * @param FLinearColor The updated color value.
 * @param FString The container name.
 * @param FString The property name.
 */
typedef TFunction<void(const FLinearColor&, const FString&, const FString&)> CavrnusColorFunction;

/**
 * @brief Callback type for handling vector property updates.
 *
 * @param FVector4 The updated vector value.
 * @param FString The container name.
 * @param FString The property name.
 */
typedef TFunction<void(const FVector4&, const FString&, const FString&)> CavrnusVectorFunction;

/**
 * @brief Callback type for handling transform property updates.
 *
 * @param FTransform The updated transform value.
 * @param FString The container name.
 * @param FString The property name.
 */
typedef TFunction<void(const FTransform&, const FString&, const FString&)> CavrnusTransformFunction;

/**
 * @brief Callback type for handling space user events.
 *
 * @param FCavrnusUser The user data.
 */
typedef TFunction<void(const FCavrnusUser&)> CavrnusSpaceUserEvent;

/**
 * @brief Callback type for handling available input devices.
 *
 * @param TArray<FCavrnusInputDevice> Array of available input devices.
 */
typedef TFunction<void(const TArray<FCavrnusInputDevice>&)> CavrnusAvailableInputDevices;

/**
 * @brief Callback type for handling available output devices.
 *
 * @param TArray<FCavrnusOutputDevice> Array of available output devices.
 */
typedef TFunction<void(const TArray<FCavrnusOutputDevice>&)> CavrnusAvailableOutputDevices;

/**
 * @brief Callback type for handling available video input devices.
 *
 * @param TArray<FCavrnusVideoInputDevice> Array of available video input devices.
 */
typedef TFunction<void(const TArray<FCavrnusVideoInputDevice>&)> CavrnusAvailableVideoInputDevices;

/**
 * @brief Callback type for handling saved input device.
 *
 * @param FCavrnusInputDevice The saved input device.
 */
typedef TFunction<void(const FCavrnusInputDevice&)> CavrnusSavedInputDevice;

/**
 * @brief Callback type for handling saved output device.
 *
 * @param FCavrnusOutputDevice The saved output device.
 */
typedef TFunction<void(const FCavrnusOutputDevice&)> CavrnusSavedOutputDevice;

/**
 * @brief Callback type for handling video frame updates.
 *
 * @param UTexture2D* The updated video frame.
 */
typedef TFunction<void(UTexture2D*)> VideoFrameUpdateFunction;

/**
 * @brief Callback type for handling policy updates.
 *
 * @param FString The policy name.
 * @param bool Whether the policy is allowed.
 */
typedef TFunction<void(const FString&, bool)> CavrnusPolicyUpdated;

/**
 * @brief Callback type for handling content progress updates.
 *
 * @param float The progress percentage.
 * @param FString The current step.
 */
typedef TFunction<void(float, const FString&)> CavrnusContentProgressFunction;

/**
 * @brief Callback type for handling content functions.
 *
 * @param TArray<uint8> The content data.
 */
typedef TFunction<void(const TArray<uint8>&, const FString&)> CavrnusContentFunction;

/**
 * @brief Callback type for handling remote content functions.
 *
 * @param TArray<FCavrnusRemoteContent> Array of remote content data.
 */
typedef TFunction<void(const TArray<FCavrnusRemoteContent>&)> CavrnusRemoteContentFunction;

/**
 * @brief Callback type for handling upload complete events.
 *
 * @param FCavrnusRemoteContent The uploaded content data.
 */
typedef TFunction<void(const FCavrnusRemoteContent&)> CavrnusUploadCompleteFunction;

typedef TFunction<void(FChatEntry)> CavrnusChatFunction;
typedef TFunction<void(FString)> CavrnusChatRemovedFunction;

/**
 * @brief Abstract class providing blueprint function library for Cavrnus callback types.
 */
UCLASS(Abstract)
class UCavrnusCallbackTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

private:

};

