// Copyright (c) 2024 Cavrnus. All rights reserved.

#include "Public/CavrnusFunctionLibrary.h"
#include "CavrnusConnectorModule.h"
#include <GameFramework/PlayerController.h>
#include <HAL/FileManager.h>
#include <Misc/FileHelper.h>
#include <Misc/Paths.h>
#include "Types/CavrnusCallbackTypes.h"
#include "LivePropertyUpdates/CavrnusLiveBoolPropertyUpdate.h"
#include "LivePropertyUpdates/CavrnusLiveColorPropertyUpdate.h"
#include "LivePropertyUpdates/CavrnusLiveFloatPropertyUpdate.h"
#include "LivePropertyUpdates/CavrnusLiveStringPropertyUpdate.h"
#include "LivePropertyUpdates/CavrnusLiveTransformPropertyUpdate.h"
#include "LivePropertyUpdates/CavrnusLiveVectorPropertyUpdate.h"
#include "Types\CavrnusPropertyValue.h"
#include "CavrnusSpatialConnectorSubSystem.h"
#include "RelayModel\CavrnusRelayModel.h"
#include "RelayModel\RelayCallbackModel.h"
#include "RelayModel\DataState.h"
#include "RelayModel\SpacePropertyModel.h"
#include "Types\AbsolutePropertyId.h"
#include "RelayModel\SpacePermissionsModel.h"
#include "Translation\CavrnusProtoTranslation.h"
#include "CavrnusSpatialConnector.h"

#pragma region Authentication

// ============================================
// Authentication Functions
// ============================================

void UCavrnusFunctionLibrary::SetForceKeepAlive()
{
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildSetForceKeepAlive());
}

void UCavrnusFunctionLibrary::EndForceKeepAlive()
{
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildEndForceKeepAlive());
}

UCavrnusSpatialConnectorSubSystemProxy* UCavrnusFunctionLibrary::GetCavrnusSpatialConnectorSubSystemProxy()
{
	TArray<UObject*> Results;
	GetObjectsOfClass(UCavrnusSpatialConnectorSubSystemProxy::StaticClass(), Results);

	UCavrnusSpatialConnectorSubSystemProxy* SpatialCore = nullptr;
	if (Results.Num() > 0)
	{
		ensure(Results.Num() == 1); // We don't expect more than one spatial core.
		SpatialCore = Cast<UCavrnusSpatialConnectorSubSystemProxy>(Results.Top());
	}

	return SpatialCore;
}

ACavrnusSpatialConnector* UCavrnusFunctionLibrary::GetCavrnusSpatialConnector()
{
	UCavrnusSpatialConnectorSubSystemProxy* SubProxy = GetCavrnusSpatialConnectorSubSystemProxy();
	if (SubProxy)
	{
		return SubProxy->GetCavrnusSpatialConnector();
	}

	return nullptr;
}

bool UCavrnusFunctionLibrary::IsLoggedIn()
{
	return Cavrnus::CavrnusRelayModel::GetDataModel()->GetDataState()->CurrentAuthentication != nullptr;
}

void UCavrnusFunctionLibrary::AuthenticateWithPassword(const FString& Server, const FString& Email, const FString& Password, FCavrnusAuthRecv OnSuccess, FCavrnusError OnFailure)
{
	CavrnusAuthRecv callback = [OnSuccess](const FCavrnusAuthentication& val)
	{
		OnSuccess.ExecuteIfBound(val);
	};
	CavrnusError errorCallback = [OnFailure](const FString& error)
	{
		OnFailure.ExecuteIfBound(error);
	};
	AuthenticateWithPassword(Server, Email, Password, callback, errorCallback);
}

void UCavrnusFunctionLibrary::AuthenticateWithPassword(const FString& Server, const FString& Email, const FString& Password, CavrnusAuthRecv OnSuccess, CavrnusError OnFailure)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterLoginPasswordCallback(OnSuccess, OnFailure);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildAuthenticateWithPassword(RequestId, Server, Email, Password));
}

void UCavrnusFunctionLibrary::AuthenticateAsGuest(const FString& Server, const FString& UserName, FCavrnusAuthRecv OnSuccess, FCavrnusError OnFailure)
{
	CavrnusAuthRecv callback = [OnSuccess](const FCavrnusAuthentication& val)
	{
		OnSuccess.ExecuteIfBound(val);
	};
	CavrnusError errorCallback = [OnFailure](const FString& error)
	{
		OnFailure.ExecuteIfBound(error);
	};
	AuthenticateAsGuest(Server, UserName, callback, errorCallback);
}

void UCavrnusFunctionLibrary::AuthenticateAsGuest(const FString& Server, const FString& UserName, CavrnusAuthRecv OnSuccess, CavrnusError OnFailure)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterLoginGuestCallback(OnSuccess, OnFailure);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildAuthenticateGuest(RequestId, Server, UserName));
}

void UCavrnusFunctionLibrary::AwaitAuthentication(FCavrnusAuthRecv OnAuth)
{
	CavrnusAuthRecv callback = [OnAuth](const FCavrnusAuthentication& val)
	{
		OnAuth.ExecuteIfBound(val);
	};
	AwaitAuthentication(callback);
}

void UCavrnusFunctionLibrary::AwaitAuthentication(CavrnusAuthRecv OnAuth)
{
	Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterAuthCallback(OnAuth);
}

#pragma endregion

#pragma region Spaces

// ============================================
// Space Functions
// ============================================

void UCavrnusFunctionLibrary::FetchJoinableSpaces(FCavrnusAllSpacesInfoEvent onRecvCurrentJoinableSpaces)
{
	CavrnusAllSpacesInfoEvent callback = [onRecvCurrentJoinableSpaces](const TArray<FCavrnusSpaceInfo>& val)
	{
		onRecvCurrentJoinableSpaces.ExecuteIfBound(val);
	};
	FetchJoinableSpaces(callback);
}

void UCavrnusFunctionLibrary::FetchJoinableSpaces(CavrnusAllSpacesInfoEvent OnRecvCurrentJoinableSpaces)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterFetchAvailableSpacesCallback(OnRecvCurrentJoinableSpaces);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildFetchAvailableSpaces(RequestId));
}

void UCavrnusFunctionLibrary::FetchSpaceInfo(FString spaceId, FCavrnusSpaceInfoEvent OnRecvSpaceInfo)
{
	CavrnusSpaceInfoEvent callback = [OnRecvSpaceInfo](const FCavrnusSpaceInfo& val)
		{
			OnRecvSpaceInfo.ExecuteIfBound(val);
		};
	FetchSpaceInfo(spaceId, callback);
}

