#include "CavrnusPropertyAssetProcessor.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/UObjectGlobals.h"
//#include "CavrnusDataSmithTransformSync.h"
#include "CavrnusFunctionLibrary.h"
#include "CavrnusCVTGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "DatasmithAssetUserData.h"
#include "IDatasmithSceneElements.h"
#include "DatasmithRuntime.h"
#include "Engine/StaticMeshActor.h"

TMap<UClass*, TArray<FString>> UCavrnusPropertyAssetProcessor::SupportedPropertyMap = {};

void UCavrnusPropertyAssetProcessor::Setup(const FCavrnusSpaceConnection& InSpaceConn)
{
	SpaceConnection = InSpaceConn;
	SingletonInits();

	ProcessScene();
}

void UCavrnusPropertyAssetProcessor::SingletonInits()
{
	if (!SingletonInit)
	{
		SingletonInit = true;
		BuildPropertyMap();
	}
}

void UCavrnusPropertyAssetProcessor::ProcessScene()
{
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), ActorList);
	for (AActor* Actor : ActorList)
	{
		if (Actor->GetClass()->GetName() == "SunSky_C") {
			ProcessActorProperties(Actor, computeHash(Actor));
		}
	}
}

void UCavrnusPropertyAssetProcessor::BuildPropertyMap()
{
	if (!SupportedPropertyMap.IsEmpty())
		return;
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), ActorList);

	// StaticMeshActor
	SupportedPropertyMap.Add(AStaticMeshActor::StaticClass(), { "bHidden" });
	// SunSky Actor
	UClass* SunSkyClass = nullptr;
	for (AActor* Actor : ActorList)
	{
		if (Actor->GetClass()->GetName() == "SunSky_C") {
			SunSkyClass = Actor->GetClass();
			break;
		}
	}
	if (SunSkyClass)
	{
		SupportedPropertyMap.Add(SunSkyClass, { "Latitude","Longitude","Year","Month","Day","TimeZone","UseDaylightSavingTime","NorthOffset","Elevation","CorrectedElevation","Azimuth","SolarTime","DSTStartMonth","DSTStartDay",
												"DSTEndMonth","DSTEndDay","DSTSwitchHour","bHidden","SolarTime","DSTStartMonth","DSTStartDay" });
	}

}

bool UCavrnusPropertyAssetProcessor::isSupportedProperty(UClass* ClassChecked, FString Property)
{
	TArray<FString>* PropertiesSupportedByClass = SupportedPropertyMap.Find(ClassChecked);
	if (PropertiesSupportedByClass && PropertiesSupportedByClass->Contains(Property))
		return true;
	return false;
}



int UCavrnusPropertyAssetProcessor::ProcessTwinmotionDatasmithChildUsingSlotNames(const AActor* Actor)
{
	if (!Actor)
		return 0;
	int FixCount = 0;

	TArray<UStaticMeshComponent*> MeshComponents;
	Actor->GetComponents<UStaticMeshComponent>(MeshComponents, true);

	for (UStaticMeshComponent* MeshComp : MeshComponents)
	{
		if (!MeshComp)
			continue;
		MeshComp->SetForcedLodModel(1);

		TArray<FName> SlotNames = MeshComp->GetMaterialSlotNames();
		int32 NumMaterials = SlotNames.Num();

		bool bModified = false;

		for (int32 i = 0; i < NumMaterials; ++i)
		{
			UMaterialInterface* Material = MeshComp->GetMaterial(i);

			if (!Material)
			{
				for (int j = i + 1; j < NumMaterials; ++j)
				{
					if (SlotNames[i] == SlotNames[j])
					{
						MeshComp->SetMaterial(i, MeshComp->GetMaterial(j));
						//FigureOutMaterialParameters(MeshComp->GetMaterial(j));
						bModified = true;
						FixCount++;
						break;
					}
				}
			}
		}

		if (bModified)
		{
			MeshComp->MarkRenderStateDirty();
		}
	}
	return FixCount;
}



