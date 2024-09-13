// Copyright(c) Cavrnus. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CavrnusValueSyncBase.h"
#include <CavrnusFunctionLibrary.h>
#include "Types\CavrnusCallbackTypes.h"

#include "CavrnusValueSyncString.generated.h"

/**
 * @brief Class responsible for synchronizing string properties between Unreal Engine and Cavrnus.
 * This class extends UCavrnusValueSyncBase and implements specific functionality to handle string values.
 */
UCLASS(BlueprintType, Blueprintable)
class CAVRNUSCONNECTOR_API UCavrnusValueSyncString : public UCavrnusValueSyncBase
{
	GENERATED_BODY()

public:
	// Default Constructor
	UCavrnusValueSyncString();

	// Destructor
	~UCavrnusValueSyncString();

	/**
	 * @brief Blueprint-callable getter function for the string property value.
	 * @return The current string value.
	 */
	UFUNCTION(BlueprintNativeEvent)
	FString GetString();
	virtual FString GetString_Implementation() { return ""; }

	/**
	 * @brief Blueprint-callable setter function for the string property value.
	 * @param PropertyValue The new string value to set.
	 */
	UFUNCTION(BlueprintNativeEvent)
	void SetString(const FString& PropertyValue);
	virtual void SetString_Implementation(const FString& PropertyValue) {}

	/**
	 * @brief Explicit getter for the string property value.
	 * @return The current property value.
	 */
	Cavrnus::FPropertyValue GetPropertyValue() override;
	/**
	 * @brief Explicit setter for the string property value.
	 * @return The current property value.
	 */
	void SetPropertyValue(Cavrnus::FPropertyValue value) override;
};