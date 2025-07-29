#pragma once

#include "CavrnusDataSmithTransformSync.h"
#include "DatasmithRuntime.h"
#include "CavrnusDatasmithRuntimeManager.generated.h"
UCLASS()
class CAVRNUSCVT_API UCavrnusDatasmithRuntimeManager : public UObject
{
	GENERATED_BODY()
public:
	UCavrnusDatasmithRuntimeManager() {};
	void Setup(const FCavrnusSpaceConnection& InSpaceConn);
	void RegisterTransformSync(AActor* Actor, UCavrnusDataSmithTransformSync* TransformSync);
	void RegisterDatasmithActor(ADatasmithRuntimeActor* DActor);

	bool isRegistered(ADatasmithRuntimeActor* DActor);
private:

	UPROPERTY()
	TArray<UCavrnusDataSmithTransformSync*> TransformSyncs;

	UPROPERTY()
	TArray<ADatasmithRuntimeActor*> DatasmithActors;

	FCavrnusSpaceConnection SpaceConnection;
};