void UCavrnusPropertyAssetProcessor::ProcessDatasmithRuntimeActorProperties(const FString& FilePath, ADatasmithRuntimeActor* DatasmithActor, const FString& Container)
{	
	if (!DatasmithActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("DatasmithActor is null."));
		return;
	}
	if (FilePath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("FilePath for Datasmith Actor is Empty.  Unable to determine type and further process"));
	}
	TArray<AActor*> ActorsToProcess;

	GetAllRelevantActorsRecursive(DatasmithActor, ActorsToProcess);

	UE_LOG(LogTemp, Error, TEXT("XXXXX - Found %d from GetAllStaticMeshActorsRecursive"), ActorsToProcess.Num());

	int TotalFixed = 0;

	ProcessActorProperties(DatasmithActor, Container);

	int count = 0;
	for (AActor* ProcessActor : ActorsToProcess)
	{
		// Make subcontainer ?
		FString NewContainer = Container;
		NewContainer.Append("/").Append(UKismetSystemLibrary::GetObjectName(ProcessActor));
		TotalFixed += ProcessActorProperties(ProcessActor, NewContainer);
		count++;
	}

	if (FilePath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("FilePath for Datasmith Actor is Empty.  Unable to determine type and further process"));
	}
	FString ErrorMessage;
	EDatasmithRuntimeFileType FileType = UDatasmithFileLibrary::GetFileTypeFromDatasmithFile(FilePath, ErrorMessage);

	switch (FileType)
	{
		case EDatasmithRuntimeFileType::Twinmotion :
		{
			// Process Twinmotion specific properties
			TotalFixed += ProcessTwinmotionDatasmithChildUsingSlotNames(DatasmithActor);
			for (AActor* Actor : ActorsToProcess)
			{
				TotalFixed += ProcessTwinmotionDatasmithChildUsingSlotNames(Actor);
			}
			break;
		}
		default:
		{
			// Nothing to do here yet.
		}
	}

	ProcessDatasmithMetadata(FilePath, DatasmithActor, Container);
}

bool UCavrnusPropertyAssetProcessor::isDatasmithChild(const AActor* Actor)
{
	const AActor* ActorPtr = Actor;
	TSet<const AActor*> Visited;

	while (ActorPtr)
	{
		if (Visited.Contains(ActorPtr))
		{
			// Prevent infinite loop in case of circular attachment
			break;
		}
		Visited.Add(ActorPtr);

		if (Cast<ADatasmithRuntimeActor>(ActorPtr))
		{
			return true;
		}

		const USceneComponent* RootComp = ActorPtr->GetRootComponent();
		if (!RootComp)
		{
			break;
		}

		const USceneComponent* ParentComp = RootComp->GetAttachParent();
		if (!ParentComp)
		{
			break;
		}

		ActorPtr = ParentComp->GetOwner();
	}
	return false;
}

void UCavrnusPropertyAssetProcessor::SpecialActorClassProcessing(const AActor* Actor, const FString& Container)
{
	if (isDatasmithChild(Actor))
		ProcessTwinmotionDatasmithChildUsingSlotNames(Actor);
}

int UCavrnusPropertyAssetProcessor::ProcessComponents(const AActor* Actor, const FString& Container)
{
	int processedCount = 0;
	if (!Actor)
		return 0;

	SpecialActorClassProcessing(Actor, Container);
	if (Actor->GetClass() == AStaticMeshActor::StaticClass())
	{
		TArray<UStaticMeshComponent*> StaticMeshComponents;
		Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents, true);
		for (UStaticMeshComponent* MeshComp : StaticMeshComponents)
		{
			if (!MeshComp)
				continue;
			TArray<UMaterialInterface*> Materials = MeshComp->GetMaterials();

			bool bModified = false;

			for (int32 i = 0; i < Materials.Num(); ++i)
			{
				UMaterialInterface* CurrentMat = Materials[i];
				UMaterialInstanceDynamic* DynMaterial = Cast<UMaterialInstanceDynamic>(Materials[i]);
				if (DynMaterial)
				{
					FString NewContainer = Container;
					NewContainer.Append("/").Append(FString::FromInt(i));
					ProcessMaterialParameters(DynMaterial, NewContainer);
				}
			}
		}
	}
	return processedCount;
}

