// Copyright(c) Cavrnus. All rights reserved.
#include "ValueSyncs/CavrnusValueSyncBoolean.h"
#include <Kismet/KismetSystemLibrary.h>

//===========================================================
UCavrnusValueSyncBoolean::UCavrnusValueSyncBoolean()
{
	PropertyName = TEXT("Boolean");
}


//===========================================================
UCavrnusValueSyncBoolean::~UCavrnusValueSyncBoolean()
{
}

//===========================================================

Cavrnus::FPropertyValue UCavrnusValueSyncBoolean::GetPropertyValue()
{
	return Cavrnus::FPropertyValue::BoolPropValue(GetBoolean());
}

void UCavrnusValueSyncBoolean::SetPropertyValue(Cavrnus::FPropertyValue value)
{
	SetBoolean(value.BoolValue);
}