void UCavrnusFunctionLibrary::FetchSpaceInfo(FString spaceId, CavrnusSpaceInfoEvent OnRecvSpaceInfo)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterFetchSpaceInfoCallback(OnRecvSpaceInfo);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildFetchSpaceInfo(RequestId, spaceId));
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindJoinableSpaces(FCavrnusSpaceInfoEvent SpaceAdded, FCavrnusSpaceInfoEvent SpaceUpdated, FCavrnusSpaceInfoEvent SpaceRemoved)
{
	CavrnusSpaceInfoEvent added = [SpaceAdded](const FCavrnusSpaceInfo& val)
	{
		SpaceAdded.ExecuteIfBound(val);
	};
	CavrnusSpaceInfoEvent updated = [SpaceUpdated](const FCavrnusSpaceInfo& val)
	{
		SpaceUpdated.ExecuteIfBound(val);
	};
	CavrnusSpaceInfoEvent removed = [SpaceRemoved](const FCavrnusSpaceInfo& val)
	{
		SpaceRemoved.ExecuteIfBound(val);
	};

	return BindJoinableSpaces(added, updated, removed);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindJoinableSpaces(CavrnusSpaceInfoEvent SpaceAdded, CavrnusSpaceInfoEvent SpaceUpdated, CavrnusSpaceInfoEvent SpaceRemoved)
{
	return Cavrnus::CavrnusRelayModel::GetDataModel()->GetDataState()->BindJoinableSpaces(SpaceAdded, SpaceUpdated, SpaceRemoved);
}

bool UCavrnusFunctionLibrary::IsConnectedToAnySpace()
{
	return Cavrnus::CavrnusRelayModel::GetDataModel()->GetDataState()->GetCurrentSpaceConnections().Num() > 0;
}

void UCavrnusFunctionLibrary::JoinSpace(FString SpaceId, FCavrnusSpaceConnected OnConnected, FCavrnusError OnFailure)
{
	CavrnusSpaceConnected callback = [OnConnected](const FCavrnusSpaceConnection& val)
	{
		OnConnected.ExecuteIfBound(val);
	};
	CavrnusError errorCallback = [OnFailure](const FString& val)
	{
		OnFailure.ExecuteIfBound(val);
	};

	JoinSpace(SpaceId, callback, errorCallback);
}

void UCavrnusFunctionLibrary::JoinSpace(FString SpaceId, CavrnusSpaceConnected OnConnected, CavrnusError OnFailure)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterJoinSpaceCallback(OnConnected, OnFailure);
	Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->HandleSpaceBeginLoading(SpaceId);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildJoinSpaceWithId(RequestId, SpaceId));
}

void UCavrnusFunctionLibrary::CreateSpace(FString SpaceName, FCavrnusSpaceCreated OnCreation, FCavrnusError OnFailure)
{
	CavrnusSpaceCreated callback = [OnCreation](const FCavrnusSpaceInfo& val)
	{
		OnCreation.ExecuteIfBound(val);
	};
	CavrnusError errorCallback = [OnFailure](const FString& val)
	{
		OnFailure.ExecuteIfBound(val);
	};

	CreateSpace(SpaceName, TArray<FString>(), callback, errorCallback);
}

void UCavrnusFunctionLibrary::CreateSpace(FString SpaceName, CavrnusSpaceCreated OnCreation, CavrnusError OnFailure)
{
	CreateSpace(SpaceName, TArray<FString>(), OnCreation, OnFailure);
}

void UCavrnusFunctionLibrary::CreateSpace(FString SpaceName, TArray<FString> Keywords, CavrnusSpaceCreated OnCreation, CavrnusError OnFailure)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterCreateSpaceCallback(OnCreation, OnFailure);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildCreateSpaceMsg(RequestId, SpaceName, Keywords));
}

void UCavrnusFunctionLibrary::AwaitAnySpaceBeginLoading(FCavrnusSpaceBeginLoading OnBeginLoading)
{
	CavrnusSpaceBeginLoading callback = [OnBeginLoading](const FString& val)
	{
		OnBeginLoading.ExecuteIfBound(val);
	};

	AwaitAnySpaceBeginLoading(callback);
}

void UCavrnusFunctionLibrary::AwaitAnySpaceBeginLoading(CavrnusSpaceBeginLoading OnBeginLoading)
{
	Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterBeginLoadingSpaceCallback(OnBeginLoading);
}

void UCavrnusFunctionLibrary::AwaitAnySpaceConnection(FCavrnusSpaceConnected OnConnected)
{
	CavrnusSpaceConnected callback = [OnConnected](const FCavrnusSpaceConnection& SpaceConn)
	{
		OnConnected.ExecuteIfBound(SpaceConn);
	};

	AwaitAnySpaceConnection(callback);
}

void UCavrnusFunctionLibrary::AwaitAnySpaceConnection(CavrnusSpaceConnected OnConnected)
{
	Cavrnus::CavrnusRelayModel::GetDataModel()->GetDataState()->AwaitAnySpaceConnection(OnConnected);
}

void UCavrnusFunctionLibrary::ExitSpace(FCavrnusSpaceConnection SpaceConnection)
{
	CheckErrors(SpaceConnection);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildExitSpaceMsg(SpaceConnection));
}

#pragma endregion

// ============================================
// Properties!
// ============================================

#pragma region Generic Prop Functions

void UCavrnusFunctionLibrary::DefineGenericPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, Cavrnus::FPropertyValue PropertyValue, bool overrideExistingDefault)
{
	CheckErrors(SpaceConnection);

	if (!overrideExistingDefault && Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(SpaceConnection)->CurrDefinedProps.Contains(FAbsolutePropertyId(ContainerName, PropertyName)))
		return;

	Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(SpaceConnection)->SetIsDefined(FAbsolutePropertyId(ContainerName, PropertyName));
	int localChangeId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(SpaceConnection)->SetLocalPropVal(FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildDefinePropMsg(SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue, localChangeId));
}

Cavrnus::FPropertyValue UCavrnusFunctionLibrary::GetGenericPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName)
{
	CheckErrors(SpaceConnection);
	return Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(SpaceConnection)->GetPropValue(FAbsolutePropertyId(ContainerName, PropertyName));
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindGenericPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, const CavrnusPropertyFunction& OnPropertyUpdated)
{
	CheckErrors(SpaceConnection);

	return Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(SpaceConnection)->BindProperty(FAbsolutePropertyId(ContainerName, PropertyName), OnPropertyUpdated);
}

