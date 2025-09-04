#include "CavrnusCVTGameSubsystem.h"
#include "CavrnusPropertyAssetProcessor.h"
#include "CavrnusFunctionLibrary.h"
void UCavrnusCVTGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

	CPAP = NewObject<UCavrnusPropertyAssetProcessor>(this);
	CPAP->SingletonInit = false;
	CavrnusSpaceConnected spaceCallback = [this](const FCavrnusSpaceConnection& SpaceConn)
		{
			CPAP->Setup(SpaceConn);
		};
	UCavrnusFunctionLibrary::AwaitAnySpaceConnection(spaceCallback);
}

void UCavrnusCVTGameSubsystem::Deinitialize()
{
	for (UCavrnusDataSmithTransformSync* Sync : TransformSyncs)
	{
		if (Sync)
			Sync->Cleanup();
	}
}

