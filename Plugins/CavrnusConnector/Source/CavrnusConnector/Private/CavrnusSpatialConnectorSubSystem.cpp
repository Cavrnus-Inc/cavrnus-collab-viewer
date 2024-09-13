// Copyright(c) Cavrnus. All rights reserved.
#include "CavrnusSpatialConnectorSubSystem.h"
#include "CavrnusFunctionLibrary.h"
#include "CavrnusSpatialConnector.h"
#include "SpawnedObjectsManager.h"
#include "CavrnusAvatarManager.h"
#include "FlagComponents/CavrnusLocalUserFlag.h"
#include "Types\CavrnusUser.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "CavrnusPropertiesContainer.h"
#include "SpawnObjectHelpers.h"
#include "RelayModel\CavrnusRelayModel.h"

#include <GameFramework/Pawn.h>
#include <GameFramework/PlayerController.h>
#include <UObject/Package.h>

#if WITH_EDITOR
#include "LevelEditor.h"
#include "SLevelViewport.h"

class FLevelEditorModule;
class SLevelViewport;
#endif

UCavrnusSpatialConnectorSubSystemProxy::UIManager::UIManager() {}

void UCavrnusSpatialConnectorSubSystemProxy::UIManager::Initialize(ACavrnusSpatialConnector* Connector)
{
	CurrentCavrnusSpatialConnector = Connector;
	World = Connector->GetWorld();
}

UUserWidget* UCavrnusSpatialConnectorSubSystemProxy::UIManager::SpawnWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (UUserWidget* Widget = CreateWidget<UUserWidget>(World, WidgetClass))
	{
		CavrnusWidgets.Add(Widget);

		if (UCavrnusSpatialConnectorSubSystemProxy* SubProxy = UCavrnusFunctionLibrary::GetCavrnusSpatialConnectorSubSystemProxy())
		{
			if (SubProxy->bInEditorMode)
			{
#if WITH_EDITOR
				Widget->SetFlags(RF_Transient);
				FLevelEditorModule& LevelEditorModule = FModuleManager::Get().GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
				TSharedPtr<SLevelViewport> GameViewport = LevelEditorModule.GetFirstActiveLevelViewport();
				TSharedPtr<SOverlay> ViewportOverlay = StaticCastSharedPtr<SOverlay>(GameViewport->GetViewportWidget().Pin()->GetContent());
				GameViewport->AddOverlayWidget(Widget->TakeWidget());
#endif
			}
			else
			{
				Widget->AddToViewport();
			}

			return Widget;
		}
	}

	return nullptr;
}

