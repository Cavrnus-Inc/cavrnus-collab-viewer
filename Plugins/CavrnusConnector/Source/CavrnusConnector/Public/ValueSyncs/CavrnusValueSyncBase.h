// Copyright(c) Cavrnus. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "IPropertySyncInterface.h"
#include "Types/CavrnusBinding.h"
#include "Types/CavrnusSpaceConnection.h"
#include "../Private/RelayModel/CavrnusVirtualPropertyUpdate.h"

#include "CavrnusValueSyncBase.generated.h"

class UCavrnusPropertiesContainer;
class UCavrnusLivePropertyUpdate;

/**
 * @brief Base class for synchronizing property values between Cavrnus and Unreal Engine.
 *
 * UCavrnusValueSyncBase is an abstract class derived from USceneComponent and implements the IPropertySyncInterface.
 * It provides mechanisms for property synchronization, including polling for property changes, sending updates,
 * and handling space connections.
 */
UCLASS(BlueprintType, HideCategories=(Transform, Physics, Collision, Rendering), Abstract)
class CAVRNUSCONNECTOR_API UCavrnusValueSyncBase : public USceneComponent, public IPropertySyncInterface
{
	GENERATED_BODY()

public:
	// Default Constructor
	UCavrnusValueSyncBase();

	// Destructor
	~UCavrnusValueSyncBase();

	void BeginPlay();
	void EndPlay(const EEndPlayReason::Type Reason);
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Called when the component is created */
	void OnComponentCreated() override;
	/** Automatically attaches a properties container to the component */
	void AutoAttachPropertiesContainer();
	
	/**
	 * @brief Generates and returns the name of the properties container.
	 * @return The generated container name.
	 */
	FString GetGeneratedContainerName() const;
	
	/** The name of the property to synchronize. Exposed to the editor when spawned */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cavrnus", meta = (ExposeOnSpawn))
	FString PropertyName = "";

	/** Indicates whether changes to the property should be sent. Exposed to the editor and spawnable */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cavrnus", meta = (ExposeOnSpawn))
	bool SendChanges = true;

	//TODO: Custom Inspector like CSC to hide this???
	/** Indicates whether the initial setup is complete. Exposed to the editor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cavrnus")
	bool InitialSetupComplete = false;

	/** Indicates whether changes to the property should be received */
	bool RecvChanges = true;

	/**
	 * @brief Retrieves the current value of the property.
	 * @return The current property value.
	 */
	virtual Cavrnus::FPropertyValue GetPropertyValue()PURE_VIRTUAL(IPropertySyncInterface::GetPropertyValue, return Cavrnus::FPropertyValue(););
	
	/**
	 * @brief Sets the value of the property.
	 * @param value The new property value.
	 */
	virtual void SetPropertyValue(Cavrnus::FPropertyValue value)PURE_VIRTUAL(IPropertySyncInterface::SetPropertyValue);

private:
	/**
	 * @brief Determines whether a properties container should be automatically added.
	 * @return True if a properties container should be automatically added, false otherwise.
	 */
	bool ShouldAutoAddPropertiesContainer() const;

	/** Pointer to handle live property updates */
	Cavrnus::CavrnusVirtualPropertyUpdate* liveUpdater = nullptr;
	/** Stores the last sent property value */
	Cavrnus::FPropertyValue lastSentPropValue;

	/**
	 * @brief Setter for space connection.
	 * @param spaceConn The space connection.
	 */
	void SpaceConnected(FCavrnusSpaceConnection spaceConn);

	/** Polls for changes in the property value */
	void PollForPropertyChanges();
	/** Attempts to send changes in the property value */
	void TrySendPropertyChanges();

	/** The interval for polling property changes */
	static const float PollingTimeSeconds;
	/** Time remaining until the next poll */
	float PollingTimeRemaining;
	/** Time to wait before posting changes */
	static const double SecondsToWaitBeforePosting;
	/** Indicates whether synchronization should occur */
	bool shouldSync = false;
	/** Represents the connection to the Cavrnus space */
	FCavrnusSpaceConnection SpaceConn;
	
	/** Pointer to manage property binding */
	UPROPERTY()
	UCavrnusBinding* PropertyBinding = nullptr;

	/**
	 * @brief Getter for owner name. 
	 * @return The name of the owner, or "No owner" if none.
	 */
	FString ReportOwnerName() const;

	/**
	 * @brief Retrieves the properties container.
	 * @return The properties container.
	 */
	UCavrnusPropertiesContainer* GetContainer() const;
	/**
	 * @brief Retrieves the name of the properties container.
	 * @return The properties container name.
	 */
	FString GetContainerName() const;
};