UCavrnusLivePropertyUpdate* UCavrnusFunctionLibrary::BeginTransientGenericPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, Cavrnus::FPropertyValue PropertyValue)
{
	CheckErrors(SpaceConnection);

	UCavrnusLivePropertyUpdate* res = nullptr;

	if(PropertyValue.PropType == Cavrnus::FPropertyValue::PropertyType::Bool)
		res = NewObject<UCavrnusLiveBoolPropertyUpdate>();
	if (PropertyValue.PropType == Cavrnus::FPropertyValue::PropertyType::String)
		res = NewObject<UCavrnusLiveStringPropertyUpdate>();
	if (PropertyValue.PropType == Cavrnus::FPropertyValue::PropertyType::Color)
		res = NewObject<UCavrnusLiveColorPropertyUpdate>();
	if (PropertyValue.PropType == Cavrnus::FPropertyValue::PropertyType::Float)
		res = NewObject<UCavrnusLiveFloatPropertyUpdate>();
	if (PropertyValue.PropType == Cavrnus::FPropertyValue::PropertyType::Vector)
		res = NewObject<UCavrnusLiveVectorPropertyUpdate>();
	if (PropertyValue.PropType == Cavrnus::FPropertyValue::PropertyType::Transform)
		res = NewObject<UCavrnusLiveTransformPropertyUpdate>();

	res->InitializeGeneric(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);

	return res;
}

void UCavrnusFunctionLibrary::PostGenericPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, Cavrnus::FPropertyValue PropertyValue, const FPropertyPostOptions& options)
{
	CheckErrors(SpaceConnection);
	int localChangeId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(SpaceConnection)->SetLocalPropVal(FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildUpdatePropMsg(SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue, localChangeId, options));
}

bool UCavrnusFunctionLibrary::PropertyValueExists(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName)
{
	return Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(SpaceConnection)->PropValueExists(FAbsolutePropertyId(ContainerName, PropertyName));
}

#pragma endregion


#pragma region Color Prop Functions

void UCavrnusFunctionLibrary::DefineColorPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FLinearColor PropertyValue)
{
	DefineGenericPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::ColorPropValue(PropertyValue));
}

void UCavrnusFunctionLibrary::DefineColorPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, FLinearColor PropertyValue)
{
	DefineGenericPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::ColorPropValue(PropertyValue));
}

FLinearColor UCavrnusFunctionLibrary::GetColorPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName)
{
	return GetGenericPropertyValue(SpaceConnection, ContainerName, PropertyName).ColorValue;
}

FLinearColor UCavrnusFunctionLibrary::GetColorPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName)
{
	return GetGenericPropertyValue(SpaceConnection, ContainerName, PropertyName).ColorValue;
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindColorPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FColorPropertyUpdated PropertyUpdateEvent)
{
	CavrnusPropertyFunction propUpdateCallback = [PropertyUpdateEvent](const Cavrnus::FPropertyValue& Prop, const FString& ContainerName, const FString& PropertyName)
	{
		UE_LOG(LogCavrnusConnector, Warning, TEXT("Exec color prop binding: %s"), *Prop.ColorValue.ToString())

		PropertyUpdateEvent.ExecuteIfBound(Prop.ColorValue, ContainerName, PropertyName);
	};

	return BindGenericPropertyValue(SpaceConnection, ContainerName, PropertyName, propUpdateCallback);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindColorPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, const CavrnusColorFunction& OnPropertyUpdated)
{
	CavrnusPropertyFunction propUpdateCallback = [OnPropertyUpdated](const Cavrnus::FPropertyValue& Prop, const FString& ContainerName, const FString& PropertyName)
	{
		OnPropertyUpdated(Prop.ColorValue, ContainerName, PropertyName);
	};

	return BindGenericPropertyValue(SpaceConnection, ContainerName, PropertyName, propUpdateCallback);
}

UCavrnusLiveColorPropertyUpdate* UCavrnusFunctionLibrary::BeginTransientColorPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FLinearColor PropertyValue)
{
	CheckErrors(SpaceConnection);

	UCavrnusLiveColorPropertyUpdate* res = NewObject<UCavrnusLiveColorPropertyUpdate>();
	res->Initialize(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);

	return res;
}

UCavrnusLiveColorPropertyUpdate* UCavrnusFunctionLibrary::BeginTransientColorPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, FLinearColor PropertyValue)
{
	CheckErrors(SpaceConnection);

	UCavrnusLiveColorPropertyUpdate* res = NewObject<UCavrnusLiveColorPropertyUpdate>();
	res->Initialize(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);

	return res;
}

void UCavrnusFunctionLibrary::PostColorPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FLinearColor PropertyValue)
{
	PostGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::ColorPropValue(PropertyValue));
}

void UCavrnusFunctionLibrary::PostColorPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, FLinearColor PropertyValue)
{
	PostGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::ColorPropValue(PropertyValue));
}

#pragma endregion

#pragma region Bool Prop Functions

void UCavrnusFunctionLibrary::DefineBoolPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, bool PropertyValue)
{
	DefineGenericPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::BoolPropValue(PropertyValue));
}

void UCavrnusFunctionLibrary::DefineBoolPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, bool PropertyValue)
{
	DefineGenericPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::BoolPropValue(PropertyValue));
}

bool UCavrnusFunctionLibrary::GetBoolPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName)
{
	return GetGenericPropertyValue(SpaceConnection, ContainerName, PropertyName).BoolValue;
}

bool UCavrnusFunctionLibrary::GetBoolPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName)
{
	return GetGenericPropertyValue(SpaceConnection, ContainerName, PropertyName).BoolValue;
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindBooleanPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FBoolPropertyUpdated PropertyUpdateEvent)
{
	CavrnusPropertyFunction propUpdateCallback = [PropertyUpdateEvent](const Cavrnus::FPropertyValue& Prop, const FString& ContainerName, const FString& PropertyName)
	{
		PropertyUpdateEvent.ExecuteIfBound(Prop.BoolValue, ContainerName, PropertyName);
	};

	return BindGenericPropertyValue(SpaceConnection, ContainerName, PropertyName, propUpdateCallback);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindBooleanPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, const CavrnusBoolFunction& OnPropertyUpdated)
{
	CavrnusPropertyFunction propUpdateCallback = [OnPropertyUpdated](const Cavrnus::FPropertyValue& Prop, const FString& ContainerName, const FString& PropertyName)
	{
		OnPropertyUpdated(Prop.BoolValue, ContainerName, PropertyName);
	};

	return BindGenericPropertyValue(SpaceConnection, ContainerName, PropertyName, propUpdateCallback);
}

