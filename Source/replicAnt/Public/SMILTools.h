#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Engine.h"
#include "Math/RandomStream.h"
#include "SMILTools.generated.h"

/**
 * Structure to hold PCA bone transformation data for a single bone
 */
USTRUCT(BlueprintType)
struct REPLICANT_API FPCABoneTransform
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "PCA Transform")
    FVector Scale = FVector::OneVector;

    UPROPERTY(BlueprintReadWrite, Category = "PCA Transform")
    FVector Translation = FVector::ZeroVector;

    FPCABoneTransform()
    {
        Scale = FVector::OneVector;
        Translation = FVector::ZeroVector;
    }
};

/**
 * Structure to hold all bone transforms for a single Principal Component
 */
USTRUCT(BlueprintType)
struct REPLICANT_API FPCAComponentData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "PCA Component")
    TArray<FPCABoneTransform> BoneTransforms;

    FPCAComponentData()
    {
        BoneTransforms.Empty();
    }
};

/**
 * Main structure to hold complete PCA morphing data loaded from CSV
 */
USTRUCT(BlueprintType)
struct REPLICANT_API FPCAMorphData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "PCA Data")
    TArray<FString> BoneNames;

    UPROPERTY(BlueprintReadWrite, Category = "PCA Data")
    TArray<FPCAComponentData> PrincipalComponents;

    UPROPERTY(BlueprintReadWrite, Category = "PCA Data")
    int32 NumBones = 0;

    UPROPERTY(BlueprintReadWrite, Category = "PCA Data")
    int32 NumComponents = 0;

    UPROPERTY(BlueprintReadWrite, Category = "PCA Data")
    bool bIsDataValid = false;

    FPCAMorphData()
    {
        BoneNames.Empty();
        PrincipalComponents.Empty();
        NumBones = 0;
        NumComponents = 0;
        bIsDataValid = false;
    }

    // Helper function to validate data integrity
    bool IsValid() const
    {
        return bIsDataValid && 
               NumBones > 0 && 
               NumComponents > 0 && 
               BoneNames.Num() == NumBones &&
               PrincipalComponents.Num() == NumComponents;
    }
};

/**
 * SMILTools Blueprint Function Library
 * Provides functionality for loading and manipulating PCA morphing data
 */
