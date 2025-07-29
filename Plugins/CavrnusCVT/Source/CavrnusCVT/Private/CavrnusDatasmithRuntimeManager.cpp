#include "CavrnusDatasmithRuntimeManager.h"

void UCavrnusDatasmithRuntimeManager::Setup(const FCavrnusSpaceConnection& InSpaceConn)
{
	SpaceConnection = InSpaceConn;
	TransformSyncs.Empty();
	DatasmithActors.Empty();
}

void UCavrnusDatasmithRuntimeManager::RegisterTransformSync(AActor*Actor, UCavrnusDataSmithTransformSync* TransformSync)
{
	UE_LOG(LogTemp, Error, TEXT("Registered Transform Sync for Target Actor %p"), Actor);
	TransformSyncs.Add(TransformSync);
}

bool UCavrnusDatasmithRuntimeManager::isRegistered(ADatasmithRuntimeActor* Actor)
{
	if (DatasmithActors.Contains(Actor))
		return true;
	return false;
}

void UCavrnusDatasmithRuntimeManager::RegisterDatasmithActor(ADatasmithRuntimeActor* DActor)
{
	DatasmithActors.Add(DActor);
}