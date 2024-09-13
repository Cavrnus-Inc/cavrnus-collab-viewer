// Copyright(c) Cavrnus. All rights reserved.
#include "ValueSyncs/CavrnusValueSyncFloat.h"
#include <Kismet/KismetSystemLibrary.h>

//===========================================================
UCavrnusValueSyncFloat::UCavrnusValueSyncFloat()
{
	PropertyName = TEXT("Float");
}


//===========================================================
UCavrnusValueSyncFloat::~UCavrnusValueSyncFloat()
{
}

Cavrnus::FPropertyValue UCavrnusValueSyncFloat::GetPropertyValue()
{
	return Cavrnus::FPropertyValue::FloatPropValue(GetFloat());
}

void UCavrnusValueSyncFloat::SetPropertyValue(Cavrnus::FPropertyValue value)
{
	SetFloat(value.FloatValue);
}