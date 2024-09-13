// Copyright(c) Cavrnus. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "CavrnusValueSyncBase.h"
#include "CavrnusFunctionLibrary.h"
#include "Types\CavrnusCallbackTypes.h"
#include <Components/ActorComponent.h>
#include "CavrnusValueSyncVector.generated.h"

/**
 * @brief Class responsible for synchronizing vector properties between Unreal Engine and Cavrnus.
 * This class extends UCavrnusValueSyncBase and implements specific functionality to handle generic vectors.
 * For purpose-specific vectors, such as Color and Transform, see the specific Value-Sync classes for those properties.
 */
UCLASS(BlueprintType, Blueprintable)
class CAVRNUSCONNECTOR_API UCavrnusValueSyncVector : public UCavrnusValueSyncBase
{
	GENERATED_BODY()

public:
	// Constructor
	UCavrnusValueSyncVector();

	// Destructor
	~UCavrnusValueSyncVector();

	/**
	 * @brief Blueprint-callable getter function for a generic vector property.
	 * @return The current vector.
	 */
	UFUNCTION(BlueprintNativeEvent)
	FVector GetVector();
	virtual FVector GetVector_Implementation() { return FVector(); }

	/**
	 * @brief Blueprint-callable setter function for the generic vector property.
	 * @param PropertyValue The new vector to set.
	 */
	UFUNCTION(BlueprintNativeEvent)
	void SetVector(FVector PropertyValue);
	virtual void SetVector_Implementation(FVector PropertyValue) {}

	/**
	 * @brief Explicit getter for the generic vector property.
	 * @return The current vector.
	 */
	Cavrnus::FPropertyValue GetPropertyValue() override;

	/**
	 * @brief Explicit setter for the generic vector property.
	 * @return The current vector.
	 */
	void SetPropertyValue(Cavrnus::FPropertyValue value) override;
};