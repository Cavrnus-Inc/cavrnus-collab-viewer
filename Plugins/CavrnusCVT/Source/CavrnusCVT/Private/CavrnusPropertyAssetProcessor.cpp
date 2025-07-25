#include "CavrnusPropertyAssetProcessor.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/UObjectGlobals.h"
#include "CavrnusDataSmithTransformSync.h"
#include "CavrnusFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
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
		UE_LOG(LogTemp, Warning, TEXT("Actor Name = %s  : Class = %s"), *Actor->GetName(), *Actor->GetClass()->GetName());
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

	UMaterialInterface* FallbackMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_Fallback.M_Fallback"));
	if (!FallbackMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("Fallback material not found!"));
		return 0;
	}

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
			UMaterialInterface* CurrentMat = MeshComp->GetMaterialByName(SlotNames[i]);
			UMaterialInterface* OtherMat = MeshComp->GetMaterial(i);
			if (OtherMat != CurrentMat)
			{
				int a = 1;
			}
			if (!CurrentMat)
			{
				UE_LOG(LogTemp, Error, TEXT("Missing Material with SlotName %s"), *SlotNames[i].ToString());
				UE_LOG(LogTemp, Warning, TEXT("Missing material on actor %s, component %s, slot %d"),
					*Actor->GetName(), *MeshComp->GetName(), i);
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

void UCavrnusPropertyAssetProcessor::ProcessRuntimeDatasmithActorProperties(AActor* Actor, const FString& Container)
{
	ADatasmithRuntimeActor* DatasmithActor = Cast<ADatasmithRuntimeActor>(Actor);
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

	TArray<AStaticMeshActor*> ActorsToProcess;

	GetAllStaticMeshActorsRecursive(DatasmithActor, ActorsToProcess);

	UE_LOG(LogTemp, Error, TEXT("Found %d Datasmith Children on Actor %s"), ActorsToProcess.Num(), *DatasmithActor->GetName());

	int TotalFixed = 0;

	ProcessActorProperties(DatasmithActor, Container);

	int count = 0;
	for (AActor* Actor : ActorsToProcess)
	{
		// Make subcontainer ?
		FString NewContainer = Container;
		NewContainer.Append("/").Append(FString::FromInt(count));
		TotalFixed += ProcessActorProperties(Actor, NewContainer);
		count++;
	}

	UE_LOG(LogTemp, Log, TEXT("Finished fixing materials. Total slots fixed: %d"), TotalFixed);
}

int UCavrnusPropertyAssetProcessor::ProcessComponents(const AActor* Actor, const FString& Container)
{
	int processedCount = 0;
	if (!Actor)
		return 0;
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
	UE_LOG(LogTemp, Error, TEXT("--------------------------\nActor = %s"), *MaterialInstance->GetName());

	UClass* ObjectClass = MaterialInstance->GetClass();
	bool FirstPropertyFound = true;

	// Scalar parameters
	TArray<FMaterialParameterInfo> ScalarParams;
	TArray<FGuid> ScalarGuids;

	MaterialInstance->GetAllScalarParameterInfo(ScalarParams, ScalarGuids);
	UE_LOG(LogTemp, Warning, TEXT("Material %s "), *MaterialInstance->GetName());
	UCavrnusFunctionLibrary::PostStringPropertyUpdate(SpaceConnection, Container, "Class", MaterialInstance->GetClass()->GetName());
	UE_LOG(LogTemp, Error, TEXT("XXXPMP - %s"), *Container);
	for (FMaterialParameterInfo Scalar : ScalarParams)
	{
		float OutValue;
		MaterialInstance->GetScalarParameterValue(Scalar, OutValue);
		UCavrnusFunctionLibrary::DefineFloatPropertyDefaultValue(SpaceConnection, Container, Scalar.Name.ToString(), OutValue);
		auto OnFloatUpdated = [MaterialInstance, Scalar](float floatVal, FString Container, FString PropertyName)
			{
				MaterialInstance->SetScalarParameterValue(FName(PropertyName), floatVal);
				UE_LOG(LogTemp, Error, TEXT("Setting %s to %s"), *PropertyName, *FString::SanitizeFloat(floatVal));
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
				UE_LOG(LogTemp, Error, TEXT("Setting %s to %s"), *PropertyName, *VectorVal.ToString());
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

	// These are editor only so commenting out
	// Static switch parameters
	/*
	TArray<FMaterialParameterInfo> StaticSwitchParams;
	TArray<FGuid> StaticSwitchGuids;
	MaterialInstance->GetAllStaticSwitchParameterInfo(StaticSwitchParams, StaticSwitchGuids);
	FString SwitchesParamsString = "Switch Params : ";

	for (FMaterialParameterInfo Switches : StaticSwitchParams)
	{
		bool bOut;
		FGuid someGuid;
		MaterialInstance->GetStaticSwitchParameterValue(Switches, bOut, someGuid);
		UCavrnusFunctionLibrary::PostBoolPropertyUpdate(SpaceConnection, Container, Switches.Name.ToString(), bOut);

		auto OnBoolUpdated = [MaterialInstance, Switches](bool bValue, FString Container, FString PropertyName)
			{
				MaterialInstance->SetStaticSwitchParameterValueEditorOnly(FName(PropertyName), bValue);
			};
		UCavrnusFunctionLibrary::BindBooleanPropertyValue(SpaceConnection, Container, Switches.Name.ToString(), OnBoolUpdated);
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *SwitchesParamsString);
	*/

	return Added;
}

int UCavrnusPropertyAssetProcessor::ProcessActorProperties(AActor* Actor, const FString& Container)
{
	if (!Actor)
		return 0;
	int Added = 0;
	UE_LOG(LogTemp, Error, TEXT("--------------------------\nActor = %s"), *Actor->GetName());

	UClass* ActorClass = Actor->GetClass();
	bool FirstPropertyFound = true;
	
	if (AStaticMeshActor* MeshActor = Cast<AStaticMeshActor>(Actor))
	{
		if (auto* SyncTransform = NewObject<UCavrnusDataSmithTransformSync>())
		{
			FString ActorName = MeshActor->GetName();
			SyncTransform->Setup(SpaceConnection, Container, "Transform", MeshActor);
			// Need to add SyncTransform to a destroyer
		}
	}
	for (TFieldIterator<FProperty> PropIt(ActorClass); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		FString PropertyName = Property->GetName();
		FString PropertyType = Property->GetClass()->GetName();

		UE_LOG(LogTemp, Error, TEXT("Class = %s  Property = %s"), *Actor->GetClass()->GetName(), *PropertyName);
		if (isSupportedProperty(Actor->GetClass(), PropertyName))
		{
			if (FirstPropertyFound)
			{
				UCavrnusFunctionLibrary::PostStringPropertyUpdate(SpaceConnection, Container, "Class", Actor->GetClass()->GetName());
				UCavrnusFunctionLibrary::PostStringPropertyUpdate(SpaceConnection, Container, "Name", Actor->GetName());
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
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, Property->GetBoolMetaData(pair.Key) ? *FString("True") : *FString("False"));
				}
#endif;
				UE_LOG(LogTemp, Error, TEXT("XXXBool - Container = %s--"), *Container);
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
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *Property->GetMetaData(pair.Key));
				}
#endif
				UE_LOG(LogTemp, Error, TEXT("XXXStr - Container = %s--"), *Container);
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
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *FString::SanitizeFloat(Property->GetFloatMetaData(pair.Key)));
				}
#endif
				UE_LOG(LogTemp, Error, TEXT("XXXFlt - Container = %s--"), *Container);
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
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *FString::SanitizeFloat(Property->GetIntMetaData(pair.Key)));
				}
