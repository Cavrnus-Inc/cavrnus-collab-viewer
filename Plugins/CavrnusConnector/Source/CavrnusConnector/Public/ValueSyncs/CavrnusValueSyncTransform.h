// Copyright(c) Cavrnus. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CavrnusValueSyncBase.h"
#include <CavrnusFunctionLibrary.h>
#include "Types\CavrnusCallbackTypes.h"

#include "CavrnusValueSyncTransform.generated.h"

/**
 * @brief Class responsible for synchronizing Transform properties between Unreal Engine and Cavrnus.
 * This class extends UCavrnusValueSyncBase and implements specific functionality to handle Transform vectors.
 */
UCLASS(BlueprintType, Blueprintable)
class CAVRNUSCONNECTOR_API UCavrnusValueSyncTransform : public UCavrnusValueSyncBase
{
	GENERATED_BODY()

public:
	// Default Constructor
	UCavrnusValueSyncTransform();

	// Destructor
	~UCavrnusValueSyncTransform();

	/**
	 * @brief Blueprint-callable getter function for the Transform property value.
	 * @return The current Transform value.
	 */
	UFUNCTION(BlueprintNativeEvent)
	FTransform GetTransform();
	virtual FTransform GetTransform_Implementation() { return FTransform(); }

	/**
	 * @brief Blueprint-callable setter function for the Transform property value.
	 * @param PropertyValue The new Transform value to set.
	 */
	UFUNCTION(BlueprintNativeEvent)
	void SetTransform(FTransform PropertyValue);
	virtual void SetTransform_Implementation(FTransform PropertyValue) { }

	/**
	 * @brief Explicit getter for the Transform property value.
	 * @return The current property value.
	 */
	Cavrnus::FPropertyValue GetPropertyValue() override;

	/**
	 * @brief Explicit setter for the Transform property value.
	 * @return The current property value.
	 */
	void SetPropertyValue(Cavrnus::FPropertyValue value) override;
};