// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "CavrnusDataSmithPropertiesHandler.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/UObjectGlobals.h"
#include "CavrnusDataSmithTransformSync.h"
#include "CavrnusFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"

TMap<UClass*, TArray<FString>> UCavrnusDataSmithPropertiesHandler::SupportedPropertyMap = {};
int UCavrnusDataSmithPropertiesHandler::SingletonInit = false;;

void UCavrnusDataSmithPropertiesHandler::Setup(const FCavrnusSpaceConnection& InSpaceConn, const FString& InContainerName, AActor* InDataSmithActor)
{
	SpaceConnection = InSpaceConn;
	ContainerName = InContainerName;
	DataSmithActor = InDataSmithActor;
	
	if (const UWorld* World = DataSmithActor->GetWorld())
	{
		World->GetTimerManager().SetTimer(CheckHierarchyHandle, FTimerDelegate::CreateUObject(this,
		&UCavrnusDataSmithPropertiesHandler::CheckHierarchyExists), 0.5f, true);
	}
	SingletonInits();
	//For debugging things without Cavrnus running:
	if (auto pc = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		pc->InputComponent->BindKey(EKeys::Slash, IE_Pressed, this, &UCavrnusDataSmithPropertiesHandler::DoDebugFunction0);
		//pc->InputComponent->BindKey(EKeys::O, IE_Pressed, this, &ACavrnusTwinmotionSetup::DoDebugFunction1);

		//BindReesourceCreationDestructionEvents();
	}
}

void UCavrnusDataSmithPropertiesHandler::SingletonInits()
{
	if (!SingletonInit)
	{
		SingletonInit = true;
		BuildPropertyMap();
	}
}

void UCavrnusDataSmithPropertiesHandler::BuildPropertyMap()
{
	if (!SupportedPropertyMap.IsEmpty())
		return;
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), ActorList);

	// StaticMeshActor
	SupportedPropertyMap.Add(AStaticMeshActor::StaticClass(), { "bHidden" });
	// SunSky Actor
	UClass* SunSkyClass=nullptr;
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

	for (AActor* Actor : ActorList)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Name = %s  : Class = %s"), *Actor->GetName(), *Actor->GetClass()->GetName());
		if (Actor->GetClass()->GetName() == "SunSky_C") {
			ProcessActorProperties(Actor, FPropertiesContainer(Actor->GetActorGuid().ToString()));
		}
	}
}

bool UCavrnusDataSmithPropertiesHandler::isSupportedProperty(UClass* ClassChecked, FString Property)
{
	TArray<FString>* PropertiesSupportedByClass = SupportedPropertyMap.Find(ClassChecked);
	if (PropertiesSupportedByClass && PropertiesSupportedByClass->Contains(Property))
		return true;
	return false;
}

void UCavrnusDataSmithPropertiesHandler::BeginDestroy()
{
	UObject::BeginDestroy();
	TransformSyncs.Empty();
}

void UCavrnusDataSmithPropertiesHandler::CheckHierarchyExists()
{
	TArray<AActor*> AttachedActors;
	if (Cast<ADatasmithRuntimeActor>(DataSmithActor)->IsReceiving())
	{
		UE_LOG(LogTemp, Verbose, TEXT("Waiting for Datasmith hierarchy to load..."));
		return;
	}
	DataSmithActor->GetAttachedActors(AttachedActors);
	if (AttachedActors.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("FAILURE!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Datasmith hierarchy is ready! Found %d actors."), AttachedActors.Num());

	// Stop polling
	if (const UWorld* World = DataSmithActor->GetWorld())
		World->GetTimerManager().ClearTimer(CheckHierarchyHandle);

	ProcessStaticMeshHierarchy();
	FixMaterialsOnRuntimeDatasmithActor(Cast<ADatasmithRuntimeActor>(DataSmithActor));
}

