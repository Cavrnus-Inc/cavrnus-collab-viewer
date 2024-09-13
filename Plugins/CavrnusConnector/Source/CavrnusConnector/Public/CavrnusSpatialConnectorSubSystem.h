// Copyright(c) Cavrnus. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include <Blueprint/UserWidget.h>
#include <Engine/GameInstance.h>
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/Object.h"

#include "Types/CavrnusAuthentication.h"
#include "Types/CavrnusCallbackTypes.h"

#include "CavrnusFunctionLibrary.h"

#include "CavrnusSpatialConnectorSubSystem.generated.h"

class UCavrnusWidgetBase;
class ACavrnusSpatialConnector;
class AController;
class APawn;
class SpawnedObjectsManager;
class UCavrnusAvatarManager;
class UPDFManager;
class USpawnObjectHelpers;

/**
 * @brief Proxy class for interacting with the Cavrnus Spatial Connector subsystem.
 */
UCLASS(BlueprintType, Blueprintable)
class CAVRNUSCONNECTOR_API UCavrnusSpatialConnectorSubSystemProxy : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief UI Manager class for handling Cavrnus UI widgets.
	 */
	class UIManager
	{
	public:
		UIManager();

		/**
		 * @brief Initializes the UI manager with the provided spatial connector.
		 * @param Connector Pointer to the spatial connector.
		 */
		void Initialize(ACavrnusSpatialConnector* Connector);

		/**
		 * @brief Spawns a widget of the specified class.
		 * @param WidgetClass The class of the widget to spawn.
		 * @return A pointer to the spawned widget.
		 */
		UUserWidget* SpawnWidget(TSubclassOf<UUserWidget> WidgetClass);

		/**
		 * @brief Removes the specified widget.
		 * @param Widget Pointer to the widget to remove.
		 */
		void RemoveWidget(UUserWidget* Widget);

		/**
		 * @brief Removes all widgets managed by the UI manager.
		 */
		void RemoveAllWidgets();

		/**
		 * @brief Shows or hides the authentication widget.
		 * @param bShowWidget Boolean to show or hide the widget.
		 */
		void ShowAuthWidget(bool bShowWidget);

		/**
		 * @brief Shows the guest login widget.
		 * @param SuccessDelegate Delegate for successful authentication.
		 * @param FailureDelegate Delegate for authentication failure.
		 */
		void ShowGuestLoginWidget(CavrnusAuthRecv SuccessDelegate, CavrnusError FailureDelegate);

		/**
		 * @brief Shows the member login widget.
		 * @param SuccessDelegate Delegate for successful authentication.
		 * @param FailureDelegate Delegate for authentication failure.
		 */
		void ShowLoginWidget(CavrnusAuthRecv SuccessDelegate, CavrnusError FailureDelegate);

		/**
		 * @brief Shows or hides the loading widget.
		 * @param bShowWidget Boolean to show or hide the widget.
		 */
		void ShowLoadingWidget(bool bShowWidget);

		/**
		 * @brief Shows the space list widget.
		 */
		void ShowSpaceList();

		/**
		 * @brief Gets the current spatial connector.
		 * @return A pointer to the current spatial connector.
		 */
		ACavrnusSpatialConnector* GetConnector();

		/** Array of weak pointers to Cavrnus widgets. */
		TArray<TWeakObjectPtr<UUserWidget>> CavrnusWidgets;

	private:
		/** Pointer to the world context. */
		UWorld* World = nullptr;
		/** Weak pointer to the loading widget. */
		TWeakObjectPtr<UUserWidget> LoadingWidget;
		/** Weak pointer to the authentication widget. */
		TWeakObjectPtr<UUserWidget> AuthWidget;
		/** Weak pointer to the current spatial connector. */
		TWeakObjectPtr<ACavrnusSpatialConnector> CurrentCavrnusSpatialConnector;
	};
	/**
	 * @brief Gets the UI manager instance.
	 * @return Pointer to the UI manager instance.
	 */
	UIManager* GetUIManager();

