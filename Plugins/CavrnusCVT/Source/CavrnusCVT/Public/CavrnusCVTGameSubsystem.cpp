#include "CavrnusCVTGameSubsystem.h"
#include "CavrnusPropertyAssetProcessor.h"
#include "CavrnusFunctionLibrary.h"
void UCavrnusCVTGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	CPAP = NewObject<UCavrnusPropertyAssetProcessor>(this);
	CavrnusSpaceConnected spaceCallback = [this](const FCavrnusSpaceConnection& SpaceConn)
		{
			CPAP->Setup(SpaceConn);

		};
	UCavrnusFunctionLibrary::AwaitAnySpaceConnection(spaceCallback);
}

void UCavrnusCVTGameSubsystem::Deinitialize()
{

}