int UCavrnusPropertyAssetProcessor::ProcessMaterialParameters(UMaterialInstanceDynamic* MaterialInstance, const FString& Container)
{
	if (!MaterialInstance)
		return 0;
	int Added = 0;

	UClass* ObjectClass = MaterialInstance->GetClass();
	bool FirstPropertyFound = true;

	// Scalar parameters
	TArray<FMaterialParameterInfo> ScalarParams;
	TArray<FGuid> ScalarGuids;

	MaterialInstance->GetAllScalarParameterInfo(ScalarParams, ScalarGuids);
	UCavrnusFunctionLibrary::DefineStringPropertyDefaultValue(SpaceConnection, Container, "_Class", MaterialInstance->GetClass()->GetName());
	for (FMaterialParameterInfo Scalar : ScalarParams)
	{
		float OutValue;
		MaterialInstance->GetScalarParameterValue(Scalar, OutValue);
		UCavrnusFunctionLibrary::DefineFloatPropertyDefaultValue(SpaceConnection, Container, Scalar.Name.ToString(), OutValue);
		auto OnFloatUpdated = [MaterialInstance, Scalar](float floatVal, FString Container, FString PropertyName)
			{
				MaterialInstance->SetScalarParameterValue(FName(PropertyName), floatVal);
			};
		UCavrnusFunctionLibrary::BindFloatPropertyValue(SpaceConnection, Container, Scalar.Name.ToString(), OnFloatUpdated);
	}

	// Vector parameters
	TArray<FMaterialParameterInfo> VectorParams;
	TArray<FGuid> VectorGuids;
	MaterialInstance->GetAllVectorParameterInfo(VectorParams, VectorGuids);

	for (FMaterialParameterInfo Vectors : VectorParams)
	{
		FLinearColor OutVector;
		MaterialInstance->GetVectorParameterValue(Vectors, OutVector);
		UCavrnusFunctionLibrary::DefineColorPropertyDefaultValue(SpaceConnection, Container, Vectors.Name.ToString(), OutVector);
		auto OnVectorUpdated = [MaterialInstance, Vectors](FLinearColor VectorVal, FString Container, FString PropertyName)
			{
				MaterialInstance->SetVectorParameterValue(FName(PropertyName), VectorVal);
			};
		UCavrnusFunctionLibrary::BindColorPropertyValue(SpaceConnection, Container, Vectors.Name.ToString(), OnVectorUpdated);
	}

	// Texture parameters
	TArray<FMaterialParameterInfo> TextureParams;
	TArray<FGuid> TextureGuids;
	MaterialInstance->GetAllTextureParameterInfo(TextureParams, TextureGuids);

	for (FMaterialParameterInfo Textures : TextureParams)
	{
		class UTexture* OutValue;
		MaterialInstance->GetTextureParameterValue(Textures, OutValue);
		UCavrnusFunctionLibrary::DefineStringPropertyDefaultValue(SpaceConnection, Container, Textures.Name.ToString(), OutValue->GetPathName());
		auto OnTexturePathUpdated = [MaterialInstance, Textures](FString StringVal, FString Container, FString PropertyName)
			{
				UE_LOG(LogTemp, Error, TEXT("Setting Textures from Properties is not supported yet %s %s"), *PropertyName, *StringVal);
			};
		UCavrnusFunctionLibrary::BindStringPropertyValue(SpaceConnection, Container, Textures.Name.ToString(), OnTexturePathUpdated);
	}

#if WITH_EDITOR
	// These are editor only Static switch parameters
	
	TArray<FMaterialParameterInfo> StaticSwitchParams;
	TArray<FGuid> StaticSwitchGuids;
	MaterialInstance->GetAllStaticSwitchParameterInfo(StaticSwitchParams, StaticSwitchGuids);

	for (FMaterialParameterInfo Switches : StaticSwitchParams)
	{
		bool bOut;
		FGuid someGuid;
		MaterialInstance->GetStaticSwitchParameterValue(Switches, bOut, someGuid);
		UCavrnusFunctionLibrary::DefineBoolPropertyDefaultValue(SpaceConnection, Container, Switches.Name.ToString(), bOut);
		auto OnBoolUpdated = [MaterialInstance, Switches](bool bValue, FString Container, FString PropertyName)
			{
				MaterialInstance->SetStaticSwitchParameterValueEditorOnly(FName(PropertyName), bValue);
			};
		UCavrnusFunctionLibrary::BindBooleanPropertyValue(SpaceConnection, Container, Switches.Name.ToString(), OnBoolUpdated);
	}
#endif

	return Added;
}