UCavrnusLiveBoolPropertyUpdate* UCavrnusFunctionLibrary::BeginTransientBoolPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, bool PropertyValue)
{
	CheckErrors(SpaceConnection);

	UCavrnusLiveBoolPropertyUpdate* res = NewObject<UCavrnusLiveBoolPropertyUpdate>();
	res->Initialize(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);

	return res;
}

UCavrnusLiveBoolPropertyUpdate* UCavrnusFunctionLibrary::BeginTransientBoolPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, bool PropertyValue)
{
	CheckErrors(SpaceConnection);

	UCavrnusLiveBoolPropertyUpdate* res = NewObject<UCavrnusLiveBoolPropertyUpdate>();
	res->Initialize(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);

	return res;
}

void UCavrnusFunctionLibrary::PostBoolPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, bool PropertyValue)
{
	PostGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::BoolPropValue(PropertyValue));
}

void UCavrnusFunctionLibrary::PostBoolPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, bool PropertyValue)
{
	PostGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::BoolPropValue(PropertyValue));
}

#pragma endregion

#pragma region Float Prop Functions

void UCavrnusFunctionLibrary::DefineFloatPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, float PropertyValue)
{
	DefineGenericPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::FloatPropValue(PropertyValue));
}

void UCavrnusFunctionLibrary::DefineFloatPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, float PropertyValue)
{
	DefineGenericPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::FloatPropValue(PropertyValue));
}

float UCavrnusFunctionLibrary::GetFloatPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName)
{
	return GetGenericPropertyValue(SpaceConnection, ContainerName, PropertyName).FloatValue;
}

float UCavrnusFunctionLibrary::GetFloatPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName)
{
	return GetGenericPropertyValue(SpaceConnection, ContainerName, PropertyName).FloatValue;
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindFloatPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FFloatPropertyUpdated PropertyUpdateEvent)
{
	CavrnusPropertyFunction propUpdateCallback = [PropertyUpdateEvent](const Cavrnus::FPropertyValue& Prop, const FString& ContainerName, const FString& PropertyName)
	{
		PropertyUpdateEvent.ExecuteIfBound(Prop.FloatValue, ContainerName, PropertyName);
	};

	return BindGenericPropertyValue(SpaceConnection, ContainerName, PropertyName, propUpdateCallback);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindFloatPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, const CavrnusFloatFunction& OnPropertyUpdated)
{
	CavrnusPropertyFunction propUpdateCallback = [OnPropertyUpdated](const Cavrnus::FPropertyValue& Prop, const FString& ContainerName, const FString& PropertyName)
	{
		OnPropertyUpdated(Prop.FloatValue, ContainerName, PropertyName);
	};

	return BindGenericPropertyValue(SpaceConnection, ContainerName, PropertyName, propUpdateCallback);
}

UCavrnusLiveFloatPropertyUpdate* UCavrnusFunctionLibrary::BeginTransientFloatPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, float PropertyValue)
{
	CheckErrors(SpaceConnection);

	UCavrnusLiveFloatPropertyUpdate* res = NewObject<UCavrnusLiveFloatPropertyUpdate>();
	res->Initialize(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);

	return res;
}

UCavrnusLiveFloatPropertyUpdate* UCavrnusFunctionLibrary::BeginTransientFloatPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, float PropertyValue)
{
	CheckErrors(SpaceConnection);

	UCavrnusLiveFloatPropertyUpdate* res = NewObject<UCavrnusLiveFloatPropertyUpdate>();
	res->Initialize(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);

	return res;
}

void UCavrnusFunctionLibrary::PostFloatPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, float PropertyValue)
{
	PostGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::FloatPropValue(PropertyValue));
}

void UCavrnusFunctionLibrary::PostFloatPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, float PropertyValue)
{
	PostGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::FloatPropValue(PropertyValue));
}

#pragma endregion

#pragma region String Prop Functions

void UCavrnusFunctionLibrary::DefineStringPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FString PropertyValue)
{
	DefineGenericPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::StringPropValue(PropertyValue));
}

void UCavrnusFunctionLibrary::DefineStringPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, FString PropertyValue)
{
	DefineGenericPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::StringPropValue(PropertyValue));
}

FString UCavrnusFunctionLibrary::GetStringPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName)
{
	return GetGenericPropertyValue(SpaceConnection, ContainerName, PropertyName).StringValue;
}

FString UCavrnusFunctionLibrary::GetStringPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName)
{
	return GetGenericPropertyValue(SpaceConnection, ContainerName, PropertyName).StringValue;
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindStringPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FStringPropertyUpdated PropertyUpdateEvent)
{
	CavrnusPropertyFunction propUpdateCallback = [PropertyUpdateEvent](const Cavrnus::FPropertyValue& Prop, const FString& ContainerName, const FString& PropertyName)
	{
		PropertyUpdateEvent.ExecuteIfBound(Prop.StringValue, ContainerName, PropertyName);
	};

	return BindGenericPropertyValue(SpaceConnection, ContainerName, PropertyName, propUpdateCallback);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindStringPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, const CavrnusStringFunction& OnPropertyUpdated)
{
	CavrnusPropertyFunction propUpdateCallback = [OnPropertyUpdated](const Cavrnus::FPropertyValue& Prop, const FString& ContainerName, const FString& PropertyName)
	{
		OnPropertyUpdated(Prop.StringValue, ContainerName, PropertyName);
	};

	return BindGenericPropertyValue(SpaceConnection, ContainerName, PropertyName, propUpdateCallback);
}

UCavrnusLiveStringPropertyUpdate* UCavrnusFunctionLibrary::BeginTransientStringPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FString PropertyValue)
{
	CheckErrors(SpaceConnection);

	UCavrnusLiveStringPropertyUpdate* res = NewObject<UCavrnusLiveStringPropertyUpdate>();
	res->Initialize(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);

	return res;
}

UCavrnusLiveStringPropertyUpdate* UCavrnusFunctionLibrary::BeginTransientStringPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, FString PropertyValue)
{
	CheckErrors(SpaceConnection);

	UCavrnusLiveStringPropertyUpdate* res = NewObject<UCavrnusLiveStringPropertyUpdate>();
	res->Initialize(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);

	return res;
}

void UCavrnusFunctionLibrary::PostStringPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FString PropertyValue)
{
	PostGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::StringPropValue(PropertyValue));
}

void UCavrnusFunctionLibrary::PostStringPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, FString PropertyValue)
{
	PostGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::StringPropValue(PropertyValue));
}

#pragma endregion

#pragma region Vector Prop Functions

void UCavrnusFunctionLibrary::DefineVectorPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FVector4 PropertyValue)
{
	DefineGenericPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::VectorPropValue(PropertyValue));
}

