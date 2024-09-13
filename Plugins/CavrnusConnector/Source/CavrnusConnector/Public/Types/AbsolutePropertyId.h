// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include <Containers/Map.h>
#include "Types/CavrnusBinding.h"
#include "Types/PropertiesContainer.h"

#include "AbsolutePropertyId.generated.h"

/**
 * @brief Structure representing the ID of a property within the Cavrnus system.
 *
 * The FFullPropertyId structure is used to uniquely identify properties within a Cavrnus container.
 */
USTRUCT()
struct CAVRNUSCONNECTOR_API FAbsolutePropertyId
{
	GENERATED_BODY()

public:
	/** The name of the container that holds the property. */
	FString ContainerName = "";
	/** The ID of the property value. */
	FString PropValueId = "";

	/**
	 * @brief Default constructor for FFullPropertyId.
	 *
	 * Initializes the ContainerName and PropValueId to empty strings.
	 */
	FAbsolutePropertyId()
	{
		PropValueId = "";
		ContainerName = "";
	}
	/**
	 * @brief Constructor to initialize FFullPropertyId from a full name string.
	 *
	 * @param fullName The full name string containing both container and property IDs.
	 */
	FAbsolutePropertyId(FString fullName)
	{
		if (fullName.StartsWith("/"))
			fullName.RemoveFromStart("/");
		if (fullName.EndsWith("/"))
			fullName.RemoveFromEnd("/");

		PropValueId = "";
		for (int i = fullName.Len() - 1; i >= 0; i--)
		{
			if (fullName[i] == '/')
			{
				PropValueId = PropValueId.Reverse();
				break;
			}
			PropValueId.AppendChar(fullName[i]);
		}

		ContainerName = fullName;
		ContainerName.RemoveFromEnd("/" + PropValueId);
	}

	/**
	 * @brief Constructor to initialize FFullPropertyId with container name and property ID.
	 *
	 * @param containerName The name of the container.
	 * @param propId The ID of the property value.
	 */
	FAbsolutePropertyId(FString containerName, FString propId)
	{
		PropValueId = propId;
		ContainerName = containerName;

		//Clean up & standardize the ContainerName
		if (ContainerName.StartsWith("/"))
			ContainerName.RemoveFromStart("/");
		if (ContainerName.EndsWith("/"))
			ContainerName.RemoveFromEnd("/");
	}

	/**
	 * @brief Constructor to initialize FFullPropertyId with container name and property ID.
	 *
	 * @param containerName The name of the container.
	 * @param propId The ID of the property value.
	 */
	FAbsolutePropertyId(FPropertiesContainer containerName, FString propId)
	{
		PropValueId = propId;
		ContainerName = containerName.ContainerName;

		//Clean up & standardize the ContainerName
		if (ContainerName.StartsWith("/"))
			ContainerName.RemoveFromStart("/");
		if (ContainerName.EndsWith("/"))
			ContainerName.RemoveFromEnd("/");
	}

	/** Default Destructor for FFullPropertyId. */
	~FAbsolutePropertyId(){}

	/**
	 * @brief Combines the container name and property ID into a single string.
	 *
	 * @param pId The property ID structure.
	 * @return The combined name string.
	 */
	static const FString GetCombinedName(FAbsolutePropertyId pId)
	{
		return pId.ContainerName + "/" + pId.PropValueId;
	}

	/**
	 * @brief Gets the hash value of the property ID.
	 *
	 * @param propertyId The property ID structure.
	 * @return The hash value.
	 */
	friend uint32 GetTypeHash(const FAbsolutePropertyId& propertyId)
	{
		return HashCombine(GetTypeHash(propertyId.ContainerName), GetTypeHash(propertyId.PropValueId));
	}

	/**
	 * @brief Equality operator for FFullPropertyId.
	 *
	 * @param other The other property ID to compare with.
	 * @return True if the property IDs are equal, false otherwise.
	 */
	bool operator==(const FAbsolutePropertyId& other) const
	{
		return ContainerName.Equals(other.ContainerName, ESearchCase::CaseSensitive) && PropValueId.Equals(other.PropValueId, ESearchCase::CaseSensitive);
	}

	/**
	 * @brief Convenience function to return FString type without calling static function GetCombinedName
	 *
	 * @return The Absolute Property Id as an FString
	 */
	FString ToString() const
	{
		return(FAbsolutePropertyId::GetCombinedName(*this));
	}
};