int UCavrnusPropertyAssetProcessor::ProcessActorProperties(AActor* Actor, const FString& Container)
{
	if (!Actor)
		return 0;

	int Added = 0;

	UClass* ActorClass = Actor->GetClass();
	bool FirstPropertyFound = true;
	
	DeclareActorBoundingBoxProperties(Actor, Container);
	DeclareActorNameProperty(Actor, Container);

	UCavrnusCVTGameSubsystem* CVTSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UCavrnusCVTGameSubsystem>();

	/*
	if (auto* SyncTransform = NewObject<UCavrnusDataSmithTransformSync>())
	{
		FString ActorName = Actor->GetName();
		SyncTransform->Setup(SpaceConnection, Container, "_Transform", Actor);
	}
	*/

	for (TFieldIterator<FProperty> PropIt(ActorClass); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		FString PropertyName = Property->GetName();
		FString PropertyType = Property->GetClass()->GetName();

		if (isSupportedProperty(Actor->GetClass(), PropertyName))
		{
			if (FirstPropertyFound)
			{
				UCavrnusFunctionLibrary::DefineStringPropertyDefaultValue(SpaceConnection, Container, "_Class", Actor->GetClass()->GetName());
				UCavrnusFunctionLibrary::DefineStringPropertyDefaultValue(SpaceConnection, Container, "_Name", Actor->GetName());
				FirstPropertyFound = false;
			}
			// Optional: Get the value for readable types (e.g. FString)
			if (PropertyType == "BoolProperty")
			{
				bool BoolValue = false;
				if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
				{
					BoolValue = BoolProp->GetPropertyValue_InContainer(Actor);
				}
#if WITH_EDITOR
				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					//UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, Property->GetBoolMetaData(pair.Key) ? *FString("True") : *FString("False"));
				}
#endif;
				UCavrnusFunctionLibrary::DefineBoolPropertyDefaultValue(SpaceConnection, Container, PropertyName, BoolValue);
				auto OnBoolUpdated = [Actor, Property](bool bValue, FString Container, FString PropertyName)
					{
						FBoolProperty* BoolProp = CastField<FBoolProperty>(Property);
						BoolProp->SetPropertyValue_InContainer(Actor, bValue);
					};
				UCavrnusFunctionLibrary::BindBooleanPropertyValue(SpaceConnection, Container, PropertyName, OnBoolUpdated);
			}
			else if (PropertyType == "StrProperty")
			{
				FString StringValue = "";
				if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
				{
					StringValue = StrProp->GetPropertyValue_InContainer(Actor);
				}
#if WITH_EDITOR
				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					//UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *Property->GetMetaData(pair.Key));
				}
#endif
				UCavrnusFunctionLibrary::DefineStringPropertyDefaultValue(SpaceConnection, Container, PropertyName, StringValue);
				auto onStringUpdated = [Actor, Property](FString stringValue, FString Container, FString PropertyName)
					{
						FStrProperty* StrProp = CastField<FStrProperty>(Property);
						StrProp->SetPropertyValue_InContainer(Actor, stringValue);

					};
				UCavrnusFunctionLibrary::BindStringPropertyValue(SpaceConnection, Container, PropertyName, onStringUpdated);

			}
			else if (PropertyType == "FloatProperty")
			{
				float FloatValue = 0.0f;
				if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
					FloatValue = FloatProp->GetPropertyValue_InContainer(Actor);
#if WITH_EDITOR
				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					//UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *FString::SanitizeFloat(Property->GetFloatMetaData(pair.Key)));
				}
#endif
				UCavrnusFunctionLibrary::DefineFloatPropertyDefaultValue(SpaceConnection, Container, PropertyName, FloatValue);
				auto onFloatUpdated = [Actor, Property](float FloatValue, FString Container, FString PropertyName)
					{
						FFloatProperty* FloatProp = CastField<FFloatProperty>(Property);
						FloatProp->SetPropertyValue_InContainer(Actor, FloatValue);

					};
				UCavrnusFunctionLibrary::BindFloatPropertyValue(SpaceConnection, Container, PropertyName, onFloatUpdated);

			}
			else if (PropertyType == "IntProperty")
			{
				int32 IntValue = 0;
				if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
					IntValue = IntProp->GetPropertyValue_InContainer(Actor);
#if WITH_EDITOR
				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					//UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *FString::SanitizeFloat(Property->GetIntMetaData(pair.Key)));
				}
