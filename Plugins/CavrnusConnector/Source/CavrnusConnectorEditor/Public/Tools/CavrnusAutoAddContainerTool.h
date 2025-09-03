// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SCS_Node.h"
#include "UObject/Object.h"
#include "CavrnusAutoAddContainerTool.generated.h"

/**
 * 
 */
UCLASS()
class CAVRNUSCONNECTOREDITOR_API UCavrnusAutoAddContainerTool : public UObject
{
	GENERATED_BODY()
public:
	void Initialize();
	void Teardown();
	
	void HookAutoAddContainerComponents();
	void UnhookAutoAddContainerComponents();
	void OnAssetOpened(UObject* Asset, IAssetEditorInstance* EditorInstance);
	void OnAssetRequestClose(UObject* Asset, EAssetEditorCloseReason CloseReason);
	void OnBlueprintChanged(UBlueprint* Blueprint);
	FString GenerateContainerName(USceneComponent* ParentContentTemplate, UBlueprint* Blueprint) const;
	FString GenerateContainerName(USCS_Node* ContainerParentNode, UBlueprint* Blueprint) const;

private:
	FDelegateHandle PostEngineInitDelegateHandle;
};
