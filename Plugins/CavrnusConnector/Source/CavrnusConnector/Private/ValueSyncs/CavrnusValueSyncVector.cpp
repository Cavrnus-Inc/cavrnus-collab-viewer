// Copyright(c) Cavrnus. All rights reserved.
#include "ValueSyncs/CavrnusValueSyncVector.h"

//===========================================================
UCavrnusValueSyncVector::UCavrnusValueSyncVector()
{
	PropertyName = TEXT("Vector");
}


//===========================================================
UCavrnusValueSyncVector::~UCavrnusValueSyncVector()
{
}

Cavrnus::FPropertyValue UCavrnusValueSyncVector::GetPropertyValue()
{
	return Cavrnus::FPropertyValue::VectorPropValue(GetVector());
}

void UCavrnusValueSyncVector::SetPropertyValue(Cavrnus::FPropertyValue value)
{
	SetVector(value.VectorValue);
}