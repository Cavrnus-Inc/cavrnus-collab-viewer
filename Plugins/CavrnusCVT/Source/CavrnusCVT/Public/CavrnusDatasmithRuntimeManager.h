#pragma once
class CAVRNUSCVT_API CavrnusDatasmithRuntimeManager
{
public:
	CavrnusDatasmithRuntimeManager() {};

private:
	static CavrnusDatasmithRuntimeManager* GetCavrnusDatasmithRuntimeManager();
	static CavrnusDatasmithRuntimeManager* Instance;

};

