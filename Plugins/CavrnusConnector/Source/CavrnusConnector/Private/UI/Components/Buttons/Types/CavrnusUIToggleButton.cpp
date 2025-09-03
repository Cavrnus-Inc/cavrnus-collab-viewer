// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Components/Buttons/Types/CavrnusUIToggleButton.h"
#include "Components/Button.h"
#include "UI/Components/Buttons/CavrnusButtonState.h"

void UCavrnusUIToggleButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button)
		Button->OnClicked.AddDynamic(this, &UCavrnusUIToggleButton::ToggleButtonClicked);
}

void UCavrnusUIToggleButton::NativeDestruct()
{
	Super::NativeDestruct();

	if (Button)
		Button->OnClicked.RemoveDynamic(this, &UCavrnusUIToggleButton::ToggleButtonClicked);
}

void UCavrnusUIToggleButton::SetToggled(const bool InToggled)
{
	IsToggled = InToggled;
	ResolvePriorityButtonVisualState();
}

bool UCavrnusUIToggleButton::GetIsToggled() const
{
	return IsToggled;
}

ECavrnusButtonState UCavrnusUIToggleButton::ResolvePriorityButtonVisualState()
{
	if (IsDisabled) return ECavrnusButtonState::Disabled;
	if (IsPressed)  return ECavrnusButtonState::Pressed;
	if (IsHovered)  return ECavrnusButtonState::Hovered;
	if (IsToggled)  return ECavrnusButtonState::ToggledOn;

	return ECavrnusButtonState::Normal;
}

void UCavrnusUIToggleButton::ToggleButtonClicked()
{
	SetToggled(!IsToggled);
}
