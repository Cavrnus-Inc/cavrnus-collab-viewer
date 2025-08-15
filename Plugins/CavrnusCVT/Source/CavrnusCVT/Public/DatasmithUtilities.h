#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DatasmithUtilities.generated.h"

class DatasmithUtilities;

UENUM(BlueprintType)
enum class EDatasmithRuntimeFileType : uint8
{
    Twinmotion     UMETA(DisplayName = "Twinmotion"),
    Solidworks     UMETA(DisplayName = "SolidWorks"),
    SketchUp       UMETA(DisplayName = "Sketchup Pro"),
    Revit          UMETA(DisplayName = "Revit"),
    Navisworks     UMETA(DisplayName = "Navisworks"),
    _3DSMax        UMETA(DisplayName = "3dsmax"),
    Rhino          UMETA(DisplayName = "Rhino"),
    Archicad       UMETA(DisplayName = "Archicad"),
    FormZ          UMETA(DisplayName = "Form•Z"),
    OpenFlight     UMETA(DisplayName = "OpenFlight"),
    CETDesigner    UMETA(DisplayName = "CET Designer"),
    Allplan        UMETA(DisplayName = "Allplan"),
    CDB            UMETA(DisplayName = "CDB"),
    Unknown        UMETA(DisplayName = "Unknown"),
};


UCLASS(Blueprintable)
class UDatasmithFileLibrary : public UBlueprintFunctionLibrary 
{
    GENERATED_BODY()

public:

    /**
     * Determines the type of a Datasmith file based on its content.
     * @param FilePath The path to the Datasmith file.
     * @return The EDatasmithRuntimeFileType corresponding to the file.
     */
    UFUNCTION(BlueprintCallable, Category = "Datasmith")
    static EDatasmithRuntimeFileType GetFileTypeFromDatasmithFile(const FString& FilePath, FString& ErrorMessage);

    /**
     * Converts a Datasmith file type enum to a readable string.
     * @param FileType The enum value.
     * @return A human-readable string representing the file type.
     */
    UFUNCTION(BlueprintCallable, Category = "Datasmith")
    static FString GetRuntimeFileTypeString(EDatasmithRuntimeFileType FileType)
    {
        UEnum* EnumPtr = StaticEnum<EDatasmithRuntimeFileType>();
        if (!EnumPtr)
        {
            return TEXT("Unknown");
        }
        FText DisplayName = EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(FileType));
        return DisplayName.ToString();
    }

    /**
     * Convenience function that returns both the file type and its string representation.
     */
    UFUNCTION(BlueprintCallable, Category = "Datasmith")
    static bool AnalyzeDatasmithFile(const FString& FilePath, EDatasmithRuntimeFileType& OutFileType, FString& OutFileTypeString, FString& ErrorMessage)
    {
        OutFileType = GetFileTypeFromDatasmithFile(FilePath, ErrorMessage);
        OutFileTypeString = GetRuntimeFileTypeString(OutFileType);
        if (OutFileType == EDatasmithRuntimeFileType::Unknown)
            return false;
        return true;
    }
};