void UCavrnusFunctionLibrary::DefineVectorPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, FVector4 PropertyValue)
{
	DefineGenericPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::VectorPropValue(PropertyValue));
}

FVector4 UCavrnusFunctionLibrary::GetVectorPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName)
{
	return GetGenericPropertyValue(SpaceConnection, ContainerName, PropertyName).VectorValue;
}

FVector4 UCavrnusFunctionLibrary::GetVectorPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName)
{
	return GetGenericPropertyValue(SpaceConnection, ContainerName, PropertyName).VectorValue;
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindVectorPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FVectorPropertyUpdated PropertyUpdateEvent)
{
	CavrnusPropertyFunction propUpdateCallback = [PropertyUpdateEvent](const Cavrnus::FPropertyValue& Prop, const FString& ContainerName, const FString& PropertyName)
	{
		PropertyUpdateEvent.ExecuteIfBound(Prop.VectorValue, ContainerName, PropertyName);
	};

	return BindGenericPropertyValue(SpaceConnection, ContainerName, PropertyName, propUpdateCallback);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindVectorPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, const CavrnusVectorFunction& OnPropertyUpdated)
{
	CavrnusPropertyFunction propUpdateCallback = [OnPropertyUpdated](const Cavrnus::FPropertyValue& Prop, const FString& ContainerName, const FString& PropertyName)
	{
		OnPropertyUpdated(Prop.VectorValue, ContainerName, PropertyName);
	};

	return BindGenericPropertyValue(SpaceConnection, ContainerName, PropertyName, propUpdateCallback);
}

UCavrnusLiveVectorPropertyUpdate* UCavrnusFunctionLibrary::BeginTransientVectorPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FVector4 PropertyValue)
{
	CheckErrors(SpaceConnection);

	UCavrnusLiveVectorPropertyUpdate* res = NewObject<UCavrnusLiveVectorPropertyUpdate>();
	res->Initialize(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);

	return res;
}

UCavrnusLiveVectorPropertyUpdate* UCavrnusFunctionLibrary::BeginTransientVectorPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, FVector4 PropertyValue)
{
	CheckErrors(SpaceConnection);

	UCavrnusLiveVectorPropertyUpdate* res = NewObject<UCavrnusLiveVectorPropertyUpdate>();
	res->Initialize(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue);

	return res;
}

void UCavrnusFunctionLibrary::PostVectorPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FVector4 PropertyValue)
{
	PostGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::VectorPropValue(PropertyValue));
}

void UCavrnusFunctionLibrary::PostVectorPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, FVector4 PropertyValue)
{
	PostGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::VectorPropValue(PropertyValue));
}

#pragma endregion

#pragma region Transform Prop Functions

void UCavrnusFunctionLibrary::DefineTransformPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FTransform PropertyValue)
{
	DefineGenericPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::TransformPropValue(PropertyValue));
}

void UCavrnusFunctionLibrary::DefineTransformPropertyDefaultValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, FTransform PropertyValue)
{
	DefineGenericPropertyDefaultValue(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::TransformPropValue(PropertyValue));
}

FTransform UCavrnusFunctionLibrary::GetTransformPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName)
{
	return GetGenericPropertyValue(SpaceConnection, ContainerName, PropertyName).TransformValue;
}

FTransform UCavrnusFunctionLibrary::GetTransformPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName)
{
	return GetGenericPropertyValue(SpaceConnection, ContainerName, PropertyName).TransformValue;
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindTransformPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FTransformPropertyUpdated PropertyUpdateEvent)
{
	CavrnusPropertyFunction propUpdateCallback = [PropertyUpdateEvent](const Cavrnus::FPropertyValue& Prop, const FString& ContainerName, const FString& PropertyName)
	{
		PropertyUpdateEvent.ExecuteIfBound(Prop.TransformValue, ContainerName, PropertyName);
	};

	return BindGenericPropertyValue(SpaceConnection, ContainerName, PropertyName, propUpdateCallback);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindTransformPropertyValue(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, const CavrnusTransformFunction& OnPropertyUpdated)
{
	CavrnusPropertyFunction propUpdateCallback = [OnPropertyUpdated](const Cavrnus::FPropertyValue& Prop, const FString& ContainerName, const FString& PropertyName)
	{
		OnPropertyUpdated(Prop.TransformValue, ContainerName, PropertyName);
	};

	return BindGenericPropertyValue(SpaceConnection, ContainerName, PropertyName, propUpdateCallback);
}

UCavrnusLiveTransformPropertyUpdate* UCavrnusFunctionLibrary::BeginTransientTransformPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FTransform PropertyValue, const FPropertyPostOptions& options)
{
	CheckErrors(SpaceConnection);

	UCavrnusLiveTransformPropertyUpdate* res = NewObject<UCavrnusLiveTransformPropertyUpdate>();
	res->Initialize(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue, options);

	return res;
}

UCavrnusLiveTransformPropertyUpdate* UCavrnusFunctionLibrary::BeginTransientTransformPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, FTransform PropertyValue, const FPropertyPostOptions& options)
{
	CheckErrors(SpaceConnection);

	UCavrnusLiveTransformPropertyUpdate* res = NewObject<UCavrnusLiveTransformPropertyUpdate>();
	res->Initialize(Cavrnus::CavrnusRelayModel::GetDataModel(), SpaceConnection, FAbsolutePropertyId(ContainerName, PropertyName), PropertyValue, options);

	return res;
}

void UCavrnusFunctionLibrary::PostTransformPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FString& ContainerName, const FString& PropertyName, FTransform PropertyValue, const FPropertyPostOptions& options)
{
	PostGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::TransformPropValue(PropertyValue), options);
}

void UCavrnusFunctionLibrary::PostTransformPropertyUpdate(FCavrnusSpaceConnection SpaceConnection, const FPropertiesContainer& ContainerName, const FString& PropertyName, FTransform PropertyValue, const FPropertyPostOptions& options)
{
	PostGenericPropertyUpdate(SpaceConnection, ContainerName, PropertyName, Cavrnus::FPropertyValue::TransformPropValue(PropertyValue), options);
}

#pragma endregion

#pragma region Permissions

UPARAM(DisplayName = "Disposable")UCavrnusBinding* UCavrnusFunctionLibrary::BindGlobalPolicy(const FString& Policy, FCavrnusPolicyUpdated OnPolicyUpdated)
{
	CavrnusPolicyUpdated callback = [OnPolicyUpdated](const FString& policy, bool allowed)
	{
		OnPolicyUpdated.ExecuteIfBound(policy, allowed);
	};

	return BindGlobalPolicy(Policy, callback);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindGlobalPolicy(FString Policy, CavrnusPolicyUpdated OnPolicyUpdated)
{
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildRequestGlobalPermission(Policy));

	return Cavrnus::CavrnusRelayModel::GetDataModel()->GetGlobalPermissionsModel()->BindPolicyAllowed(Policy, OnPolicyUpdated);
}