void ProcessMaterialParameters(UMaterialInterface* MaterialInstance)
{
	// Scalar parameters
	TArray<FMaterialParameterInfo> ScalarParams;
	TArray<FGuid> ScalarGuids;
	if (!MaterialInstance)
		return;
	MaterialInstance->GetAllScalarParameterInfo(ScalarParams, ScalarGuids);
	UE_LOG(LogTemp, Warning, TEXT("Material %s "), *MaterialInstance->GetName());
	FString ScalarParamsString = "Scalar Params : ";
		
	for (FMaterialParameterInfo Scalar : ScalarParams)
	{
		ScalarParamsString.Append(Scalar.Name.ToString()).Append(", ");
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ScalarParamsString);

	// Vector parameters
	TArray<FMaterialParameterInfo> VectorParams;
	TArray<FGuid> VectorGuids;
	MaterialInstance->GetAllVectorParameterInfo(VectorParams, VectorGuids);

	FString VectorParamsString = "Vectors Params : ";

	for (FMaterialParameterInfo Vectors : VectorParams)
	{
		VectorParamsString.Append(Vectors.Name.ToString()).Append(", ");
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *VectorParamsString);

	// Texture parameters
	TArray<FMaterialParameterInfo> TextureParams;
	TArray<FGuid> TextureGuids;
	MaterialInstance->GetAllTextureParameterInfo(TextureParams, TextureGuids);
	FString TextureParamsString = "Texture Params : ";

	for (FMaterialParameterInfo Textures : TextureParams)
	{
		VectorParamsString.Append(Textures.Name.ToString()).Append(", ");
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *TextureParamsString);

	// Static switch parameters
	TArray<FMaterialParameterInfo> StaticSwitchParams;
	TArray<FGuid> StaticSwitchGuids;
	MaterialInstance->GetAllStaticSwitchParameterInfo(StaticSwitchParams, StaticSwitchGuids);
	FString SwitchesParamsString = "Switch Params : ";

	for (FMaterialParameterInfo Switches : StaticSwitchParams)
	{
		SwitchesParamsString.Append(Switches.Name.ToString()).Append(", ");
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *SwitchesParamsString);

}

int UCavrnusDataSmithPropertiesHandler::ProcessTwinmotionDatasmithChildUsingSlotNames(const AActor* Actor)
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

void UCavrnusDataSmithPropertiesHandler::ProcessRuntimeDatasmithActorProperties(ADatasmithRuntimeActor* DatasmithActor)
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

	TArray<AStaticMeshActor*> ActorsToProcess;

	GetAllStaticMeshActorsRecursive(DatasmithActor, ActorsToProcess);

	UE_LOG(LogTemp, Error, TEXT("Found %d Datasmith Children on Actor %s"), ActorsToProcess.Num(), *DatasmithActor->GetName());

	int TotalFixed = 0;

	ProcessActorProperties(DatasmithActor, FPropertiesContainer(DatasmithActor->GetActorGuid().ToString()));

	for (AActor* Actor : ActorsToProcess)
	{
		TotalFixed += ProcessActorProperties(Actor, Actor->GetActorGuid().ToString());
	}

	UE_LOG(LogTemp, Log, TEXT("Finished fixing materials. Total slots fixed: %d"), TotalFixed);
}

int UCavrnusDataSmithPropertiesHandler::ProcessComponents(const AActor* Actor, const FPropertiesContainer& Container)
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
				ProcessMaterialParameters(CurrentMat, Container);
			}
		}
	}
	return processedCount;
}