public:
	UCavrnusSpatialConnectorSubSystemProxy();
	virtual ~UCavrnusSpatialConnectorSubSystemProxy();

	void Initialize();
	void Deinitialize();
	
	/**
	 * @brief Sets the owner of the objects created within the core. If not set, default is transient package.
	 * @param Owner Pointer to the owner object.
	 */
	void SetObjectOwner(UObject* Owner);

	/**
	 * @brief Sets the game instance for PIE or standalone mode. Requirement is to get the first local player. Editor mode provides its own local player.
	 * @param GameInstance Pointer to the game instance.
	 */
	void SetGameInstance(UGameInstance* GameInstance);

	/**
	 * @brief Registers the spatial connector.
	 * @param CavrnusSpatialConnector Pointer to the spatial connector.
	 */
	void RegisterCavrnusSpatialConnector(ACavrnusSpatialConnector* CavrnusSpatialConnector);

	/**
	 * @brief Gets the current spatial connector.
	 * @return Pointer to the current spatial connector.
	 */
	UFUNCTION(BlueprintCallable, Category = "Cavrnus")
	ACavrnusSpatialConnector* GetCavrnusSpatialConnector() const;

	/**
	 * @brief Authenticates and joins a space.
	 */
	void AuthenticateAndJoin();

	/**
	 * @brief Attempts to join a space with the specified ID.
	 * @param JoinSpaceId The ID of the space to join.
	 */
	void AttemptToJoinSpace(FString JoinSpaceId);

	/**
	 * @brief Called on authentication success.
	 * @param Authentication Authentication details.
	 */
	UFUNCTION()
	void OnAuthSuccess(FCavrnusAuthentication Authentication);

	/**
	 * @brief Called on authentication failure.
	 * @param Error Error message.
	 */
	UFUNCTION()
	void OnAuthFailure(FString Error);

	/**
	 * @brief Called on space connection success.
	 * @param SpaceConnection Space connection details.
	 */
	UFUNCTION()
	void OnSpaceConnectionSuccess(FCavrnusSpaceConnection SpaceConnection);

	/**
	 * @brief Called on space connection failure.
	 * @param Error Error message.
	 */
	UFUNCTION()
	void OnSpaceConnectionFailure(FString Error);

	/**
	 * @brief Initializes the Cavrnus actor.
	 * @param CavrnusActor Pointer to the Cavrnus actor.
	 */
	void InitializeCavrnusActor(AActor* CavrnusActor);

private:
	/**
	 * @brief Called when the pawn controller changes.
	 * @param InPawn Pointer to the new pawn.
	 * @param InController Pointer to the new controller.
	 */
	UFUNCTION()
	void OnPawnControllerChanged(APawn* InPawn, AController* InController);

	/**
	 * @brief Called when the possessed pawn changes.
	 * @param OldPawn Pointer to the old pawn.
	 * @param NewPawn Pointer to the new pawn.
	 */
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	/**
	 * @brief Sets up the local user pawn.
	 */
	UFUNCTION()
	void SetupLocalUserPawn();

private:
	/** Pointer to the UI manager instance. */
	UIManager* UIManagerInstance;
	/** Weak pointer to the current spatial connector. */
	TWeakObjectPtr<ACavrnusSpatialConnector> CurrentCavrnusSpatialConnector;
	
	/** Delegate for authentication success. */
	CavrnusAuthRecv AuthSuccess;
	/** Delegate for authentication failure. */
	CavrnusError AuthFailure;
	/** Delegate for space connection success. */
	CavrnusSpaceConnected SpaceConnectionSuccess;
	/** Delegate for space connection failure. */
	CavrnusError SpaceConnectionFailure;

	/** Property for authentication details. */
	UPROPERTY()
	FCavrnusAuthentication Authentication;

	/** Boolean indicating if there is a space connection. */
	bool hasSpaceConn;
	/** Property for space connection details. */
	FCavrnusSpaceConnection SpaceConn;

	/** Pointer to the spawn object helpers. */
	UPROPERTY()
	USpawnObjectHelpers* SpawnHelpers;

	/** Pointer to the spawned objects manager. */
	SpawnedObjectsManager* SpawnManager;

	/** Pointer to the avatar manager. */
	UPROPERTY()
	UCavrnusAvatarManager* AvatarManager;

	/** Weak pointer to the game instance. */
	TWeakObjectPtr<UGameInstance> GameInstance;
	/** Weak pointer to the object owner. */
	TWeakObjectPtr<UObject> ObjectOwner;

public:
	/** Boolean indicating if the subsystem is in editor mode. */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus")
	bool bInEditorMode = false;
};

/**
 * @brief Subsystem class for managing the lifecycle of the Cavrnus Spatial Connector.
 */
UCLASS(BlueprintType, Blueprintable)
class CAVRNUSCONNECTOR_API UCavrnusSpatialConnectorSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Constructor
	UCavrnusSpatialConnectorSubSystem();

	// Destructor
	~UCavrnusSpatialConnectorSubSystem();

	/**
	 * @brief Initializes the subsystem.
	 * @param Collection The subsystem collection base.
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection);
	 /**
     * @brief Deinitializes the subsystem.
     */
	virtual void Deinitialize();

	/**
	 * @brief Gets the subsystem proxy.
	 * @return Pointer to the subsystem proxy.
	 */
	UFUNCTION(BlueprintCallable, Category = "Cavrnus")
	UCavrnusSpatialConnectorSubSystemProxy* GetSystemProxy();

private:
	/** Pointer to the SubSystem Proxy. */
	UPROPERTY()
	UCavrnusSpatialConnectorSubSystemProxy* SystemProxy;
};