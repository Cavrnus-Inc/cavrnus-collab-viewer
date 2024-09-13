// Copyright(c) Cavrnus. All rights reserved.
#include "ValueSyncs/CavrnusValueSyncColor.h"
#include <Kismet/KismetSystemLibrary.h>

//===========================================================
UCavrnusValueSyncColor::UCavrnusValueSyncColor()
{
	PropertyName = TEXT("Color");
}


//===========================================================
UCavrnusValueSyncColor::~UCavrnusValueSyncColor()
{
}

Cavrnus::FPropertyValue UCavrnusValueSyncColor::GetPropertyValue()
{
	return Cavrnus::FPropertyValue::ColorPropValue(GetColor());
}

void UCavrnusValueSyncColor::SetPropertyValue(Cavrnus::FPropertyValue value)
{
	SetColor(value.ColorValue);
}