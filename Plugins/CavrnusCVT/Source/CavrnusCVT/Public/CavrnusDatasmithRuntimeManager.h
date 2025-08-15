#pragma once

#include "CavrnusDataSmithTransformSync.h"
#include "DatasmithRuntime.h"

#include "Delegates/DelegateCombinations.h"

#include "CavrnusDatasmithRuntimeManager.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDatasmithActorLoaded, ADatasmithRuntimeActor*, SpawnedActor);

UENUM()
enum EDatasmithStatusFlags : uint8
{
	None = 0,
	isWaiting = 1 << 0,
	isLoading = 1 << 1,
	isLoaded = 1 << 2,
	isRegistered = 1 << 3,
	isCavrnized = 1 << 4,
};
ENUM_CLASS_FLAGS(EDatasmithStatusFlags)


UCLASS()
class UDatasmithTrackingInfo : public UObject
{
	GENERATED_BODY()

public:
	UDatasmithTrackingInfo() {
		Status = EDatasmithStatusFlags::None;
		SourcePath = "";
	}
	UDatasmithTrackingInfo(const FString& InSourcePath)
	{
		Status = EDatasmithStatusFlags::None;
		SourcePath = InSourcePath;
	}

	bool HasFlag(EDatasmithStatusFlags Flag) const
	{
		return EnumHasAllFlags(static_cast<EDatasmithStatusFlags>(Status), Flag);
	}

	void SetStatusFlag(EDatasmithStatusFlags Flag)
	{
		Status = static_cast<EDatasmithStatusFlags>(Status) | Flag;
	}

	void ClearStatusFlag(EDatasmithStatusFlags Flag)
	{
		Status = static_cast<EDatasmithStatusFlags>(Status) & ~Flag;
	}

	UPROPERTY()
	FString SourcePath;

	UPROPERTY()
	TEnumAsByte<EDatasmithStatusFlags> Status;
};


UCLASS(BlueprintType, Blueprintable)
class CAVRNUSCVT_API UCavrnusDatasmithRuntimeManager : public UObject
{
	GENERATED_BODY()
public:
	UCavrnusDatasmithRuntimeManager() {};
	void Setup(const FCavrnusSpaceConnection& InSpaceConn);

	void RegisterTransformSync(AActor* Actor, UCavrnusDataSmithTransformSync* TransformSync);

	UFUNCTION(BlueprintCallable, Category="Cavrnus Datasmith")
	bool RegisterDatasmithActor(ADatasmithRuntimeActor* DActor);

	UFUNCTION(BlueprintCallable, Category = "Cavrnus Datasmith")
	bool UnregisterDatasmithActor(ADatasmithRuntimeActor* DActor);

	UFUNCTION(BlueprintCallable, Category="Cavrnus Datasmith")
	void ProcessDatasmithActor(ADatasmithRuntimeActor* DActor, const FString& ContainerName);

	UFUNCTION(BlueprintCallable, Category = "Cavrnus Datasmith")
	bool IsRegistered(ADatasmithRuntimeActor* DActor);

	UFUNCTION(BlueprintCallable, Category = "Cavrnus Datasmith")
	bool IsLoading(ADatasmithRuntimeActor* DActor);

	UFUNCTION(BlueprintCallable, Category = "Cavrnus Datasmith")
	bool IsLoaded(ADatasmithRuntimeActor* DActor);


	UFUNCTION(BlueprintCallable, Category = "Cavrnus Datasmith")
	bool IsRegisteredCheckPath(const FString& DActorPath);

	UFUNCTION(BlueprintCallable, Category = "Cavrnus Datasmith")
	bool IsLoadingCheckPath(const FString& DActorPath);

	UFUNCTION(BlueprintCallable, Category = "Cavrnus Datasmith")
	bool IsLoadedCheckPath(const FString& DActorPath);

	UFUNCTION(BlueprintCallable, Category = "Cavrnus Datasmith")
	void LoadDatasmithActor(ADatasmithRuntimeActor* DatasmithActor, const FString& SourcePath, FOnDatasmithActorLoaded OnDatasmithActorLoaded);

	UFUNCTION(BlueprintCallable, Category = "Cavrnus Datasmith")
	void SpawnDatasmithActor(const FString& SourcePath, FOnDatasmithActorLoaded OnDatasmithActorLoaded);

	UFUNCTION(BlueprintCallable, Category = "Cavrnus Datasmith")
	void DestroyDatasmithActor(ADatasmithRuntimeActor* DActor);

private:
	bool FindDatasmithActorByPath(const FString& SourcePath, ADatasmithRuntimeActor*& DActor, UDatasmithTrackingInfo*& DTI);

	UPROPERTY()
	TArray<UCavrnusDataSmithTransformSync*> TransformSyncs; // Holds on to these so they're not garbage collected

	UPROPERTY()
	TArray<ADatasmithRuntimeActor*> DatasmithActors;

	UPROPERTY()
	TMap<TWeakObjectPtr<ADatasmithRuntimeActor>, UDatasmithTrackingInfo*> DatasmithActorInfo;

	FCavrnusSpaceConnection SpaceConnection;
};