UCLASS()
class REPLICANT_API USMILTools : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Load PCA morphing data from a CSV file
     * @param FilePath - Absolute path to the CSV file containing PCA data
     * @param bShowDebugMessages - Whether to display debug messages on screen
     * @param OutPCAData - Output structure containing the loaded PCA data
     * @param OutNumComponents - Number of principal components loaded
     * @param OutNumBones - Number of bones loaded
     * @return True if loading was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "SMIL Tools", CallInEditor)
    static bool LoadPCADataFromCSV(
        const FString& FilePath,
        bool bShowDebugMessages,
        FPCAMorphData& OutPCAData,
        int32& OutNumComponents,
        int32& OutNumBones
    );

    /**
     * Generate PCA component weights using normal distribution sampling
     * @param NumComponents - Number of principal components to generate weights for
     * @param StandardDeviation - Standard deviation of the normal distribution (mean is always 0)
     * @param bShowDebugMessages - Whether to display debug messages on screen
     * @param RandomStream - Random stream for reproducible results
     * @param OutWeights - Array of generated weights for each principal component
     * @return True if sampling was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "SMIL Tools", CallInEditor)
    static bool GeneratePCAWeights(
        int32 NumComponents,
        float StandardDeviation,
        bool bShowDebugMessages,
        UPARAM(ref) FRandomStream& RandomStream,
        TArray<float>& OutWeights
    );

    /**
     * Generate PCA component weights using normal distribution sampling (simple version without random stream)
     * @param NumComponents - Number of principal components to generate weights for
     * @param StandardDeviation - Standard deviation of the normal distribution (mean is always 0)
     * @param bShowDebugMessages - Whether to display debug messages on screen
     * @param OutWeights - Array of generated weights for each principal component
     * @return True if sampling was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "SMIL Tools", CallInEditor)
    static bool GeneratePCAWeightsSimple(
        int32 NumComponents,
        float StandardDeviation,
        bool bShowDebugMessages,
        TArray<float>& OutWeights
    );

    /**
     * Sample bone transforms from PCA data using provided weights
     * @param PCAData - The loaded PCA morphing data
     * @param TranslationWeights - Array of weights for translation components (must match NumComponents)
     * @param ScaleWeights - Array of weights for scale components (must match NumComponents)
     * @param bShowDebugMessages - Whether to display debug messages on screen
     * @param OutBoneNames - Array of bone names corresponding to transforms
     * @param OutTransforms - Array of computed transforms (scale + translation, no rotation)
     * @return True if sampling was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "SMIL Tools", CallInEditor)
    static bool SamplePCATransforms(
        const FPCAMorphData& PCAData,
        const TArray<float>& TranslationWeights,
        const TArray<float>& ScaleWeights,
        bool bShowDebugMessages,
        TArray<FString>& OutBoneNames,
        TArray<FTransform>& OutTransforms
    );

private:
    /**
     * Parse a single line of CSV data
     * @param Line - The CSV line to parse
     * @param OutTokens - Array to store the parsed tokens
     * @return Number of tokens parsed
     */
    static int32 ParseCSVLine(const FString& Line, TArray<FString>& OutTokens);

    /**
     * Validate the CSV header structure
     * @param HeaderTokens - Array of header tokens
     * @param OutNumComponents - Number of components detected from header
     * @return True if header is valid
     */
    static bool ValidateCSVHeader(const TArray<FString>& HeaderTokens, int32& OutNumComponents);

    /**
     * Parse bone data from a CSV line
     * @param Tokens - Tokenized CSV line
     * @param BoneName - Name of the bone (first token)
     * @param NumComponents - Expected number of components
     * @param OutComponentData - Array to store the parsed component data for this bone
     * @return True if parsing was successful
     */
    static bool ParseBoneData(
        const TArray<FString>& Tokens,
        const FString& BoneName,
        int32 NumComponents,
        TArray<FPCABoneTransform>& OutComponentData
    );

    /**
     * Log detailed information about the loaded PCA data
     * @param PCAData - The loaded PCA data to log
     * @param FilePath - Path of the source file
     */
    static void LogPCADataInfo(const FPCAMorphData& PCAData, const FString& FilePath);

    /**
     * Generate a normally distributed random number using Box-Muller transform
     * @param RandomStream - Random stream to use for generation
     * @param Mean - Mean of the normal distribution
     * @param StandardDeviation - Standard deviation of the normal distribution
     * @return Normally distributed random number
     */
    static float GenerateNormalRandom(FRandomStream& RandomStream, float Mean = 0.0f, float StandardDeviation = 1.0f);

    /**
     * Generate a normally distributed random number using Box-Muller transform (global random)
     * @param Mean - Mean of the normal distribution
     * @param StandardDeviation - Standard deviation of the normal distribution
     * @return Normally distributed random number
     */
    static float GenerateNormalRandomGlobal(float Mean = 0.0f, float StandardDeviation = 1.0f);

    /**
     * Log information about generated PCA weights
     * @param Weights - Array of generated weights
     * @param StandardDeviation - Standard deviation used for generation
     */
    static void LogPCAWeights(const TArray<float>& Weights, float StandardDeviation);

    /**
     * Log information about sampled PCA transforms
     * @param BoneNames - Array of bone names
     * @param Transforms - Array of transforms
     * @param TranslationWeights - Translation weights used for sampling
     * @param ScaleWeights - Scale weights used for sampling
     */
    static void LogPCATransforms(const TArray<FString>& BoneNames, const TArray<FTransform>& Transforms, const TArray<float>& TranslationWeights, const TArray<float>& ScaleWeights);
}; 