#endif
				UCavrnusFunctionLibrary::DefineFloatPropertyDefaultValue(SpaceConnection, Container, PropertyName, IntValue);
				auto onIntUpdated = [Actor, Property](float IntValue, FString Container, FString PropertyName)
					{
						FIntProperty* IntProp = CastField<FIntProperty>(Property);
						IntProp->SetPropertyValue_InContainer(Actor, IntValue);

					};
				UCavrnusFunctionLibrary::BindFloatPropertyValue(SpaceConnection, Container, PropertyName, onIntUpdated);
			}
			else if (PropertyType == "DoubleProperty")
			{
				double DoubleValue = 0.0f;
				if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property))
					DoubleValue = DoubleProp->GetPropertyValue_InContainer(Actor);
#if WITH_EDITOR
				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					//UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *FString::SanitizeFloat(Property->GetDoubleMetaData(pair.Key)));
				}
#endif
				UCavrnusFunctionLibrary::DefineFloatPropertyDefaultValue(SpaceConnection, Container, PropertyName, DoubleValue);

				auto onDoubleUpdated = [Actor, Property](float DoubleValue, FString Container, FString PropertyName)
					{
						FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property);
						DoubleProp->SetPropertyValue_InContainer(Actor, DoubleValue);

					};
				UCavrnusFunctionLibrary::BindFloatPropertyValue(SpaceConnection, Container, PropertyName, onDoubleUpdated);
			}
			else if (PropertyType == "EnumProperty")
			{
				UE_LOG(LogTemp, Error, TEXT("Unable to support property %s of type %s"), *PropertyName, *PropertyType);
				check(0);
			}
			else if (PropertyType == "ByteProperty")
			{

				UE_LOG(LogTemp, Error, TEXT("Unable to support property %s of type %s"), *PropertyName, *PropertyType);
				check(0);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Unable to support property %s of type %s"), *PropertyName, *PropertyType);
				check(0);
			}
			//UE_LOG(LogTemp, Log, TEXT("Property: %s, Type: %s"), *PropertyName, *PropertyType);
			Added++;
		}
	}
	ProcessComponents(Actor, Container);

	return Added;
}


void UCavrnusPropertyAssetProcessor::FixMaterialsOnRuntimeDatasmithActor(ADatasmithRuntimeActor* DatasmithActor)
{
	if (!DatasmithActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("DatasmithActor is null."));
		return;
	}
	if (DatasmithActor->IsReceiving())
	{
		UE_LOG(LogTemp, Warning, TEXT("DatasmithActor is still loading."));
		return;
	}


	int TotalFixed = 0;

	ProcessTwinmotionDatasmithChildUsingSlotNames(DatasmithActor);

	TArray<AStaticMeshActor*> ActorsToProcess;

	FString Container = computeHash(DatasmithActor); // Get from Datasmith Actor component I hope

	GetAllStaticMeshActorsRecursive(DatasmithActor, ActorsToProcess);
	for (AActor* Actor : ActorsToProcess)
	{
		TotalFixed += ProcessTwinmotionDatasmithChildUsingSlotNames(Actor);
	}

	ProcessActorsRecursive(DatasmithActor, ActorsToProcess, Container);
	UE_LOG(LogTemp, Error, TEXT("Found %d Datasmith Children on Actor %s"), ActorsToProcess.Num(), *DatasmithActor->GetName());

}

void UCavrnusPropertyAssetProcessor::GetAllRelevantActorsRecursive(AActor* RootActor, TArray<AActor*>& OutActors)
{
	if (!RootActor)
	{
		return;
	}

	// Use a set to avoid duplicates or circular references
	TSet<AActor*> Visited;
	TQueue<AActor*> ActorQueue;

	ActorQueue.Enqueue(RootActor);
	Visited.Add(RootActor);

	while (!ActorQueue.IsEmpty())
	{
		AActor* CurrentActor = nullptr;
		ActorQueue.Dequeue(CurrentActor);

		if (!CurrentActor)
		{
			continue;
		}

		OutActors.Add(CurrentActor);

		TArray<AActor*> AttachedActors;
		CurrentActor->GetAttachedActors(AttachedActors);

		for (AActor* ChildActor : AttachedActors)
		{
			if (ChildActor && !Visited.Contains(ChildActor))
			{
				Visited.Add(ChildActor);
				ActorQueue.Enqueue(ChildActor);
			}
		}
	}
}