UPARAM(DisplayName = "Disposable")UCavrnusBinding* UCavrnusFunctionLibrary::BindSpacePolicy(FCavrnusSpaceConnection SpaceConnection, const FString& Policy, FCavrnusPolicyUpdated OnPolicyUpdated)
{
	CavrnusPolicyUpdated callback = [OnPolicyUpdated](const FString& policy, bool allowed)
	{
		OnPolicyUpdated.ExecuteIfBound(policy, allowed);
	};

	return BindSpacePolicy(SpaceConnection, Policy, callback);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindSpacePolicy(FCavrnusSpaceConnection SpaceConnection, const FString& Policy, CavrnusPolicyUpdated OnPolicyUpdated)
{
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildRequestSpacePermission(SpaceConnection, Policy));

	return Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePermissionsModel(SpaceConnection)->BindPolicyAllowed(Policy, OnPolicyUpdated);
}

#pragma endregion


#pragma region Spawned Objects

const FCavrnusSpawnedObject& UCavrnusFunctionLibrary::SpawnObject(FCavrnusSpaceConnection SpaceConnection, const FString& UniqueIdentifier)
{
	CheckErrors(SpaceConnection);

	FString InstanceId = Cavrnus::CavrnusProtoTranslation::CreateTransientId();

	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildCreateOp(SpaceConnection, UniqueIdentifier, InstanceId));

	//CavrnusSpawnedObjectFunction* CallbackPtr = new CavrnusSpawnedObjectFunction(spawnedObjectArrived);
	//Cavrnus::CavrnusRelayModel::GetDataModel()->ObjectCreationCallbacks.Add(FAbsolutePropertyId(InstanceId), CallbackPtr);

	Cavrnus::CavrnusRelayModel::GetDataModel()->HandleSpaceObjectAdded(Cavrnus::CavrnusProtoTranslation::BuildObjectAdded(SpaceConnection, UniqueIdentifier, InstanceId));
	return Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(SpaceConnection)->SpawnedObjects[InstanceId];
}

void UCavrnusFunctionLibrary::DestroyObject(const FCavrnusSpawnedObject& SpawnedObject)
{
	CheckErrors(SpawnedObject.SpaceConnection);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildDestroyOp(SpawnedObject.SpaceConnection, SpawnedObject.PropertiesContainerName));

	Cavrnus::CavrnusRelayModel::GetDataModel()->HandleSpaceObjectRemoved(Cavrnus::CavrnusProtoTranslation::BuildObjectRemoved(SpawnedObject.SpaceConnection, SpawnedObject.PropertiesContainerName));
}

#pragma endregion

#pragma region Space Users

// ============================================
// Space Users 
// ============================================

TArray<FCavrnusUser> UCavrnusFunctionLibrary::GetCurrentSpaceUsers(FCavrnusSpaceConnection SpaceConnection)
{
	CheckErrors(SpaceConnection);
	TArray<FCavrnusUser> users;
	Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(SpaceConnection)->CurrSpaceUsers.GenerateValueArray(users);

	return users;
}

void UCavrnusFunctionLibrary::AwaitLocalUser(FCavrnusSpaceConnection SpaceConnection, FCavrnusSpaceUserEvent LocalUserArrived)
{
	CavrnusSpaceUserEvent callback = [LocalUserArrived](const FCavrnusUser& user)
	{
		LocalUserArrived.ExecuteIfBound(user);
	};

	AwaitLocalUser(SpaceConnection, callback);
}

void UCavrnusFunctionLibrary::AwaitLocalUser(FCavrnusSpaceConnection SpaceConnection, CavrnusSpaceUserEvent LocalUserArrived)
{
	CheckErrors(SpaceConnection);
	Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(SpaceConnection)->AwaitLocalUser(LocalUserArrived);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindSpaceUsers(FCavrnusSpaceConnection SpaceConnection, FCavrnusSpaceUserEvent UserAdded, FCavrnusSpaceUserEvent UserRemoved)
{
	CavrnusSpaceUserEvent addedCallback = [UserAdded](const FCavrnusUser& user)
	{
		UserAdded.ExecuteIfBound(user);
	};
	CavrnusSpaceUserEvent removedCallback = [UserRemoved](const FCavrnusUser& user)
	{
		UserRemoved.ExecuteIfBound(user);
	};
	return BindSpaceUsers(SpaceConnection, addedCallback, removedCallback);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindSpaceUsers(FCavrnusSpaceConnection SpaceConnection, CavrnusSpaceUserEvent UserAdded, CavrnusSpaceUserEvent UserRemoved)
{
	CheckErrors(SpaceConnection);
	return Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(SpaceConnection)->BindSpaceUsers(UserAdded, UserRemoved);
}

UPARAM(DisplayName = "Disposable") UCavrnusBinding* UCavrnusFunctionLibrary::BindUserVideoFrames(FCavrnusSpaceConnection SpaceConnection, const FCavrnusUser& User, FCavrnusUserVideoFrameEvent OnVideoFrameUpdate)
{
	VideoFrameUpdateFunction VideoFrameUpdateCallback = [OnVideoFrameUpdate](UTexture2D* VideoTexture)
	{
		OnVideoFrameUpdate.ExecuteIfBound(VideoTexture);
	};

	return BindUserVideoFrames(SpaceConnection, User, VideoFrameUpdateCallback);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindUserVideoFrames(FCavrnusSpaceConnection SpaceConnection, const FCavrnusUser& User, const VideoFrameUpdateFunction& OnVideoFrameUpdate)
{
	CheckErrors(SpaceConnection);
	return Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(SpaceConnection)->BindUserVideoTexture(User, OnVideoFrameUpdate);
}

#pragma endregion

#pragma region Voice and Video

// ============================================
// Voice and Video
// ============================================

void UCavrnusFunctionLibrary::SetLocalUserMutedState(FCavrnusSpaceConnection SpaceConnection, bool bIsMuted)
{
	CheckErrors(SpaceConnection);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildSetLocalUserMuted(SpaceConnection, bIsMuted));
}

void UCavrnusFunctionLibrary::SetLocalUserStreamingState(FCavrnusSpaceConnection SpaceConnection, bool bIsStreaming)
{
	CheckErrors(SpaceConnection);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildSetLocalUserStreaming(SpaceConnection, bIsStreaming));
}

void UCavrnusFunctionLibrary::FetchSavedAudioInput(FCavrnusSavedInputDevice OnReceiveDevice)
{
	const CavrnusSavedInputDevice callback = [OnReceiveDevice](const FCavrnusInputDevice& device)
	{
		OnReceiveDevice.ExecuteIfBound(device);
	};
	FetchSavedAudioInput(callback);
}

void UCavrnusFunctionLibrary::FetchSavedAudioInput(CavrnusSavedInputDevice OnReceiveDevice)
{
	const CavrnusAvailableInputDevices callback = [OnReceiveDevice](const TArray<FCavrnusInputDevice>& devices)
	{
		FString savedDeviceId;
		FPlatformMisc::GetStoredValue(TEXT("Cavrnus"), TEXT("UE"), TEXT("AudioInput"), savedDeviceId);

		for (int i = 0; i < devices.Num(); i++) 
		{
			if (devices[i].DeviceId.Equals(savedDeviceId))
			{
				OnReceiveDevice(devices[i]);
				return;
			}
		}

		if (devices.Num() > 0)
			OnReceiveDevice(devices[0]);
	};
	FetchAudioInputs(callback);
}

void UCavrnusFunctionLibrary::FetchAudioInputs(FCavrnusAvailableInputDevices OnReceiveDevices)
{
	const CavrnusAvailableInputDevices callback = [OnReceiveDevices](const TArray<FCavrnusInputDevice>& devices)
	{
		OnReceiveDevices.ExecuteIfBound(devices);
	};
	FetchAudioInputs(callback);
}

void UCavrnusFunctionLibrary::FetchAudioInputs(CavrnusAvailableInputDevices OnReceiveDevices)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterFetchAudioInputs(OnReceiveDevices);

	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildRequestAudioInputs(RequestId));
}

