#include "UI/Helpers/CavrnusWidgetFactory.h"
#include "CavrnusConnectorModule.h"

UClass* FCavrnusWidgetFactory::GetDefaultBlueprint(const FString& Path, UClass* BaseClass)
{
	// Use BP as default value
	UClass* LoadedBlueprintClass = StaticLoadClass(BaseClass, nullptr, *Path, nullptr, LOAD_None, nullptr);
	if (!LoadedBlueprintClass)
		UE_LOG(LogCavrnusConnector, Error, TEXT("Blueprint asset failed to load from path: %s, base class name: %s"), *Path, *BaseClass->GetName());

	return LoadedBlueprintClass;
}
