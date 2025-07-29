#pragma once
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "CavrnusDataSmithTransformSync.h"

#include "Types\AbsolutePropertyId.h"

#include "Types/CavrnusSpaceConnection.h"
#include "UObject/Object.h"
#include "DatasmithRuntime.h"
#include "Engine/StaticMeshActor.h"
#include "CavrnusPropertyAssetProcessor.generated.h"
UCLASS()
class CAVRNUSCVT_API UCavrnusPropertyAssetProcessor : public UObject
{
	GENERATED_BODY()
public:
	UCavrnusPropertyAssetProcessor() {};

	void Setup(const FCavrnusSpaceConnection& InSpaceConn);

	int SingletonInit;

	static void GetAllStaticMeshActorsRecursive(const AActor* InRoot, TArray<AStaticMeshActor*>& OutMeshActors);
	
	UFUNCTION(BlueprintCallable, Category = "Cavrnus")
	void ProcessRuntimeDatasmithActorProperties(AActor* Actor, const FString& Container);

private:
	static TMap<UClass*, TArray<FString>> SupportedPropertyMap;

	FString computeHash(AActor* MeshActor)
	{
		FString ActorName = MeshActor->GetName();
		const uint32 Hash = GetTypeHash(ActorName);
		FString UniqueName = FString::Printf(TEXT("%u_%s"), Hash, *ActorName);
		return UniqueName;
	}

	void SingletonInits();
	void BuildPropertyMap();
	bool isSupportedProperty(UClass*, FString);

	FString ContainerName = "";
	FCavrnusSpaceConnection SpaceConnection = FCavrnusSpaceConnection();

	UFUNCTION(BlueprintCallable, Category = "Cavrnus")
	void FixMaterialsOnRuntimeDatasmithActor(ADatasmithRuntimeActor* DatasmithActor);

	void ProcessActorsRecursive(const AActor* InRoot, TArray<AStaticMeshActor*>& OutMeshActors, const FString& Container);
	int ProcessTwinmotionDatasmithChildUsingSlotNames(const AActor* Actor);
	int ProcessActorProperties(AActor* Actor, const FString& Container);
	int ProcessComponents(const AActor* Actor, const FString& Container);
	int ProcessMaterialParameters(UMaterialInstanceDynamic* MaterialInterface, const FString& Container);

	void ProcessScene();
};

