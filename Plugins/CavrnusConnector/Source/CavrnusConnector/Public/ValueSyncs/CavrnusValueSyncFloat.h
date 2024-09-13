// Copyright(c) Cavrnus. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CavrnusValueSyncBase.h"
#include <CavrnusFunctionLibrary.h>
#include "Types\CavrnusCallbackTypes.h"

#include "CavrnusValueSyncFloat.generated.h"

/**
 * @brief Class responsible for synchronizing float properties between Unreal Engine and Cavrnus.
 * This class extends UCavrnusValueSyncBase and implements specific functionality to handle float values.
 */
UCLASS(BlueprintType, Blueprintable)
class CAVRNUSCONNECTOR_API UCavrnusValueSyncFloat : public UCavrnusValueSyncBase
{
	GENERATED_BODY()

public:
	// Default Constructor
	UCavrnusValueSyncFloat();

	// Destructor
	~UCavrnusValueSyncFloat();

	/**
	 * @brief Blueprint-callable getter function for the float property value.
	 * @param PropertyValue The new float value to set.
	 */
	UFUNCTION(BlueprintNativeEvent)
	float GetFloat();
	virtual float GetFloat_Implementation() { return 0.0; }

	/**
	 * @brief Blueprint-callable setter function for the float property value.
	 * @param PropertyValue The new float value to set.
	 */
	UFUNCTION(BlueprintNativeEvent)
	void SetFloat(float PropertyValue);
	virtual void SetFloat_Implementation(float PropertyValue) {};
	
	/**
	 * @brief Explicit getter for the float property value.
	 * @return The current property value.
	 */
	Cavrnus::FPropertyValue GetPropertyValue() override;
	/**
	 * @brief Explicit setter for the float property value.
	 * @return The current property value.
	 */
	void SetPropertyValue(Cavrnus::FPropertyValue value) override;
};