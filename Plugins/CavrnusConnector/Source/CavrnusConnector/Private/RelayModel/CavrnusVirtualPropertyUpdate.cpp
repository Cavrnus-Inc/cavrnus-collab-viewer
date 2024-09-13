// Copyright(c) Cavrnus. All rights reserved.
#include "RelayModel/CavrnusVirtualPropertyUpdate.h"
#include "RelayModel/CavrnusRelayModel.h"
#include "Types/CavrnusPropertyValue.h"
#include "Types/AbsolutePropertyId.h"
#include "Translation/CavrnusProtoTranslation.h"

namespace Cavrnus
{
	CavrnusVirtualPropertyUpdate::CavrnusVirtualPropertyUpdate()
	{
	}

	CavrnusVirtualPropertyUpdate::CavrnusVirtualPropertyUpdate(Cavrnus::CavrnusRelayModel* relayModel, FCavrnusSpaceConnection spaceConn, const FAbsolutePropertyId& propertyId, const FPropertyValue& propVal, const FPropertyPostOptions& options)
	{
		RelayModel = relayModel;
		SpaceConn = spaceConn;
		PropertyId = propertyId;
		Options = options;

		LiveUpdaterId = FGuid::NewGuid().ToString();

		lastSentValue = propVal;

		int localChangeId = -1;
		if(!Options.Smoothed)
			RelayModel->GetSpacePropertyModel(SpaceConn)->SetLocalPropVal(PropertyId, propVal);
		RelayModel->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildBeginLivePropertyUpdateMsg(SpaceConn, LiveUpdaterId, PropertyId, propVal, localChangeId, Options));

		lastUpdatedTimeSec = FPlatformTime::Seconds();
	}

	CavrnusVirtualPropertyUpdate::~CavrnusVirtualPropertyUpdate()
	{
	}

	void CavrnusVirtualPropertyUpdate::UpdateWithNewData(const FPropertyValue& propVal)
	{
		lastSentValue = propVal;

		int localChangeId = -1;
		if (!Options.Smoothed)
			RelayModel->GetSpacePropertyModel(SpaceConn)->SetLocalPropVal(PropertyId, propVal);
		RelayModel->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildContinueLivePropertyUpdateMsg(SpaceConn, LiveUpdaterId, PropertyId, propVal, localChangeId, Options));

		lastUpdatedTimeSec = FPlatformTime::Seconds();
	}

	void CavrnusVirtualPropertyUpdate::Finalize()
	{
		int localChangeId = -1;
		if (!Options.Smoothed)
			RelayModel->GetSpacePropertyModel(SpaceConn)->SetLocalPropVal(PropertyId, lastSentValue);
		RelayModel->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildFinalizeLivePropertyUpdateMsg(SpaceConn, LiveUpdaterId, PropertyId, lastSentValue, localChangeId, Options));

		lastUpdatedTimeSec = FPlatformTime::Seconds();
	}

	void CavrnusVirtualPropertyUpdate::Finalize(const FPropertyValue& propVal)
	{
		lastSentValue = propVal;

		int localChangeId = -1;
		if (!Options.Smoothed)
			RelayModel->GetSpacePropertyModel(SpaceConn)->SetLocalPropVal(PropertyId, propVal);
		RelayModel->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildFinalizeLivePropertyUpdateMsg(SpaceConn, LiveUpdaterId, PropertyId, propVal, localChangeId, Options));

		lastUpdatedTimeSec = FPlatformTime::Seconds();
	}

	void CavrnusVirtualPropertyUpdate::Cancel()
	{
		RelayModel->SendMessage(Cavrnus::CavrnusProtoTranslation::BuildCancelLiveUpdateMsg(SpaceConn, LiveUpdaterId));
	}
}