#endif
				UE_LOG(LogTemp, Error, TEXT("XXXInt - Container = %s--"), *Container);
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
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *FString::SanitizeFloat(Property->GetDoubleMetaData(pair.Key)));
				}
#endif
				UE_LOG(LogTemp, Error, TEXT("XXXDBL - Container = %s--"), *Container);
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
			UE_LOG(LogTemp, Log, TEXT("Property: %s, Type: %s"), *PropertyName, *PropertyType);
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



	UE_LOG(LogTemp, Log, TEXT("Finished fixing materials. Total slots fixed: %d"), TotalFixed);
}


void UCavrnusPropertyAssetProcessor::DoDebugFunction0()
{
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADatasmithRuntimeActor::StaticClass(), FoundActors);

	for (int i = 0; i < FoundActors.Num(); i++)
	{
		ADatasmithRuntimeActor* DActor = Cast<ADatasmithRuntimeActor>(FoundActors[i]);
		FString BaseContainer = computeHash(DActor);
		if (DActor)
		{
			FixMaterialsOnRuntimeDatasmithActor(DActor);

			TArray<AStaticMeshActor*> ActorsToProcess;

			GetAllStaticMeshActorsRecursive(DActor, ActorsToProcess);
			int count = 0;
			for (AActor* Actor : ActorsToProcess)
			{
				// Make Subcontainer
				FString NewContainer = BaseContainer;
				NewContainer.Append("/").Append(FString::FromInt(count));
				UE_LOG(LogTemp, Error, TEXT("XXXDDF - Container = %s--"), *NewContainer);
				ProcessActorProperties(Actor, NewContainer);
				count++;
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
		NewContainer.Append("/").Append(FString::FromInt(count));

		if (auto* Sma = Cast<AStaticMeshActor>(AA))
		{
			UE_LOG(LogTemp, Error, TEXT("XXXPAR - Container = %s--"), *NewContainer);
			ProcessActorProperties(Sma, NewContainer);
			ProcessActorsRecursive(Sma, OutMeshActors, NewContainer);
			count++;
		}
	}
}