void UCavrnusPropertyAssetProcessor::GetAllStaticMeshActorsRecursive(const AActor* InRoot, TArray<AStaticMeshActor*>& OutMeshActors)
{
	if (!InRoot)
		return;

	TArray<AActor*> AttachedActors;
	InRoot->GetAttachedActors(AttachedActors);

	for (auto* AA : AttachedActors)
	{
		if (!AA)
			continue;

		if (auto* Sma = Cast<AStaticMeshActor>(AA))
			OutMeshActors.Add(Sma);

		GetAllStaticMeshActorsRecursive(AA, OutMeshActors);
	}
}

void UCavrnusPropertyAssetProcessor::ProcessActorsRecursive(const AActor* InRoot, TArray<AStaticMeshActor*>& OutMeshActors, const FString& Container)
{
	if (!InRoot)
		return;

	TArray<AActor*> AttachedActors;
	InRoot->GetAttachedActors(AttachedActors);

	int count = 0;
	for (auto* AA : AttachedActors)
	{
		if (!AA)
			continue;
		FString NewContainer = Container;
		NewContainer.Append("/").Append(UKismetSystemLibrary::GetObjectName(AA));

		if (auto* Sma = Cast<AStaticMeshActor>(AA))
		{
			ProcessActorProperties(Sma, NewContainer);
			ProcessActorsRecursive(Sma, OutMeshActors, NewContainer);
			count++;
		}
	}
}

void UCavrnusPropertyAssetProcessor::DeclareActorBoundingBoxProperties(const AActor* Actor, const FString& Container)
{
	if (Actor)
	{
		FString OriginPropertyName = "_AABBOrigin";
		FString ExtentsPropertyName = "_AABBHalfExtents";
		FString MinBBPropertyName = "_AxisAlignedBoundingBoxMin";
		FString MaxBBPropertyName = "_AxisAlignedBoundingBoxMax";
		FVector origin;
		FVector extents;
		Actor->GetActorBounds(true, origin, extents, true);
		FVector Max = origin + extents;
		FVector Min = origin - extents;
		UCavrnusFunctionLibrary::DefineVectorPropertyDefaultValue(SpaceConnection, Container, OriginPropertyName, origin);
		UCavrnusFunctionLibrary::DefineVectorPropertyDefaultValue(SpaceConnection, Container, ExtentsPropertyName, extents);
		UCavrnusFunctionLibrary::DefineVectorPropertyDefaultValue(SpaceConnection, Container, MinBBPropertyName, Min);
		UCavrnusFunctionLibrary::DefineVectorPropertyDefaultValue(SpaceConnection, Container, MaxBBPropertyName, Max);

	}
}

void UCavrnusPropertyAssetProcessor::DeclareActorNameProperty(const AActor* Actor, const FString& Container)
{
#if WITH_EDITOR
	if (Actor)
	{
		FString NamePropertyName = "DisplayName_";
		UCavrnusFunctionLibrary::DefineStringPropertyDefaultValue(SpaceConnection, Container, NamePropertyName, Actor->GetActorLabel());
	}
#endif
}

void UCavrnusPropertyAssetProcessor::ProcessDatasmithMetadata(const FString& FilePath, const AActor* DatasmithActor, const FString& Container)
{
	if (!DatasmithActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("DatasmithActor is null."));
		return;
	}
	FString ErrorMessage;
	EDatasmithRuntimeFileType FileType = UDatasmithFileLibrary::GetFileTypeFromDatasmithFile(FilePath, ErrorMessage);

	switch (FileType)
	{
		case EDatasmithRuntimeFileType::Twinmotion:
		{

		}
		case EDatasmithRuntimeFileType::Revit:
		{

		}
		default:
		{
			// Nothing to do here yet.
		}

	}
	/*
	const UObject* Object = Actor; // or the component or mesh asset
	const AStaticMeshActor* StaticMesh = Cast<AStaticMeshActor>(Actor);
	UDatasmithAssetUserData* AssetData = StaticMesh->GetStaticMeshComponent()->GetAssetUserData<UDatasmithAssetUserData>();
	for (const TPair<FName, FString>& Pair : AssetData->MetaData)
	{
		UE_LOG(LogTemp, Log, TEXT("Key: %s Value: %s"), *Pair.Key.ToString(), *Pair.Value);
	}
	*/
}
