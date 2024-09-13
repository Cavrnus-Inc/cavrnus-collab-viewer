// Copyright(c) Cavrnus. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CavrnusValueSyncBase.h"
#include <CavrnusFunctionLibrary.h>
#include "Types\CavrnusCallbackTypes.h"

#include "CavrnusValueSyncBoolean.generated.h"

/**
 * @brief Class responsible for synchronizing a boolean property between Cavrnus and Unreal Engine.
 *
 * The UCavrnusValueSyncBoolean class provides mechanisms to get and set a boolean property value,
 * and ensures that these values are synchronized with the Cavrnus platform.
 */
UCLASS(BlueprintType, Blueprintable)
class CAVRNUSCONNECTOR_API UCavrnusValueSyncBoolean : public UCavrnusValueSyncBase
{
	GENERATED_BODY()

public:
	// Default Constructor
	UCavrnusValueSyncBoolean();

	// Destructor
	~UCavrnusValueSyncBoolean();

	/**
	 * @brief Gets the current boolean property value.
	 * @return The current boolean property value.
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool GetBoolean();
	virtual bool GetBoolean_Implementation() { return false; }

	/**
	 * @brief Sets the boolean property value.
	 * @param bPropertyValue The new boolean property value.
	 */
	UFUNCTION(BlueprintNativeEvent)
	void SetBoolean(bool bPropertyValue);
	virtual void SetBoolean_Implementation(bool bPropertyValue) {}

	/**
	 * @brief Explicit getter for the current property value as a Cavrnus property.
	 * @return The current Cavrnus property value.
	 */
	Cavrnus::FPropertyValue GetPropertyValue() override;
	/**
	 * @brief Sets the property value as a Cavrnus property.
	 * @param value The new Cavrnus property value.
	 */
	void SetPropertyValue(Cavrnus::FPropertyValue value) override;
};