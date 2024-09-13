// // Copyright (c), Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CavrnusSyncAvatarTransform.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class CAVRNUSCONNECTOR_API UCavrnusSyncAvatarTransform : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cavrnus|AvatarTransform")
	bool UseHeightOffset = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cavrnus|AvatarTransform")
	bool ForceSetHeight = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cavrnus|AvatarTransform")
	bool LimitRoll = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cavrnus|AvatarTransform")
	bool LimitPitch = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cavrnus|AvatarTransform")
	bool LimitYaw = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cavrnus|AvatarTransform")
	float TargetSetHeight = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cavrnus|AvatarTransform")
	float HeightOffset = 0.0f;
};