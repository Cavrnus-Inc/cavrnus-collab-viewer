#pragma once
// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CavrnusPropertyAssetProcessor.h"
#include "CavrnusCVTGameSubsystem.generated.h"

UCLASS()
class CAVRNUSCVT_API UCavrnusCVTGameSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    UCavrnusPropertyAssetProcessor* GetCPAP() { return CPAP; }

    UFUNCTION(BlueprintCallable)
	void AddTransformSync(UCavrnusDataSmithTransformSync* NewSync) { TransformSyncs.Add(NewSync); }

private:
    UPROPERTY()
    UCavrnusPropertyAssetProcessor* CPAP;

    UPROPERTY()
    TArray<UCavrnusDataSmithTransformSync*> TransformSyncs;

};