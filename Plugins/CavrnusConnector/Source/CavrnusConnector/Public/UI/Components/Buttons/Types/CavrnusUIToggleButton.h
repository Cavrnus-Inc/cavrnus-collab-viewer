// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Components/Buttons/CavrnusUIButton.h"
#include "UI/Components/Buttons/Interfaces/CavrnusToggleButtonInterface.h"
#include "CavrnusUIToggleButton.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Cavrnus Toggle")
class CAVRNUSCONNECTOR_API UCavrnusUIToggleButton : public UCavrnusUIButton, public ICavrnusToggleButtonInterface
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
public:
	virtual void SetToggled(const bool InToggled) override;
	virtual bool GetIsToggled() const override;
	
protected:
	virtual ECavrnusButtonState ResolvePriorityButtonVisualState() override;
	
private:
	bool IsToggled = false;
	
	UFUNCTION()
	void ToggleButtonClicked();
};