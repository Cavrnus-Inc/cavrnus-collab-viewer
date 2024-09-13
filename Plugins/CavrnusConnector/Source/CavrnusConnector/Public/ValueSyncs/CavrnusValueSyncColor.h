// Copyright(c) Cavrnus. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CavrnusValueSyncBase.h"
#include <CavrnusFunctionLibrary.h>
#include "Types\CavrnusCallbackTypes.h"

#include "CavrnusValueSyncColor.generated.h"

/**
 * @brief Class responsible for synchronizing color properties between Unreal Engine and Cavrnus.
 * This class extends UCavrnusValueSyncBase and implements specific functionality to handle color values.
 */
UCLASS(BlueprintType, Blueprintable)
class CAVRNUSCONNECTOR_API UCavrnusValueSyncColor : public UCavrnusValueSyncBase
{
	GENERATED_BODY()

public:
	// Default Constructor
	UCavrnusValueSyncColor();

	// Destructor
	~UCavrnusValueSyncColor();

	/**
	 * @brief Blueprint-callable getter function for the color property value.
	 * @param PropertyValue The new color value to set.
	 */
	UFUNCTION(BlueprintNativeEvent)
	FLinearColor GetColor();
	virtual FLinearColor GetColor_Implementation() { return FLinearColor::Black; }

	/**
	 * @brief Blueprint-callable setter function for the color property value.
	 * @param PropertyValue The new color value to set.
	 */
	UFUNCTION(BlueprintNativeEvent)
	void SetColor(FLinearColor PropertyValue);
	virtual void SetColor_Implementation(FLinearColor PropertyValue) {};

	/**
	 * @brief Explicit getter for the color property value.
	 * @return The current property value.
	 */
	Cavrnus::FPropertyValue GetPropertyValue() override;

	/**
	 * @brief Explicit setter for the color property value.
	 * @return The current property value.
	 */
	void SetPropertyValue(Cavrnus::FPropertyValue value) override;
};