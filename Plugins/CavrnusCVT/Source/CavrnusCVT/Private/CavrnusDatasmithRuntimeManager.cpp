#include "CavrnusDatasmithRuntimeManager.h"



CavrnusDatasmithRuntimeManager* CavrnusDatasmithRuntimeManager::Instance = nullptr;

CavrnusDatasmithRuntimeManager* CavrnusDatasmithRuntimeManager::GetCavrnusDatasmithRuntimeManager()
{
	if (Instance == nullptr)
	{
		Instance = new CavrnusDatasmithRuntimeManager();
	}

	return Instance;
}