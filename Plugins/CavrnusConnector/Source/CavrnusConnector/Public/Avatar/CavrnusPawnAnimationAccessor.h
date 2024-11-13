// Copyright(c) Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include <Components/ActorComponent.h>

#include "CavrnusPawnAnimationAccessor.generated.h"

UCLASS(Blueprintable)
class UCavrnusPawnAnimationAccessor : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, Category="Cavrnus")
	void SetHeadRotation(FRotator RotationValue);
};