void UCavrnusFunctionLibrary::UpdateAudioInput(FCavrnusInputDevice Device)
{
	FPlatformMisc::SetStoredValue(TEXT("Cavrnus"), TEXT("UE"), TEXT("AudioInput"), Device.DeviceId);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildSetAudioInput(Device));
}

void UCavrnusFunctionLibrary::FetchSavedAudioOutput(FCavrnusSavedOutputDevice OnReceiveDevice)
{
	const CavrnusSavedOutputDevice callback = [OnReceiveDevice](const FCavrnusOutputDevice& device)
	{
		OnReceiveDevice.ExecuteIfBound(device);
	};
	FetchSavedAudioOutput(callback);
}

void UCavrnusFunctionLibrary::FetchSavedAudioOutput(CavrnusSavedOutputDevice OnReceiveDevice)
{
	const CavrnusAvailableOutputDevices callback = [OnReceiveDevice](const TArray<FCavrnusOutputDevice>& devices)
	{
		FString savedDeviceId;
		FPlatformMisc::GetStoredValue(TEXT("Cavrnus"), TEXT("UE"), TEXT("AudioOutput"), savedDeviceId);

		for (int i = 0; i < devices.Num(); i++) {
			if (devices[i].DeviceId.Equals(savedDeviceId))
			{
				OnReceiveDevice(devices[i]);
				return;
			}
		}

		if (devices.Num() > 0)
			OnReceiveDevice(devices[0]);
	};
	FetchAudioOutputs(callback);
}

void UCavrnusFunctionLibrary::FetchAudioOutputs(FCavrnusAvailableOutputDevices OnReceiveDevices)
{
	const CavrnusAvailableOutputDevices callback = [OnReceiveDevices](const TArray<FCavrnusOutputDevice>& devices)
	{
		OnReceiveDevices.ExecuteIfBound(devices);
	};
	FetchAudioOutputs(callback);
}

void UCavrnusFunctionLibrary::FetchAudioOutputs(CavrnusAvailableOutputDevices OnReceiveDevices)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterFetchAudioOutputs(OnReceiveDevices);

	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildRequestAudioOutputs(RequestId));
}

void UCavrnusFunctionLibrary::UpdateAudioOutput(FCavrnusOutputDevice Device)
{
	FPlatformMisc::SetStoredValue(TEXT("Cavrnus"), TEXT("UE"), TEXT("AudioOutput"), Device.DeviceId);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildSetAudioOutput(Device));
}

void UCavrnusFunctionLibrary::FetchVideoInputs(FCavrnusAvailableVideoInputDevices OnReceiveDevices)
{
	const CavrnusAvailableVideoInputDevices callback = [OnReceiveDevices](const TArray<FCavrnusVideoInputDevice>& devices)
	{
		OnReceiveDevices.ExecuteIfBound(devices);
	};
	FetchVideoInputs(callback);
}

void UCavrnusFunctionLibrary::FetchVideoInputs(CavrnusAvailableVideoInputDevices OnReceiveDevices)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterFetchVideoInputs(OnReceiveDevices);

	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildRequestVideoInputs(RequestId));
}

void UCavrnusFunctionLibrary::UpdateVideoInput(FCavrnusVideoInputDevice Device)
{
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildSetVideoInput(Device));
}

void UCavrnusFunctionLibrary::FetchFileInfoById(FString ContentId, FCavrnusRemoteContentInfoFunction OnRecvContentInfo)
{
	CavrnusRemoteContentInfoFunction completeCallback = [OnRecvContentInfo](const FCavrnusRemoteContent& content)
		{
			OnRecvContentInfo.ExecuteIfBound(content);
		};

	FetchFileInfoById(ContentId, completeCallback);
}

void UCavrnusFunctionLibrary::FetchFileInfoById(FString ContentId, const CavrnusRemoteContentInfoFunction& OnRecvContentInfo)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterFetchRemoteContentInfo(OnRecvContentInfo);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildRequestFileInfoById(RequestId, ContentId));
}

void UCavrnusFunctionLibrary::FetchFileById(FString ContentId, FCavrnusContentProgressFunction OnProgress, FCavrnusContentFunction OnContentLoaded)
{
	CavrnusContentProgressFunction progressCallback = [OnProgress](const float prog, const FString& step)
	{
		OnProgress.ExecuteIfBound(prog, step);
	};

	CavrnusContentFunction completeCallback = [OnContentLoaded](const TArray<uint8>& bytes, const FString& fileName)
	{
		OnContentLoaded.ExecuteIfBound(bytes);
	};

	FetchFileById(ContentId, progressCallback, completeCallback);
}

void UCavrnusFunctionLibrary::FetchFileById(FString ContentId, const CavrnusContentProgressFunction& OnProgress, const CavrnusContentFunction& OnContentLoaded)
{
	//Sending this multiple times shouldn't hurt anything...
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildRequestFileById(ContentId));

	Cavrnus::CavrnusRelayModel::GetDataModel()->ContentModel.RegisterContentCallbacks(ContentId, OnProgress, OnContentLoaded);
}

