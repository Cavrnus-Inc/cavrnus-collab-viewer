// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "DatasmithUtilities.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "XmlFile.h"

EDatasmithRuntimeFileType UDatasmithFileLibrary::GetFileTypeFromDatasmithFile(const FString& FilePath, FString& ErrorMessage)
{
	ErrorMessage = TEXT("");
    if (!FPaths::FileExists(FilePath))
    {
        ErrorMessage = FString::Printf(TEXT("File does not exist: %s"), *FilePath);
        return EDatasmithRuntimeFileType::Unknown;
    }

    FXmlFile XmlFile(FilePath, EConstructMethod::ConstructFromFile);
    if (!XmlFile.IsValid())
    {
        ErrorMessage = FString::Printf(TEXT("Failed to parse XML from : % s"), *FilePath);
        return EDatasmithRuntimeFileType::Unknown;
    }

    const FXmlNode* RootNode = XmlFile.GetRootNode();
    if (!RootNode)
    {
        ErrorMessage = FString::Printf(TEXT("Missing root node in: %s"), *FilePath);
        return EDatasmithRuntimeFileType::Unknown;
    }

    const FXmlNode* AppNode = RootNode->FindChildNode(TEXT("Application"));
    if (!AppNode)
    {
        ErrorMessage = FString::Printf(TEXT("Missing <Application> node in: %s"), *FilePath);
        return EDatasmithRuntimeFileType::Unknown;
    }

    FString Producer = AppNode->GetAttribute(TEXT("ProductName")).ToLower();

    // Use enum metadata to match display name
    UEnum* EnumPtr = StaticEnum<EDatasmithRuntimeFileType>();
    if (!EnumPtr)
    {
        return EDatasmithRuntimeFileType::Unknown;
    }

    for (int32 i = 0; i < EnumPtr->NumEnums(); ++i)
    {
        FText DisplayName = EnumPtr->GetDisplayNameTextByIndex(i);
        FString DisplayNameStr = DisplayName.ToString().ToLower();

        if (Producer.Contains(DisplayNameStr))
        {
            return static_cast<EDatasmithRuntimeFileType>(EnumPtr->GetValueByIndex(i));
        }
    }
    ErrorMessage = FString::Printf(TEXT("Parsing Error : % s"), *FilePath);
    return EDatasmithRuntimeFileType::Unknown;
}
