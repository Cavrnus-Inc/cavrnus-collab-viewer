// Copyright(c) Cavrnus. All rights reserved.
/**
 * @file CavrnusSpaceConnection.h
 * @brief Declaration of the FCavrnusSpaceConnection struct, which is one of the most important objects in Cavrnus.  
 * It represents a user’s connection to a Space.  As such it is used to access Properties, hook Users, etc.
 * 
 */

#pragma once

#include "CoreMinimal.h"

#include "CavrnusSpaceConnection.generated.h"		// Always last

/**
 * @brief Represents a connection to a Cavrnus Space.
 *
 * This struct holds information about the connection to a specific space in Cavrnus,
 * including the connection ID, user connection ID, and container name.
 */
USTRUCT(BlueprintType)
struct CAVRNUSCONNECTOR_API FCavrnusSpaceConnection
{
	GENERATED_BODY()

	/** Unique identifier for the space connection */
	UPROPERTY(BlueprintReadWrite, Category = "Cavrnus|Spaces")
	int SpaceConnectionId = -1;

	/** Identifier for the local user's connection */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus|Spaces")
	FString LocalUserConnectionId = "";

	/** Name of the container associated with the local user */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus|Spaces")
	FString LocalUserContainerName = "";

	/**
	 * @brief Default constructor.
	 *
	 * Initializes the space connection with default values.
	 */
	FCavrnusSpaceConnection() = default;

	/**
	 * @brief Constructor with space connection ID.
	 *
	 * Initializes the space connection with the specified connection ID.
	 *
	 * @param SpaceConnectionId Unique identifier for the space connection.
	 */
	FCavrnusSpaceConnection(int SpaceConnectionId)
		: SpaceConnectionId(SpaceConnectionId)
	{
	}

	/**
	 * @brief Constructor with full initialization.
	 *
	 * Initializes the space connection with the specified connection ID, user connection ID, and container name.
	 *
	 * @param InSpaceConnectionId Unique identifier for the space connection.
	 * @param InLocalUserConnectionId Identifier for the local user's connection.
	 * @param InLocalUserContainerName Name of the container associated with the local user.
	 */
	FCavrnusSpaceConnection(int InSpaceConnectionId, const FString& InLocalUserConnectionId, const FString& InLocalUserContainerName)
		: SpaceConnectionId(InSpaceConnectionId), LocalUserConnectionId(InLocalUserConnectionId), LocalUserContainerName(InLocalUserContainerName)
	{
	}
};