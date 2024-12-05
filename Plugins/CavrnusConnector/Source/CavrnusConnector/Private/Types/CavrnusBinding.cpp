// Copyright (c) 2024 Cavrnus. All rights reserved.

#include "Types/CavrnusBinding.h"

//===========================================================

void UCavrnusBinding::SetupUnbind(TFunction<void()> unhook)
{
	Unhook = new TFunction<void()>(unhook);
}

void UCavrnusBinding::Unbind_Implementation()
{
	if (Unhook != nullptr)
		(*Unhook)();
	Unhook = nullptr;
}
