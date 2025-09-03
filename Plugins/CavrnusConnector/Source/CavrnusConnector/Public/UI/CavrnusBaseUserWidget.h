// Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CavrnusBaseUserWidget.generated.h"

/**
 * This class is used to identify/find cavrnus-specific UI during runtime
 */
UCLASS(Abstract)
class CAVRNUSCONNECTOR_API UCavrnusBaseUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetTitle(const FString& InTitle) { Title = InTitle; }
	FString GetTitle() const { return Title;}
	
	void SetId(const FGuid& Guid) {Id = Guid;}
	FGuid GetId() const {return Id;}
	
private:
	FString Title = "";
	FGuid Id = FGuid();
};
