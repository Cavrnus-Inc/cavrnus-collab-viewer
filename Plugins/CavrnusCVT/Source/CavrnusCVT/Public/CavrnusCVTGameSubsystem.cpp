#include "CavrnusCVTGameSubsystem.h"
#include "CavrnusPropertyAssetProcessor.h"
#include "CavrnusDatasmithRuntimeManager.h"
#include "CavrnusFunctionLibrary.h"
void UCavrnusCVTGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

	CPAP = NewObject<UCavrnusPropertyAssetProcessor>(this);
	CDRM = NewObject<UCavrnusDatasmithRuntimeManager>(this);
	CPAP->SingletonInit = false;
	CavrnusSpaceConnected spaceCallback = [this](const FCavrnusSpaceConnection& SpaceConn)
		{
			CPAP->Setup(SpaceConn);
			CDRM->Setup(SpaceConn);
		};
	UCavrnusFunctionLibrary::AwaitAnySpaceConnection(spaceCallback);
}

void UCavrnusCVTGameSubsystem::Deinitialize()
{

}