void UCavrnusSpatialConnectorSubSystemProxy::UIManager::RemoveWidget(UUserWidget* Widget)
{
	if (Widget)
	{
		if (UCavrnusSpatialConnectorSubSystemProxy* SubProxy = UCavrnusFunctionLibrary::GetCavrnusSpatialConnectorSubSystemProxy())
		{
			if (SubProxy->bInEditorMode)
			{
#if WITH_EDITOR
				FLevelEditorModule& LevelEditorModule = FModuleManager::Get().GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
				TSharedPtr<SLevelViewport> GameViewport = LevelEditorModule.GetFirstActiveLevelViewport();
				if (GameViewport.IsValid())
				{
					GameViewport->RemoveOverlayWidget(Widget->TakeWidget());
				}
#endif
			}
			else
			{
				Widget->RemoveFromParent();
			}
		}

		CavrnusWidgets.Remove(Widget);
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::UIManager::RemoveAllWidgets()
{
	for (int i = CavrnusWidgets.Num() - 1; i > -1; i--)
	{
		if (UUserWidget* Widget = CavrnusWidgets[i].Get())
		{
			RemoveWidget(Widget);
		}
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::UIManager::ShowAuthWidget(bool bShowWidget)
{
	if (bShowWidget)
	{
		ACavrnusSpatialConnector* CavrnusSpatialConnector = GetConnector();
		if (CavrnusSpatialConnector->AuthenticationWidgetClass)
		{
			AuthWidget = SpawnWidget(CavrnusSpatialConnector->AuthenticationWidgetClass);
		}
	}
	else
	{
		RemoveWidget(AuthWidget.Get());
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::UIManager::ShowGuestLoginWidget(CavrnusAuthRecv AuthSuccess, CavrnusError AuthFailure)
{
	if (UCavrnusGuestLoginWidget* LoginWidget = CreateWidget<UCavrnusGuestLoginWidget>(World, CurrentCavrnusSpatialConnector->GuestJoinMenu))
	{

		LoginWidget->OnLogin.AddLambda
		(
			[this, LoginWidget, AuthSuccess, AuthFailure](FString GuestLoginUsername)
			{
				RemoveWidget(LoginWidget);
				ShowAuthWidget(true);
				UCavrnusFunctionLibrary::AuthenticateAsGuest(CurrentCavrnusSpatialConnector->MyServer, GuestLoginUsername, AuthSuccess, AuthFailure);
			}
		);

		LoginWidget->AddToViewport();
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::UIManager::ShowLoginWidget(CavrnusAuthRecv AuthSuccess, CavrnusError AuthFailure)
{
	ACavrnusSpatialConnector* CavrnusSpatialConnector = GetConnector();

	// Spawn a widget and get credentials from that
	if (UCavrnusLoginWidget* LoginWidget = Cast<UCavrnusLoginWidget>(SpawnWidget(CavrnusSpatialConnector->MemberLoginMenu)))
	{
		LoginWidget->OnLogin.AddLambda
		(
			[this, LoginWidget, CavrnusSpatialConnector, AuthSuccess, AuthFailure](FString LoginEmail, FString LoginPassword)
			{
				RemoveWidget(LoginWidget);
				ShowAuthWidget(true);
				UCavrnusFunctionLibrary::AuthenticateWithPassword(CavrnusSpatialConnector->MyServer, LoginEmail, LoginPassword, AuthSuccess, AuthFailure);
			}
		);
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::UIManager::ShowLoadingWidget(bool bShowWidget)
{
	if (bShowWidget)
	{
		ACavrnusSpatialConnector* CavrnusSpatialConnector = GetConnector();
		if (CavrnusSpatialConnector->LoadingWidgetClass)
		{
			LoadingWidget = SpawnWidget(CavrnusSpatialConnector->LoadingWidgetClass);
		}
	}
	else
	{
		RemoveWidget(LoadingWidget.Get());
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::UIManager::ShowSpaceList()
{
	ACavrnusSpatialConnector* CavrnusSpatialConnector = GetConnector();
	if (UCavrnusSpaceListWidget* SpaceListWidget = Cast<UCavrnusSpaceListWidget>(SpawnWidget(CavrnusSpatialConnector->SpaceJoinMenu)))
	{
		SpaceListWidget->SpaceSelected = [this, SpaceListWidget](const FCavrnusSpaceInfo& SpaceInfo)
		{
			RemoveWidget(SpaceListWidget);
			if (UCavrnusSpatialConnectorSubSystemProxy* SubProxy =UCavrnusFunctionLibrary::GetCavrnusSpatialConnectorSubSystemProxy())
			{
				SubProxy->AttemptToJoinSpace(SpaceInfo.SpaceId);
			}
		};
	}
}

ACavrnusSpatialConnector* UCavrnusSpatialConnectorSubSystemProxy::UIManager::GetConnector()
{
	return CurrentCavrnusSpatialConnector.Get();
}

UCavrnusSpatialConnectorSubSystemProxy::UIManager* UCavrnusSpatialConnectorSubSystemProxy::GetUIManager()
{
	return UIManagerInstance;
}

UCavrnusSpatialConnectorSubSystemProxy::UCavrnusSpatialConnectorSubSystemProxy()
{
	ObjectOwner = Cast<UObject>(GetTransientPackage());
}

UCavrnusSpatialConnectorSubSystemProxy::~UCavrnusSpatialConnectorSubSystemProxy()
{
	AvatarManager = nullptr;
	SpawnManager = nullptr;
	hasSpaceConn = false;
}

void UCavrnusSpatialConnectorSubSystemProxy::Initialize()
{
	UIManagerInstance = new UIManager();

	AuthSuccess = [this](const FCavrnusAuthentication& auth)
	{
		OnAuthSuccess(auth);
	};
	AuthFailure = [this](const FString& failure)
	{
		OnAuthFailure(failure);
	};
	SpaceConnectionSuccess = [this](const FCavrnusSpaceConnection& spaceConn) 
	{
		OnSpaceConnectionSuccess(spaceConn);
	};
	SpaceConnectionFailure = [this](const FString& failure)
	{
		OnSpaceConnectionFailure(failure);
	};

	SpawnHelpers = NewObject<USpawnObjectHelpers>();

	SpawnManager = new SpawnedObjectsManager();

	TFunction<AActor* (FCavrnusSpawnedObject, FString)> onObjectCreation = [this](const FCavrnusSpawnedObject& ob, FString uniqueId)
	{
		AActor* actor = nullptr;

		if (!GetCavrnusSpatialConnector()->SpawnableIdentifiers.Contains(uniqueId))
		{
			UE_LOG(LogCavrnusConnector, Error, TEXT("Could not find spawnable object with Unique ID %s in the Cavrnus Spatial Connector"), *uniqueId);
			return actor;
		}
		return SpawnManager->RegisterSpawnedObject(ob, GetCavrnusSpatialConnector()->SpawnableIdentifiers[uniqueId], GetWorld(), SpawnHelpers);
	};
	Cavrnus::CavrnusRelayModel::GetDataModel()->RegisterObjectCreationCallback(onObjectCreation);

	TFunction<void(FCavrnusSpawnedObject)> onObjectDestruction = [this](const FCavrnusSpawnedObject& ob)
	{
		SpawnManager->UnregisterSpawnedObject(ob, GetWorld());
	};
	Cavrnus::CavrnusRelayModel::GetDataModel()->RegisterObjectDestructionCallback(onObjectDestruction);
}

void UCavrnusSpatialConnectorSubSystemProxy::Deinitialize()
{
	AvatarManager = nullptr;
	SpawnManager = nullptr;
	hasSpaceConn = false;
	Cavrnus::CavrnusRelayModel::KillDataModel();
}

void UCavrnusSpatialConnectorSubSystemProxy::SetObjectOwner(UObject* Owner)
{
	ObjectOwner = Owner;
}

void UCavrnusSpatialConnectorSubSystemProxy::SetGameInstance(UGameInstance* GInstance)
{
	GameInstance = GInstance;
}

void UCavrnusSpatialConnectorSubSystemProxy::RegisterCavrnusSpatialConnector(ACavrnusSpatialConnector* CavrnusSpatialConnector)
{
	CurrentCavrnusSpatialConnector = CavrnusSpatialConnector;
	UIManagerInstance->Initialize(CavrnusSpatialConnector);
}

ACavrnusSpatialConnector* UCavrnusSpatialConnectorSubSystemProxy::GetCavrnusSpatialConnector() const
{
	return CurrentCavrnusSpatialConnector.Get();
}

void UCavrnusSpatialConnectorSubSystemProxy::AuthenticateAndJoin()
{
	ACavrnusSpatialConnector* CavrnusSpatialConnector = GetCavrnusSpatialConnector();
	if (!CavrnusSpatialConnector)
		return;
	
	// TO DO: Check if actually a valid token?
	if (Authentication.Token.IsEmpty())
	{
		if (CavrnusSpatialConnector->AuthMethod == ECavrnusAuthMethod::JoinAsGuest)
		{
			if (CavrnusSpatialConnector->GuestLoginMethod == ECavrnusGuestLoginMethod::EnterNameBelow)
			{
				// AUTH GRAPHIC SHOW
				UIManagerInstance->ShowAuthWidget(true);
				UCavrnusFunctionLibrary::AuthenticateAsGuest
				(
					CavrnusSpatialConnector->MyServer,
					CavrnusSpatialConnector->GuestName,
					AuthSuccess, AuthFailure
				);
			}
			else if (CavrnusSpatialConnector->GuestLoginMethod == ECavrnusGuestLoginMethod::PromptToEnterName)
			{
				UIManagerInstance->ShowGuestLoginWidget(AuthSuccess, AuthFailure);
			}
			else
			{
				UE_LOG(LogCavrnusConnector, Error, TEXT("No guest login method selected in CavrnusSpatialConnector actor!"));
			}
		}
		else if (CavrnusSpatialConnector->AuthMethod == ECavrnusAuthMethod::JoinAsMember)
		{
			if (CavrnusSpatialConnector->MemberLoginMethod == ECavrnusMemberLoginMethod::EnterMemberLoginCredentials)
			{
				UIManagerInstance->ShowAuthWidget(true);
				UCavrnusFunctionLibrary::AuthenticateWithPassword
				(
					CavrnusSpatialConnector->MyServer,
					CavrnusSpatialConnector->MemberLoginEmail,
					CavrnusSpatialConnector->MemberLoginPassword,
					AuthSuccess, AuthFailure
				);
			}
			else if (CavrnusSpatialConnector->MemberLoginMethod == ECavrnusMemberLoginMethod::PromptMemberToLogin)
			{
				UIManagerInstance->ShowLoginWidget(AuthSuccess, AuthFailure);
			}
			else
			{
				UE_LOG(LogCavrnusConnector, Error, TEXT("No member login method selected in CavrnusSpatialConnector actor!"));
			}
		}
		else
		{
			UE_LOG(LogCavrnusConnector, Error, TEXT("No authentication method selected in CavrnusSpatialConnector actor!"));
		}
	}
	else if (!hasSpaceConn) // Authenticated but not in a space
	{
		OnAuthSuccess(Authentication);
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::OnAuthSuccess(FCavrnusAuthentication Auth)
{
	UIManagerInstance->ShowAuthWidget(false);

	Authentication = Auth;
	ACavrnusSpatialConnector* CavrnusSpatialConnector = GetCavrnusSpatialConnector();
	UE_LOG(LogCavrnusConnector, Log, TEXT("Successfully authenticated"));
	if (CavrnusSpatialConnector->SpaceJoinMethod == ECavrnusSpaceJoinMethod::EnterSpaceId)
	{
		AttemptToJoinSpace(CavrnusSpatialConnector->AutomaticSpaceJoinId);
	}
	else if (CavrnusSpatialConnector->SpaceJoinMethod == ECavrnusSpaceJoinMethod::SpacesList)
	{
		UIManagerInstance->ShowSpaceList();
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::OnAuthFailure(FString ErrorMessage)
{
	UIManagerInstance->ShowAuthWidget(false);

	Authentication = FCavrnusAuthentication();
	ACavrnusSpatialConnector* CavrnusSpatialConnector = GetCavrnusSpatialConnector();
	UE_LOG(LogCavrnusConnector, Error, TEXT("Failed to authenticate, error: %s"), *ErrorMessage);
	if (CavrnusSpatialConnector->AuthMethod == ECavrnusAuthMethod::JoinAsMember)
	{
		UIManagerInstance->ShowLoginWidget(AuthSuccess, AuthFailure);
	}
	else if (CavrnusSpatialConnector->AuthMethod == ECavrnusAuthMethod::JoinAsGuest)
	{
		UIManagerInstance->ShowGuestLoginWidget(AuthSuccess, AuthFailure);
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::OnSpaceConnectionSuccess(FCavrnusSpaceConnection SpaceConnection)
{
	UIManagerInstance->ShowAuthWidget(false);

	AvatarManager = NewObject<UCavrnusAvatarManager>();
	CavrnusSpaceUserEvent userAdded = [this, SpaceConnection](const FCavrnusUser& user) {
		AvatarManager->RegisterUser(user, GetCavrnusSpatialConnector()->RemoteAvatarClass, GetWorld(), SpawnHelpers);
	};
	CavrnusSpaceUserEvent userRemoved = [this](const FCavrnusUser& user) {
		AvatarManager->UnregisterUser(user, GetWorld());
	};

	UCavrnusFunctionLibrary::BindSpaceUsers(SpaceConnection, userAdded, userRemoved);

	SpaceConn = SpaceConnection;
	hasSpaceConn = true;

	// Listen for future controller or pawn changes
	if (GameInstance.IsValid())
	{
		GameInstance->GetOnPawnControllerChanged().AddDynamic(this, &UCavrnusSpatialConnectorSubSystemProxy::OnPawnControllerChanged);
		if (APlayerController* PlayerController = GameInstance->GetFirstLocalPlayerController())
		{
			PlayerController->OnPossessedPawnChanged.AddDynamic(this, &UCavrnusSpatialConnectorSubSystemProxy::OnPossessedPawnChanged);
		}
	}

	SetupLocalUserPawn();

	ACavrnusSpatialConnector* CavrnusSpatialConnector = GetCavrnusSpatialConnector();
	UE_LOG(LogCavrnusConnector, Log, TEXT("Successfully joined space!"));

	UIManagerInstance->ShowLoadingWidget(false);

	for (TSubclassOf<UUserWidget>& Widget : CavrnusSpatialConnector->WidgetsToLoad)
	{
		UIManagerInstance->SpawnWidget(Widget);
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::OnPawnControllerChanged(APawn* InPawn, AController* InController)
{
	if (InController && InController->IsLocalPlayerController() && InPawn)
	{
		SetupLocalUserPawn();
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (NewPawn)
	{
		SetupLocalUserPawn();
	}

	if (OldPawn)
	{
		OldPawn->Destroy();
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::SetupLocalUserPawn()
{
	if (GameInstance == nullptr)
	{
		return;
	}

	APlayerController* PlayerController = GameInstance->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
	{
		return;
	}

	APawn* Pawn = PlayerController->GetPawn();
	if (Pawn && !Pawn->GetComponentByClass(UCavrnusLocalUserFlag::StaticClass()))
	{
		USceneComponent* PawnRootComponent = Pawn->GetRootComponent();
		ensureAlwaysMsgf(PawnRootComponent != nullptr, TEXT("No root component on pawn when attaching local user component"));
		UCavrnusLocalUserFlag* LocalUserComponent = Cast<UCavrnusLocalUserFlag>(Pawn->AddComponentByClass(
			UCavrnusLocalUserFlag::StaticClass(),
			true,
			FTransform::Identity,
			false));

		ensureAlwaysMsgf(LocalUserComponent != nullptr, TEXT("No local user component to attach"));
		LocalUserComponent->AttachToComponent(PawnRootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		LocalUserComponent->RegisterComponent();

		FString PropertyPath = TEXT("users/") + SpaceConn.LocalUserConnectionId;
		USpawnObjectHelpers::ResetLiveHierarchyRootName(Pawn, PropertyPath);

		CavrnusSpaceUserEvent evt = [LocalUserComponent](const FCavrnusUser& user) {
			LocalUserComponent->LocalUser = user;
		};
		
		UCavrnusFunctionLibrary::BeginTransientBoolPropertyUpdate(SpaceConn, PropertyPath, "AvatarVis", true);
		
		UCavrnusFunctionLibrary::AwaitLocalUser(SpaceConn, evt);
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::OnSpaceConnectionFailure(FString ErrorMessage)
{
	hasSpaceConn = false;

	UE_LOG(LogCavrnusConnector, Error, TEXT("Failed to join space, error: %s"), *ErrorMessage);
}

void UCavrnusSpatialConnectorSubSystemProxy::InitializeCavrnusActor(AActor* CavrnusActor)
{
	if (SpawnHelpers)
	{
		SpawnHelpers->InitializeCavrnusActor(CavrnusActor);
	}
	else
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("InitializeCavrnusActor called before USpawnObjectHelpers creation."));
	}
}

void UCavrnusSpatialConnectorSubSystemProxy::AttemptToJoinSpace(FString JoinSpaceId)
{
	UIManagerInstance->ShowLoadingWidget(true);

	UE_LOG(LogCavrnusConnector, Log, TEXT("Attempting to join space: %s"), *JoinSpaceId);
	UCavrnusFunctionLibrary::JoinSpace(JoinSpaceId, SpaceConnectionSuccess, SpaceConnectionFailure);
}

UCavrnusSpatialConnectorSubSystem::UCavrnusSpatialConnectorSubSystem()
{
	SystemProxy = nullptr;
}

UCavrnusSpatialConnectorSubSystem::~UCavrnusSpatialConnectorSubSystem()
{
	SystemProxy = nullptr;
}

void UCavrnusSpatialConnectorSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (SystemProxy == nullptr)
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			SystemProxy = NewObject <UCavrnusSpatialConnectorSubSystemProxy>(GameInstance);
			SystemProxy->SetGameInstance(GameInstance);
			SystemProxy->SetObjectOwner(GameInstance);
			SystemProxy->Initialize();
		}
	}
}

void UCavrnusSpatialConnectorSubSystem::Deinitialize()
{
	SystemProxy->Deinitialize();
	SystemProxy = nullptr;

	Super::Deinitialize();
}

UCavrnusSpatialConnectorSubSystemProxy* UCavrnusSpatialConnectorSubSystem::GetSystemProxy()
{
	return SystemProxy;
}