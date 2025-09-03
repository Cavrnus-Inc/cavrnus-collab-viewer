// // Copyright (c) 2025 Cavrnus. All rights reserved.


#include "Tools/CavrnusAutoAddContainerTool.h"

#include "CavrnusPropertiesContainer.h"
#include "Engine/SCS_Node.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ValueSyncs/CavrnusValueSyncBase.h"

void UCavrnusAutoAddContainerTool::Initialize()
{
}

void UCavrnusAutoAddContainerTool::Teardown()
{
}

void UCavrnusAutoAddContainerTool::HookAutoAddContainerComponents()
{
	PostEngineInitDelegateHandle = FCoreDelegates::OnPostEngineInit.AddLambda([this]() {
		if (GEditor)
		{
			if (UAssetEditorSubsystem* EditorSubSystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
			{
				EditorSubSystem->OnAssetOpenedInEditor().AddUObject(this, &UCavrnusAutoAddContainerTool::OnAssetOpened);
				EditorSubSystem->OnAssetEditorRequestClose().AddUObject(this, &UCavrnusAutoAddContainerTool::OnAssetRequestClose);
			}
		}
	});
}

void UCavrnusAutoAddContainerTool::UnhookAutoAddContainerComponents()
{
	if (PostEngineInitDelegateHandle.IsValid())
	{
		FCoreDelegates::OnPostEngineInit.Remove(PostEngineInitDelegateHandle);
	}

	if (GEditor)
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetOpenedInEditor().RemoveAll(this);
	}
}

void UCavrnusAutoAddContainerTool::OnAssetOpened(UObject* Asset, IAssetEditorInstance* EditorInstance)
{
	// Need to get hold of the blueprint and register to OnBlueprintChanged();
	if (UBlueprint* Blueprint = Cast<UBlueprint>(Asset))
	{
		Blueprint->OnChanged().AddUObject(this, &UCavrnusAutoAddContainerTool::OnBlueprintChanged);
	}
}

void UCavrnusAutoAddContainerTool::OnAssetRequestClose(UObject* Asset, EAssetEditorCloseReason CloseReason)
{
	if (UBlueprint* Blueprint = Cast<UBlueprint>(Asset))
	{
		Blueprint->OnChanged().RemoveAll(this);
	}
}

void UCavrnusAutoAddContainerTool::OnBlueprintChanged(UBlueprint* Blueprint)
{
	// Search the components and auto attach containers
	if (!Blueprint || !Blueprint->SimpleConstructionScript)
		return;

	bool bStructurallyModified = false;
	TArray<USCS_Node*> BlueprintNodes = Blueprint->SimpleConstructionScript->GetAllNodes();
	TArray<USCS_Node*> NodesNeedingContainer;
	for (USCS_Node* Node : BlueprintNodes)
	{
		if (!Node->ComponentTemplate->IsA<UCavrnusValueSyncBase>())
			continue;

		// Need to check siblings for a pre-existing container
		bool bParentNeedsContainer = true;
		AActor* CDO = Cast<AActor>(Blueprint->GeneratedClass->GetDefaultObject(false));
		if (CDO)
		{
			TArray<USCS_Node*> SiblingNodes;
			if (Node->bIsParentComponentNative)
			{
				USceneComponent* ParentContentTemplate = Node->GetParentComponentTemplate(Blueprint);
				if (ParentContentTemplate)
				{
					SiblingNodes = BlueprintNodes.FilterByPredicate([Node, Blueprint, ParentContentTemplate](USCS_Node* BPNode) {
						return BPNode != Node && BPNode->GetParentComponentTemplate(Blueprint) == ParentContentTemplate;
						});
				}
			}
			else
			{
				USCS_Node* Parent = Blueprint->SimpleConstructionScript->FindParentNode(Node);
				if (Parent)
				{
					SiblingNodes = Parent->ChildNodes;
				}
			}

			for (USCS_Node* Sibling : SiblingNodes)
			{
				if (Sibling->ComponentTemplate->IsA<UCavrnusPropertiesContainer>())
				{
					bParentNeedsContainer = false;
					break;
				}
			}
		}

		if (bParentNeedsContainer)
		{
			UCavrnusPropertiesContainer* PropertiesContainer = NewObject<UCavrnusPropertiesContainer>();

			USCS_Node* NewNode = Blueprint->SimpleConstructionScript->CreateNode(PropertiesContainer->GetClass(), *PropertiesContainer->GetName());
			if (Node->bIsParentComponentNative)
			{
				Blueprint->SimpleConstructionScript->AddNode(NewNode);
				USceneComponent* ParentComponentTemplate = Node->GetParentComponentTemplate(Blueprint);
				if (ParentComponentTemplate)
				{
					PropertiesContainer->ContainerName = GenerateContainerName(ParentComponentTemplate, Blueprint);
					NewNode->SetParent(Cast<USceneComponent>(ParentComponentTemplate));
				}
			}
			else
			{
				USCS_Node* Parent = Blueprint->SimpleConstructionScript->FindParentNode(Node);
				PropertiesContainer->ContainerName = GenerateContainerName(Parent, Blueprint);
				if (Parent)
				{
					Parent->AddChildNode(NewNode);
				}
			}

			UEditorEngine::CopyPropertiesForUnrelatedObjects(PropertiesContainer, NewNode->ComponentTemplate);

			bStructurallyModified = true;

		}
	}

	if (bStructurallyModified)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	}
}


FString UCavrnusAutoAddContainerTool::GenerateContainerName(USceneComponent* ParentContentTemplate, UBlueprint* Blueprint) const
{
	USceneComponent* Current = ParentContentTemplate;
	FString ContainerName;

	do
	{
		FString ObjectName = Current->GetName();
		ObjectName.RemoveFromEnd(TEXT("_GEN_VARIABLE"));
		if (ContainerName.IsEmpty())
		{
			ContainerName = ObjectName;
		}
		else
		{
			ContainerName = ObjectName + "/" + ContainerName;
		}

		Current = Current->GetAttachParent();
	} while (Current != nullptr);

	ContainerName = Blueprint->GetName() + "/" + ContainerName;

	return ContainerName;
}

FString UCavrnusAutoAddContainerTool::GenerateContainerName(USCS_Node* ContainerParentNode, UBlueprint* Blueprint) const
{
	USCS_Node* CurrentNode = ContainerParentNode;
	FString ContainerName;

	do
	{
		FString ObjectName = CurrentNode->ComponentTemplate->GetName();
		ObjectName.RemoveFromEnd(TEXT("_GEN_VARIABLE"));
		if (ContainerName.IsEmpty())
		{
			ContainerName = ObjectName;
		}
		else
		{
			ContainerName = ObjectName + "/" + ContainerName;
		}

		CurrentNode = Blueprint->SimpleConstructionScript->FindParentNode(CurrentNode);
	} while (CurrentNode != nullptr);

	ContainerName = Blueprint->GetName() + "/" + ContainerName;

	return ContainerName;
}

