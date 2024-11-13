// Fill out your copyright notice in the Description page of Project Settings.

#include "CvtUtilities.h"
#include <Serialization/JsonReader.h>
#include <Serialization/JsonSerializer.h>

TArray<FTransform> UCvtUtilities::JsonStringToFTransformArray(const FString& JsonStr, const FString& FieldName,
                                                              bool& Success)
{
	TArray<FTransform> Transforms;
	TSharedPtr<FJsonObject> JsonObject;
	TArray<TSharedPtr<FJsonValue>> JsonTransforms;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		JsonTransforms = JsonObject->GetArrayField(FieldName);
		for (const TSharedPtr<FJsonValue>& TransformValue : JsonTransforms)
		{
			bool bSuccess = false;
			FTransform Transform = JsonObjectToFTransform(TransformValue->AsObject(), bSuccess);
			if (bSuccess)
			{
				Transforms.Add(Transform);
			}
		}
	}

	Success = JsonTransforms.Num() > 0 && Transforms.Num() == JsonTransforms.Num();
	return Transforms;
}

TArray<FString> UCvtUtilities::JsonStringToFStringArray(const FString& JsonStr, const FString& FieldName, bool& Success)
{
	TArray<FString> ReturnValue;
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		const TArray<TSharedPtr<FJsonValue>>* JsonArray;
		if (JsonObject->TryGetArrayField(FieldName, JsonArray))
		{
			for (const TSharedPtr<FJsonValue>& StringValue : *JsonArray)
			{
				FString FoundString = StringValue.Get()->AsString();
				ReturnValue.Add(FoundString);
			}
		}
	}

	Success = ReturnValue.Num() > 0;

	return ReturnValue;
}

FTransform UCvtUtilities::JsonObjectToFTransform(TSharedPtr<FJsonObject> JsonObject, bool& Success)
{
	FJsonObject* Data = JsonObject.Get();
	if (Data->Values.Num() == 3)
	{
		bool bTranslationSuccess = false;
		TSharedPtr<FJsonObject> TranslationObject = Data->Values["translation"]->AsObject();
		const FVector3d Translation = JsonObjectToFVector(TranslationObject, bTranslationSuccess);

		bool bRotationSuccess = false;
		TSharedPtr<FJsonObject> RotationObject = Data->Values["rotation"]->AsObject();
		const FQuat Rotation = JsonObjectToFQuat(RotationObject, bRotationSuccess);

		bool bScaleSuccess = false;
		TSharedPtr<FJsonObject> ScaleObject = Data->Values["scale3D"]->AsObject();
		const FVector3d Scale = JsonObjectToFVector(ScaleObject, bScaleSuccess);

		Success = bTranslationSuccess && bRotationSuccess && bScaleSuccess;
		return FTransform(Rotation, Translation, Scale);
	}

	return FTransform();
}

FVector UCvtUtilities::JsonObjectToFVector(TSharedPtr<FJsonObject> JsonObject, bool& Success)
{
	FJsonObject* Data = JsonObject.Get();
	if (Data->Values.Num() == 3)
	{
		double LocationX = Data->Values["x"]->AsNumber();
		double LocationY = Data->Values["y"]->AsNumber();
		double LocationZ = Data->Values["z"]->AsNumber();
		FVector3d Vector(LocationX, LocationY, LocationZ);

		Success = true;
		return Vector;
	}

	return FVector();
}

FQuat UCvtUtilities::JsonObjectToFQuat(TSharedPtr<FJsonObject> JsonObject, bool& Success)
{
	FJsonObject* Data = JsonObject.Get();
	if (Data->Values.Num() == 4)
	{
		double X = Data->Values["x"]->AsNumber();
		double Y = Data->Values["y"]->AsNumber();
		double Z = Data->Values["z"]->AsNumber();
		double W = Data->Values["w"]->AsNumber();
		FQuat Quaternion(X, Y, Z, W);

		Success = true;
		return Quaternion;
	}

	return FQuat();
}


bool UCvtUtilities::DeleteLocalFile(FString FilePath)
{
	return IFileManager::Get().Delete(*FilePath);
}

bool UCvtUtilities::DeleteLocalFolder(FString FolderPath)
{
	IFileManager& FileManager = IFileManager::Get();

	const FString AbsoluteFolderPath = FPaths::ConvertRelativePathToFull(FolderPath);

	// Delete the directory and its contents
	return FileManager.DeleteDirectory(*AbsoluteFolderPath, false, true);
}

FString UCvtUtilities::GetSavedFolder(const FString SubDirectory)
{
	const FString CacheDirectory = FPaths::ProjectSavedDir() / SubDirectory;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*CacheDirectory))
	{
		PlatformFile.CreateDirectory(*CacheDirectory);
	}

	return CacheDirectory;
}

TArray<FString> UCvtUtilities::GetFilesOfType(const FString& FolderPath, const FString& Extension)
{
	TArray<FString> FoundFiles;

	const FString AbsoluteFolderPath = FPaths::ConvertRelativePathToFull(FolderPath);
	const FString SearchPattern = FString::Printf(TEXT("%s/*%s"), *AbsoluteFolderPath, *Extension);

	IFileManager& FileManager = IFileManager::Get();
	FileManager.FindFiles(FoundFiles, *SearchPattern, true, false);

	for (FString& FileName : FoundFiles)
	{
		FileName = FPaths::Combine(AbsoluteFolderPath, FileName);
	}

	return FoundFiles;
}