int UCavrnusDataSmithPropertiesHandler::ProcessMaterialParameters(UMaterialInterface* MaterialInstance, const FPropertiesContainer& Container)
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

	for (FMaterialParameterInfo Scalar : ScalarParams)
	{
		float OutValue;
		MaterialInstance->GetScalarParameterValue(Scalar, OutValue);
		UCavrnusFunctionLibrary::PostFloatPropertyUpdate(SpaceConnection, Container, Scalar.Name.ToString(), OutValue);
	}

	// Vector parameters
	TArray<FMaterialParameterInfo> VectorParams;
	TArray<FGuid> VectorGuids;
	MaterialInstance->GetAllVectorParameterInfo(VectorParams, VectorGuids);

	for (FMaterialParameterInfo Vectors : VectorParams)
	{
		FLinearColor OutVector;
		MaterialInstance->GetVectorParameterValue(Vectors, OutVector );
		UCavrnusFunctionLibrary::PostColorPropertyUpdate(SpaceConnection, Container, Vectors.Name.ToString(), OutVector);
	}

	// Texture parameters
	TArray<FMaterialParameterInfo> TextureParams;
	TArray<FGuid> TextureGuids;
	MaterialInstance->GetAllTextureParameterInfo(TextureParams, TextureGuids);
	FString TextureParamsString = "Texture Params : ";

	for (FMaterialParameterInfo Textures : TextureParams)
	{
		TextureParamsString.Append(Textures.Name.ToString()).Append(", ");
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *TextureParamsString);

	// Static switch parameters
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
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *SwitchesParamsString);
	for (TFieldIterator<FProperty> PropIt(ObjectClass); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		FString PropertyName = Property->GetName();
		FString PropertyType = Property->GetClass()->GetName();

		UE_LOG(LogTemp, Error, TEXT("Class = %s  Property = %s"), *MaterialInstance->GetClass()->GetName(), *PropertyName);
		if (isSupportedProperty(MaterialInstance->GetClass(), PropertyName))
		{
			if (FirstPropertyFound)
			{
				UCavrnusFunctionLibrary::PostStringPropertyUpdate(SpaceConnection, Container, "Class", MaterialInstance->GetClass()->GetName());
				UCavrnusFunctionLibrary::PostStringPropertyUpdate(SpaceConnection, Container, "Name", MaterialInstance->GetName());
				FirstPropertyFound = false;
			}
			// Optional: Get the value for readable types (e.g. FString)
			if (PropertyType == "BoolProperty")
			{
				bool BoolValue = false;
				if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
				{
					BoolValue = BoolProp->GetPropertyValue_InContainer(MaterialInstance);
				}
				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, Property->GetBoolMetaData(pair.Key) ? *FString("True") : *FString("False"));
				}

				UCavrnusFunctionLibrary::PostBoolPropertyUpdate(SpaceConnection, Container, PropertyName, BoolValue);
				auto OnBoolUpdated = [MaterialInstance, Property](bool bValue, FString Container, FString PropertyName)
					{
						FBoolProperty* BoolProp = CastField<FBoolProperty>(Property);
						BoolProp->SetPropertyValue_InContainer(MaterialInstance, bValue);
					};
				UCavrnusFunctionLibrary::BindBooleanPropertyValue(SpaceConnection, Container, PropertyName, OnBoolUpdated);
			}
			else if (PropertyType == "StrProperty")
			{
				FString StringValue = "";
				if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
				{
					StringValue = StrProp->GetPropertyValue_InContainer(MaterialInstance);
				}

				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *Property->GetMetaData(pair.Key));
				}
				UCavrnusFunctionLibrary::PostStringPropertyUpdate(SpaceConnection, Container, PropertyName, StringValue);
				auto onStringUpdated = [MaterialInstance, Property](FString stringValue, FString Container, FString PropertyName)
					{
						FStrProperty* StrProp = CastField<FStrProperty>(Property);
						StrProp->SetPropertyValue_InContainer(MaterialInstance, stringValue);

					};
				UCavrnusFunctionLibrary::BindStringPropertyValue(SpaceConnection, Container, PropertyName, onStringUpdated);

			}
			else if (PropertyType == "FloatProperty")
			{
				float FloatValue = 0.0f;
				if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
					FloatValue = FloatProp->GetPropertyValue_InContainer(MaterialInstance);
				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *FString::SanitizeFloat(Property->GetFloatMetaData(pair.Key)));
				}

				UCavrnusFunctionLibrary::PostFloatPropertyUpdate(SpaceConnection, Container, PropertyName, FloatValue);
				auto onFloatUpdated = [MaterialInstance, Property](float FloatValue, FString Container, FString PropertyName)
					{
						FFloatProperty* FloatProp = CastField<FFloatProperty>(Property);
						FloatProp->SetPropertyValue_InContainer(MaterialInstance, FloatValue);

					};
				UCavrnusFunctionLibrary::BindFloatPropertyValue(SpaceConnection, Container, PropertyName, onFloatUpdated);

			}
			else if (PropertyType == "IntProperty")
			{
				int32 IntValue = 0;
				if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
					IntValue = IntProp->GetPropertyValue_InContainer(MaterialInstance);

				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *FString::SanitizeFloat(Property->GetIntMetaData(pair.Key)));
				}
				UCavrnusFunctionLibrary::PostFloatPropertyUpdate(SpaceConnection, Container, PropertyName, IntValue);

				auto onIntUpdated = [MaterialInstance, Property](float IntValue, FString Container, FString PropertyName)
					{
						FIntProperty* IntProp = CastField<FIntProperty>(Property);
						IntProp->SetPropertyValue_InContainer(MaterialInstance, IntValue);

					};
				UCavrnusFunctionLibrary::BindFloatPropertyValue(SpaceConnection, Container, PropertyName, onIntUpdated);
			}
			else if (PropertyType == "DoubleProperty")
			{
				double DoubleValue = 0.0f;
				if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property))
					DoubleValue = DoubleProp->GetPropertyValue_InContainer(MaterialInstance);
				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *FString::SanitizeFloat(Property->GetDoubleMetaData(pair.Key)));
				}
				UCavrnusFunctionLibrary::PostFloatPropertyUpdate(SpaceConnection, Container, PropertyName, DoubleValue);

				auto onDoubleUpdated = [MaterialInstance, Property](float DoubleValue, FString Container, FString PropertyName)
					{
						FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property);
						DoubleProp->SetPropertyValue_InContainer(MaterialInstance, DoubleValue);

					};
				UCavrnusFunctionLibrary::BindFloatPropertyValue(SpaceConnection, Container, PropertyName, onDoubleUpdated);
			}
			else if (PropertyType == "EnumProperty")
			{
				UE_LOG(LogTemp, Error, TEXT("Unable to support property %s of type %s"), *PropertyName, *PropertyType);

			}
			else if (PropertyType == "ByteProperty")
			{

				UE_LOG(LogTemp, Error, TEXT("Unable to support property %s of type %s"), *PropertyName, *PropertyType);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Unable to support property %s of type %s"), *PropertyName, *PropertyType);
			}
			UE_LOG(LogTemp, Log, TEXT("Property: %s, Type: %s"), *PropertyName, *PropertyType);
			Added++;
		}
	}

	return Added;
}

