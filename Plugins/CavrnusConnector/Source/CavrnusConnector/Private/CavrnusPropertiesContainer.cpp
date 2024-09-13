// Copyright(c) Cavrnus. All rights reserved.
#include "CavrnusPropertiesContainer.h"
#include "CavrnusSpatialConnectorSubSystem.h"
#include "CavrnusFunctionLibrary.h"
#include "Components/SceneComponent.h"
#include <Engine/Level.h>
#include <GameFramework/Actor.h>

UCavrnusPropertiesContainer::UCavrnusPropertiesContainer()
{
	bWantsInitializeComponent = true;
}

void UCavrnusPropertiesContainer::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner() && (GetOwner()->GetWorld()->WorldType == EWorldType::Game || GetOwner()->GetWorld()->WorldType == EWorldType::PIE))
	{
		UCavrnusFunctionLibrary::GetCavrnusSpatialConnectorSubSystemProxy()->InitializeCavrnusActor(this->GetOwner());
	}
}