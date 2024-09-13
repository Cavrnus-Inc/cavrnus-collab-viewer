// Copyright(c) Cavrnus. All rights reserved.
#include "ValueSyncs/CavrnusValueSyncString.h"
#include <Kismet/KismetSystemLibrary.h>

//===========================================================
UCavrnusValueSyncString::UCavrnusValueSyncString()
{
	PropertyName = TEXT("Text");
}


//===========================================================
UCavrnusValueSyncString::~UCavrnusValueSyncString()
{
}

Cavrnus::FPropertyValue UCavrnusValueSyncString::GetPropertyValue()
{
	return Cavrnus::FPropertyValue::StringPropValue(GetString());
}

void UCavrnusValueSyncString::SetPropertyValue(Cavrnus::FPropertyValue value)
{
	SetString(value.StringValue);
}