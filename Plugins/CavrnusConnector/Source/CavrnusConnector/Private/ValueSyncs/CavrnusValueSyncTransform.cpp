// Copyright(c) Cavrnus. All rights reserved.
#include "ValueSyncs/CavrnusValueSyncTransform.h"
#include <Kismet/KismetSystemLibrary.h>

//===========================================================
UCavrnusValueSyncTransform::UCavrnusValueSyncTransform()
{
	PropertyName = TEXT("Transform");
}


//===========================================================
UCavrnusValueSyncTransform::~UCavrnusValueSyncTransform()
{
}

Cavrnus::FPropertyValue UCavrnusValueSyncTransform::GetPropertyValue()
{
	return Cavrnus::FPropertyValue::TransformPropValue(GetTransform());
}

void UCavrnusValueSyncTransform::SetPropertyValue(Cavrnus::FPropertyValue value)
{
	SetTransform(value.TransformValue);
}