int UCavrnusDataSmithPropertiesHandler::ProcessActorProperties(AActor* Actor, const FPropertiesContainer& Container)
{
	if (!Actor)
		return 0;
	int Added = 0;
	UE_LOG(LogTemp, Error, TEXT("--------------------------\nActor = %s"), *Actor->GetName());

	UClass* ActorClass = Actor->GetClass();
	bool FirstPropertyFound = true;

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
				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, Property->GetBoolMetaData(pair.Key) ? *FString("True") : *FString("False"));
				}

				UCavrnusFunctionLibrary::PostBoolPropertyUpdate(SpaceConnection, Container, PropertyName, BoolValue);
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

				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *Property->GetMetaData(pair.Key));
				}
				UCavrnusFunctionLibrary::PostStringPropertyUpdate(SpaceConnection, Container, PropertyName, StringValue);
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
				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *FString::SanitizeFloat(Property->GetFloatMetaData(pair.Key)));
				}

				UCavrnusFunctionLibrary::PostFloatPropertyUpdate(SpaceConnection, Container, PropertyName, FloatValue);
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

				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *FString::SanitizeFloat(Property->GetIntMetaData(pair.Key)));
				}
				UCavrnusFunctionLibrary::PostFloatPropertyUpdate(SpaceConnection, Container, PropertyName, IntValue);

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
				const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
				for (TPair<FName, FString> pair : *MetaData)
				{
					UE_LOG(LogTemp, Error, TEXT("MetaData : %s %s %s"), *pair.Key.ToString(), *pair.Value, *FString::SanitizeFloat(Property->GetDoubleMetaData(pair.Key)));
				}
				UCavrnusFunctionLibrary::PostFloatPropertyUpdate(SpaceConnection, Container, PropertyName, DoubleValue);

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

			}
			else if (PropertyType == "ByteProperty")
			{

				UE_LOG(LogTemp, Error, TEXT("Unable to support property %s of type %s"), *PropertyName, *PropertyType);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Unable to support property %s of type %s"), *PropertyName, *PropertyType);
			}
			UE_LOG(LogTemp, Log, TEXT("Property: %s, Type: %s"), *PropertyName, *PropertyType);
			Added++;
		}
	}
	ProcessComponents(Actor, Container);
	
	return Added;
}