void UCavrnusFunctionLibrary::FetchFileByIdToDisk(FString ContentId, FString FolderDestination, FCavrnusContentProgressFunction OnProgress, FCavrnusContentFileFunction OnContentLoaded)
{
	CavrnusContentProgressFunction progressCallback = [OnProgress](const float prog, const FString& step)
	{
		OnProgress.ExecuteIfBound(prog, step);
	};

	const TFunction<void(FString)>& completeCallback = [OnContentLoaded](const FString& path)
	{
		OnContentLoaded.ExecuteIfBound(path);
	};

	FetchFileByIdToDisk(ContentId, FolderDestination, progressCallback, completeCallback);
}

void UCavrnusFunctionLibrary::FetchFileByIdToDisk(FString ContentId, FString FolderDestination, const CavrnusContentProgressFunction& OnProgress, const TFunction<void(FString)>& OnContentLoaded)
{
	CavrnusContentFunction callback = [FolderDestination, OnContentLoaded](const TArray<uint8>& bytes, const FString& fileName)
	{
		FString fullDestPath = FolderDestination + "/" + fileName;

		FFileHelper::SaveArrayToFile(bytes, *fullDestPath, &IFileManager::Get(), 0);
		OnContentLoaded(fullDestPath);
	};

	FetchFileById(ContentId, OnProgress, callback);
}

void UCavrnusFunctionLibrary::FetchAllUploadedContent(FCavrnusRemoteContentFunction OnAvailableContentFetched)
{
	const CavrnusRemoteContentFunction remoteContentCallback = [OnAvailableContentFetched](const TArray<FCavrnusRemoteContent>& allContent)
	{
		OnAvailableContentFetched.ExecuteIfBound(allContent);
	};

	FetchAllUploadedContent(remoteContentCallback);
}

void UCavrnusFunctionLibrary::FetchAllUploadedContent(const CavrnusRemoteContentFunction& OnAvailableContentFetched)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterFetchAllAvailableContent(OnAvailableContentFetched);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildRequestAllUploadedContent(RequestId));
}

void UCavrnusFunctionLibrary::UploadContent(FString FilePath, FCavrnusUploadCompleteFunction OnUploadComplete)
{
	UploadContentWithTags(FilePath, TMap<FString, FString>(), OnUploadComplete);
}

void UCavrnusFunctionLibrary::UploadContent(FString FilePath, const CavrnusUploadCompleteFunction& OnUploadComplete)
{
	UploadContentWithTags(FilePath, TMap<FString, FString>(), OnUploadComplete);
}

void UCavrnusFunctionLibrary::UploadContentWithTags(FString FilePath, TMap<FString, FString> Tags, FCavrnusUploadCompleteFunction OnUploadComplete)
{
	CavrnusUploadCompleteFunction callback = [OnUploadComplete](const FCavrnusRemoteContent& uploadedContent)
	{
		OnUploadComplete.ExecuteIfBound(uploadedContent);
	};

	UploadContentWithTags(FilePath, Tags, callback);
}

void UCavrnusFunctionLibrary::UploadContentWithTags(FString FilePath, TMap<FString, FString> Tags, const CavrnusUploadCompleteFunction& OnUploadComplete)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterUploadContent(OnUploadComplete);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildUploadContent(RequestId, FilePath, Tags));
}

void UCavrnusFunctionLibrary::RequestContentDestinationFolder(FString FolderName, FCavrnusFolderCallback OnRecvFullFolderPath)
{
	TFunction<void(FString)> callback = [OnRecvFullFolderPath](const FString& fullFolderPath)
		{
			OnRecvFullFolderPath.ExecuteIfBound(fullFolderPath);
		};

	RequestContentDestinationFolder(FolderName, callback);
}

void UCavrnusFunctionLibrary::RequestContentDestinationFolder(FString FolderName, const TFunction<void(FString)>& OnRecvFullFolderPath)
{
	int RequestId = Cavrnus::CavrnusRelayModel::GetDataModel()->GetCallbackModel()->RegisterFolderReq(OnRecvFullFolderPath);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildFolderReq(RequestId, FolderName));
}

UPARAM(DisplayName = "Disposable")UCavrnusBinding* UCavrnusFunctionLibrary::BindChatMessages(const FCavrnusSpaceConnection& spaceConn, const FCavrnusChatFunction& ChatAdded, const FCavrnusChatFunction& ChatUpdated, const FCavrnusChatRemovedFunction& ChatRemoved)
{
	CavrnusChatFunction addedCallback = [ChatAdded](const FChatEntry& v)
	{
		ChatAdded.ExecuteIfBound(v);
	};
	CavrnusChatFunction updatedCallback = [ChatUpdated](const FChatEntry& v)
	{
		ChatUpdated.ExecuteIfBound(v);
	};
	CavrnusChatRemovedFunction removedCallback = [ChatRemoved](const FString& v)
	{
		ChatRemoved.ExecuteIfBound(v);
	};

	return BindChatMessages(spaceConn, addedCallback, updatedCallback, removedCallback);
}

UCavrnusBinding* UCavrnusFunctionLibrary::BindChatMessages(const FCavrnusSpaceConnection& spaceConn, const CavrnusChatFunction& ChatAdded, const CavrnusChatFunction& ChatUpdated, const CavrnusChatRemovedFunction& ChatRemoved)
{
	return Cavrnus::CavrnusRelayModel::GetDataModel()->GetSpacePropertyModel(spaceConn)->ChatModel->BindChatEvents(ChatAdded, ChatUpdated, ChatRemoved);
}

void UCavrnusFunctionLibrary::PostChatMessage(const FCavrnusSpaceConnection& spaceConn, const FString& Chat)
{
	CheckErrors(spaceConn);
	Cavrnus::CavrnusRelayModel::GetDataModel()->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildPostChatEntry(spaceConn, Chat));
}

#pragma endregion

bool UCavrnusFunctionLibrary::CheckErrors(FCavrnusSpaceConnection SpaceConnection)
{
	// If the SpaceConnectionId is -1, it indicates an invalid or uninitialized connection.
	if (SpaceConnection.SpaceConnectionId == -1)
	{
		// Log an error message indicating that no valid CavrnusSpaceConnection was provided.
		UE_LOG(LogCavrnusConnector, Error, TEXT("No CavrnusSpaceConnection provided, function will not execute!"));
		return false;			// Return false to indicate that the SpaceConnectionId is invalid.
	}
	return true;				// If the SpaceConnectionId is valid, return true.
}
