// Copyright(c) Cavrnus. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Types/CavrnusBinding.h"
#include "Types/CavrnusPropertyValue.h"
#include "IPropertySyncInterface.generated.h"


/**
 * @brief Interface for synchronizing property values between Unreal Engine and the Cavrnus platform.
 * 
 * This interface defines the contract for getting and setting property values, ensuring consistent property synchronization.
 */
UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UPropertySyncInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief Interface class for property synchronization.
 *
 * This class provides pure virtual functions that must be implemented by any class that inherits from this interface.
 */
class CAVRNUSCONNECTOR_API IPropertySyncInterface
{
	GENERATED_BODY()

public:
	/**
	 * @brief Gets the current property value.
	 * This function must be implemented by any class inheriting this interface.
	 * 
	 * @return The current property value as a Cavrnus::FPropertyValue.
	 */
	virtual Cavrnus::FPropertyValue GetPropertyValue() = 0;
	/**
	 * @brief Sets a new property value.
	 * This function must be implemented by any class inheriting this interface.
	 * 
	 * @param value The new property value to set.
	 */
	virtual void SetPropertyValue(Cavrnus::FPropertyValue value) = 0;
};