void UCavrnusDataSmithPropertiesHandler::ProcessStaticMeshHierarchy()
{
	TArray<AStaticMeshActor*> AllMeshActors;
	GetAllStaticMeshActorsRecursive(DataSmithActor, AllMeshActors);

	TransformSyncs.Empty();  
	for (const auto MeshActor : AllMeshActors)
	{
		if (auto* SyncTransform = NewObject<UCavrnusDataSmithTransformSync>())
		{
			FString ActorName = MeshActor->GetName();
			const uint32 Hash = GetTypeHash(ActorName);
			FString UniqueName = FString::Printf(TEXT("%u_%s"), Hash, *ActorName);
			
			SyncTransform->Setup(SpaceConnection, ContainerName, UniqueName, MeshActor);
			TransformSyncs.Add(SyncTransform);
			TArray<UMaterialInterface*> MaterialInterfaces = MeshActor->GetStaticMeshComponent()->GetMaterials();
		}
	}
}

void UCavrnusDataSmithPropertiesHandler::FixMaterialsOnRuntimeDatasmithActor(ADatasmithRuntimeActor* DatasmithActor)
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

	TArray<AStaticMeshActor*> ActorsToProcess;

	GetAllStaticMeshActorsRecursive(DatasmithActor, ActorsToProcess);

	UE_LOG(LogTemp, Error, TEXT("Found %d Datasmith Children on Actor %s"), ActorsToProcess.Num(), *DatasmithActor->GetName());

	int TotalFixed = 0;

	ProcessTwinmotionDatasmithChildUsingSlotNames(DatasmithActor);

	for (AActor* Actor : ActorsToProcess)
	{
		TotalFixed += ProcessTwinmotionDatasmithChildUsingSlotNames(Actor);
	}

	UE_LOG(LogTemp, Log, TEXT("Finished fixing materials. Total slots fixed: %d"), TotalFixed);
}


void UCavrnusDataSmithPropertiesHandler::DoDebugFunction0()
{
	TArray<AActor*> FoundActors;

	ADatasmithRuntimeActor* DActor = Cast<ADatasmithRuntimeActor>(DataSmithActor);
	if (DActor)
	{
		FixMaterialsOnRuntimeDatasmithActor(DActor);

		TArray<AStaticMeshActor*> ActorsToProcess;

		GetAllStaticMeshActorsRecursive(DataSmithActor, ActorsToProcess);
		for (AActor* Actor : ActorsToProcess)
		{
			ProcessActorProperties(Actor, Actor->GetActorGuid().ToString());
		}
	}

}


void UCavrnusDataSmithPropertiesHandler::GetAllStaticMeshActorsRecursive(const AActor* InRoot, TArray<AStaticMeshActor*>& OutMeshActors)
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