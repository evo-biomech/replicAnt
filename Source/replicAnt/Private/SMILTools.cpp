#include "SMILTools.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Engine/Engine.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogSMILTools, Log, All);

bool USMILTools::LoadPCADataFromCSV(const FString& FilePath, bool bShowDebugMessages, FPCAMorphData& OutPCAData, int32& OutNumComponents, int32& OutNumBones)
{
    // Initialize output parameters
    OutPCAData = FPCAMorphData();
    OutNumComponents = 0;
    OutNumBones = 0;

    // Validate file path
    if (FilePath.IsEmpty())
    {
        UE_LOG(LogSMILTools, Error, TEXT("LoadPCADataFromCSV: File path is empty"));
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("SMILTools Error: File path is empty"));
        }
        return false;
    }

    // Check if file exists
    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
    {
        FString ErrorMsg = FString::Printf(TEXT("LoadPCADataFromCSV: File does not exist: %s"), *FilePath);
        UE_LOG(LogSMILTools, Error, TEXT("%s"), *ErrorMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *ErrorMsg);
        }
        return false;
    }

    // Load file content
    TArray<FString> FileLines;
    if (!FFileHelper::LoadFileToStringArray(FileLines, *FilePath))
    {
        FString ErrorMsg = FString::Printf(TEXT("LoadPCADataFromCSV: Failed to load file: %s"), *FilePath);
        UE_LOG(LogSMILTools, Error, TEXT("%s"), *ErrorMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *ErrorMsg);
        }
        return false;
    }

    if (FileLines.Num() < 2)
    {
        FString ErrorMsg = TEXT("LoadPCADataFromCSV: File must contain at least header and one data row");
        UE_LOG(LogSMILTools, Error, TEXT("%s"), *ErrorMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *ErrorMsg);
        }
        return false;
    }

    UE_LOG(LogSMILTools, Log, TEXT("LoadPCADataFromCSV: Starting to parse file: %s"), *FilePath);
    UE_LOG(LogSMILTools, Log, TEXT("LoadPCADataFromCSV: File contains %d lines"), FileLines.Num());

    // Parse header
    TArray<FString> HeaderTokens;
    ParseCSVLine(FileLines[0], HeaderTokens);

    int32 DetectedNumComponents = 0;
    if (!ValidateCSVHeader(HeaderTokens, DetectedNumComponents))
    {
        FString ErrorMsg = TEXT("LoadPCADataFromCSV: Invalid CSV header format");
        UE_LOG(LogSMILTools, Error, TEXT("%s"), *ErrorMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *ErrorMsg);
        }
        return false;
    }

    OutNumComponents = DetectedNumComponents;
    UE_LOG(LogSMILTools, Log, TEXT("LoadPCADataFromCSV: Detected %d principal components"), OutNumComponents);

    // Initialize PCA data structure
    OutPCAData.NumComponents = OutNumComponents;
    OutPCAData.PrincipalComponents.SetNum(OutNumComponents);

    // Parse data lines
    int32 ValidBonesCount = 0;
    for (int32 LineIndex = 1; LineIndex < FileLines.Num(); LineIndex++)
    {
        FString CurrentLine = FileLines[LineIndex].TrimStartAndEnd();
        
        // Skip empty lines
        if (CurrentLine.IsEmpty())
        {
            continue;
        }

        TArray<FString> LineTokens;
        ParseCSVLine(CurrentLine, LineTokens);

        if (LineTokens.Num() < 1)
        {
            UE_LOG(LogSMILTools, Warning, TEXT("LoadPCADataFromCSV: Skipping empty line %d"), LineIndex + 1);
            continue;
        }

        FString BoneName = LineTokens[0].TrimStartAndEnd();
        if (BoneName.IsEmpty())
        {
            UE_LOG(LogSMILTools, Warning, TEXT("LoadPCADataFromCSV: Skipping line %d with empty bone name"), LineIndex + 1);
            continue;
        }

        // Parse bone data for all components
        TArray<FPCABoneTransform> BoneComponentData;
        if (ParseBoneData(LineTokens, BoneName, OutNumComponents, BoneComponentData))
        {
            // Add bone name
            OutPCAData.BoneNames.Add(BoneName);

            // Add bone transform data to each component
            for (int32 ComponentIndex = 0; ComponentIndex < OutNumComponents; ComponentIndex++)
            {
                if (BoneComponentData.IsValidIndex(ComponentIndex))
                {
                    OutPCAData.PrincipalComponents[ComponentIndex].BoneTransforms.Add(BoneComponentData[ComponentIndex]);
                }
                else
                {
                    // Add default transform if data is missing
                    OutPCAData.PrincipalComponents[ComponentIndex].BoneTransforms.Add(FPCABoneTransform());
                    UE_LOG(LogSMILTools, Warning, TEXT("LoadPCADataFromCSV: Missing data for bone '%s' component %d, using default"), *BoneName, ComponentIndex + 1);
                }
            }

            ValidBonesCount++;
            UE_LOG(LogSMILTools, VeryVerbose, TEXT("LoadPCADataFromCSV: Successfully parsed bone '%s'"), *BoneName);
        }
        else
        {
            UE_LOG(LogSMILTools, Warning, TEXT("LoadPCADataFromCSV: Failed to parse bone data for '%s' on line %d"), *BoneName, LineIndex + 1);
        }
    }

    // Finalize data
    OutPCAData.NumBones = ValidBonesCount;
    OutPCAData.bIsDataValid = (ValidBonesCount > 0 && OutNumComponents > 0);
    OutNumBones = ValidBonesCount;

    // Log results
    LogPCADataInfo(OutPCAData, FilePath);

    // Display success message
    if (OutPCAData.bIsDataValid)
    {
        FString SuccessMsg = FString::Printf(TEXT("SMILTools: Successfully loaded %d bones, %d components"), OutNumBones, OutNumComponents);
        UE_LOG(LogSMILTools, Log, TEXT("LoadPCADataFromCSV: %s"), *SuccessMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, *SuccessMsg);
        }
        return true;
    }
    else
    {
        FString ErrorMsg = TEXT("SMILTools: Failed to load valid PCA data");
        UE_LOG(LogSMILTools, Error, TEXT("LoadPCADataFromCSV: %s"), *ErrorMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *ErrorMsg);
        }
        return false;
    }
}

int32 USMILTools::ParseCSVLine(const FString& Line, TArray<FString>& OutTokens)
{
    OutTokens.Empty();
    
    if (Line.IsEmpty())
    {
        return 0;
    }

    // Simple CSV parsing - split by comma and trim whitespace
    Line.ParseIntoArray(OutTokens, TEXT(","), true);
    
    // Trim whitespace from each token
    for (FString& Token : OutTokens)
    {
        Token = Token.TrimStartAndEnd();
    }

    return OutTokens.Num();
}

bool USMILTools::ValidateCSVHeader(const TArray<FString>& HeaderTokens, int32& OutNumComponents)
{
    if (HeaderTokens.Num() < 7) // At least: joint_name + 6 columns for 1 PC
    {
        UE_LOG(LogSMILTools, Error, TEXT("ValidateCSVHeader: Header has too few columns (%d), expected at least 7"), HeaderTokens.Num());
        return false;
    }

    // First column should be joint_name
    if (!HeaderTokens[0].Equals(TEXT("joint_name"), ESearchCase::IgnoreCase))
    {
        UE_LOG(LogSMILTools, Error, TEXT("ValidateCSVHeader: First column should be 'joint_name', found '%s'"), *HeaderTokens[0]);
        return false;
    }

    // Calculate number of components from remaining columns
    // Each PC should have 6 columns: scale_x, scale_y, scale_z, translation_x, translation_y, translation_z
    int32 DataColumns = HeaderTokens.Num() - 1; // Exclude joint_name column
    if (DataColumns % 6 != 0)
    {
        UE_LOG(LogSMILTools, Error, TEXT("ValidateCSVHeader: Data columns (%d) not divisible by 6"), DataColumns);
        return false;
    }

    OutNumComponents = DataColumns / 6;
    
    // Validate PC column naming pattern
    for (int32 PC = 1; PC <= OutNumComponents; PC++)
    {
        int32 BaseIndex = 1 + (PC - 1) * 6; // Start index for this PC
        
        TArray<FString> ExpectedSuffixes = {
            TEXT("_scale_x"), TEXT("_scale_y"), TEXT("_scale_z"),
            TEXT("_translation_x"), TEXT("_translation_y"), TEXT("_translation_z")
        };

        for (int32 i = 0; i < 6; i++)
        {
            if (!HeaderTokens.IsValidIndex(BaseIndex + i))
            {
                UE_LOG(LogSMILTools, Error, TEXT("ValidateCSVHeader: Missing column for PC_%d%s"), PC, *ExpectedSuffixes[i]);
                return false;
            }

            FString ExpectedPattern = FString::Printf(TEXT("PC_%d%s"), PC, *ExpectedSuffixes[i]);
            if (!HeaderTokens[BaseIndex + i].Equals(ExpectedPattern, ESearchCase::IgnoreCase))
            {
                UE_LOG(LogSMILTools, Warning, TEXT("ValidateCSVHeader: Column name mismatch. Expected '%s', found '%s'"), 
                    *ExpectedPattern, *HeaderTokens[BaseIndex + i]);
            }
        }
    }

    UE_LOG(LogSMILTools, Log, TEXT("ValidateCSVHeader: Valid header with %d principal components"), OutNumComponents);
    return true;
}

bool USMILTools::ParseBoneData(const TArray<FString>& Tokens, const FString& BoneName, int32 NumComponents, TArray<FPCABoneTransform>& OutComponentData)
{
    OutComponentData.Empty();
    OutComponentData.SetNum(NumComponents);

    int32 ExpectedTokens = 1 + (NumComponents * 6); // bone_name + 6 values per component
    if (Tokens.Num() < ExpectedTokens)
    {
        UE_LOG(LogSMILTools, Error, TEXT("ParseBoneData: Bone '%s' has %d tokens, expected %d"), *BoneName, Tokens.Num(), ExpectedTokens);
        return false;
    }

    for (int32 ComponentIndex = 0; ComponentIndex < NumComponents; ComponentIndex++)
    {
        int32 BaseIndex = 1 + (ComponentIndex * 6); // Skip bone name, then 6 values per component

        FPCABoneTransform& Transform = OutComponentData[ComponentIndex];

        // Parse scale values (X, Y, Z)
        for (int32 i = 0; i < 3; i++)
        {
            float ScaleValue = FCString::Atof(*Tokens[BaseIndex + i]);
            Transform.Scale[i] = ScaleValue;
        }

        // Parse translation values (X, Y, Z)
        for (int32 i = 0; i < 3; i++)
        {
            float TranslationValue = FCString::Atof(*Tokens[BaseIndex + 3 + i]);
            Transform.Translation[i] = TranslationValue;
        }

        UE_LOG(LogSMILTools, VeryVerbose, TEXT("ParseBoneData: Bone '%s' PC_%d - Scale:(%.6f,%.6f,%.6f) Translation:(%.6f,%.6f,%.6f)"), 
            *BoneName, ComponentIndex + 1,
            Transform.Scale.X, Transform.Scale.Y, Transform.Scale.Z,
            Transform.Translation.X, Transform.Translation.Y, Transform.Translation.Z);
    }

    return true;
}

void USMILTools::LogPCADataInfo(const FPCAMorphData& PCAData, const FString& FilePath)
{
    UE_LOG(LogSMILTools, Log, TEXT("=== PCA Data Loading Summary ==="));
    UE_LOG(LogSMILTools, Log, TEXT("Source File: %s"), *FilePath);
    UE_LOG(LogSMILTools, Log, TEXT("Number of Bones: %d"), PCAData.NumBones);
    UE_LOG(LogSMILTools, Log, TEXT("Number of Components: %d"), PCAData.NumComponents);
    UE_LOG(LogSMILTools, Log, TEXT("Data Valid: %s"), PCAData.bIsDataValid ? TEXT("Yes") : TEXT("No"));
    
    if (PCAData.bIsDataValid)
    {
        UE_LOG(LogSMILTools, Log, TEXT("Loaded Bones:"));
        for (int32 i = 0; i < PCAData.BoneNames.Num(); i++)
        {
            UE_LOG(LogSMILTools, Log, TEXT("  [%d] %s"), i, *PCAData.BoneNames[i]);
        }

        // Log some sample data for verification
        if (PCAData.PrincipalComponents.Num() > 0 && PCAData.PrincipalComponents[0].BoneTransforms.Num() > 0)
        {
            const FPCABoneTransform& FirstTransform = PCAData.PrincipalComponents[0].BoneTransforms[0];
            UE_LOG(LogSMILTools, Log, TEXT("Sample Data (PC_1, Bone 0): Scale(%.6f,%.6f,%.6f) Translation(%.6f,%.6f,%.6f)"),
                FirstTransform.Scale.X, FirstTransform.Scale.Y, FirstTransform.Scale.Z,
                FirstTransform.Translation.X, FirstTransform.Translation.Y, FirstTransform.Translation.Z);
        }
    }
    
    UE_LOG(LogSMILTools, Log, TEXT("=== End PCA Data Summary ==="));
}

bool USMILTools::GeneratePCAWeights(int32 NumComponents, float StandardDeviation, bool bShowDebugMessages, FRandomStream& RandomStream, TArray<float>& OutWeights)
{
    // Initialize output
    OutWeights.Empty();

    // Validate input parameters
    if (NumComponents <= 0)
    {
        FString ErrorMsg = FString::Printf(TEXT("GeneratePCAWeights: Invalid number of components (%d), must be greater than 0"), NumComponents);
        UE_LOG(LogSMILTools, Error, TEXT("%s"), *ErrorMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *ErrorMsg);
        }
        return false;
    }

    if (StandardDeviation < 0.0f)
    {
        FString ErrorMsg = FString::Printf(TEXT("GeneratePCAWeights: Invalid standard deviation (%.3f), must be non-negative"), StandardDeviation);
        UE_LOG(LogSMILTools, Error, TEXT("%s"), *ErrorMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *ErrorMsg);
        }
        return false;
    }

    UE_LOG(LogSMILTools, Log, TEXT("GeneratePCAWeights: Generating %d weights with standard deviation %.3f"), NumComponents, StandardDeviation);

    // Generate weights using normal distribution
    OutWeights.SetNum(NumComponents);
    for (int32 i = 0; i < NumComponents; i++)
    {
        OutWeights[i] = GenerateNormalRandom(RandomStream, 0.0f, StandardDeviation);
        UE_LOG(LogSMILTools, VeryVerbose, TEXT("GeneratePCAWeights: PC_%d weight = %.6f"), i + 1, OutWeights[i]);
    }

    // Log summary information
    LogPCAWeights(OutWeights, StandardDeviation);

    // Display success message
    FString SuccessMsg = FString::Printf(TEXT("SMILTools: Generated %d PCA weights (σ=%.3f)"), NumComponents, StandardDeviation);
    UE_LOG(LogSMILTools, Log, TEXT("GeneratePCAWeights: %s"), *SuccessMsg);
    if (bShowDebugMessages && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, *SuccessMsg);
    }

    return true;
}

bool USMILTools::GeneratePCAWeightsSimple(int32 NumComponents, float StandardDeviation, bool bShowDebugMessages, TArray<float>& OutWeights)
{
    // Initialize output
    OutWeights.Empty();

    // Validate input parameters
    if (NumComponents <= 0)
    {
        FString ErrorMsg = FString::Printf(TEXT("GeneratePCAWeightsSimple: Invalid number of components (%d), must be greater than 0"), NumComponents);
        UE_LOG(LogSMILTools, Error, TEXT("%s"), *ErrorMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *ErrorMsg);
        }
        return false;
    }

    if (StandardDeviation < 0.0f)
    {
        FString ErrorMsg = FString::Printf(TEXT("GeneratePCAWeightsSimple: Invalid standard deviation (%.3f), must be non-negative"), StandardDeviation);
        UE_LOG(LogSMILTools, Error, TEXT("%s"), *ErrorMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *ErrorMsg);
        }
        return false;
    }

    UE_LOG(LogSMILTools, Log, TEXT("GeneratePCAWeightsSimple: Generating %d weights with standard deviation %.3f"), NumComponents, StandardDeviation);

    // Generate weights using normal distribution with global random
    OutWeights.SetNum(NumComponents);
    for (int32 i = 0; i < NumComponents; i++)
    {
        OutWeights[i] = GenerateNormalRandomGlobal(0.0f, StandardDeviation);
        UE_LOG(LogSMILTools, VeryVerbose, TEXT("GeneratePCAWeightsSimple: PC_%d weight = %.6f"), i + 1, OutWeights[i]);
    }

    // Log summary information
    LogPCAWeights(OutWeights, StandardDeviation);

    // Display success message
    FString SuccessMsg = FString::Printf(TEXT("SMILTools: Generated %d PCA weights (σ=%.3f)"), NumComponents, StandardDeviation);
    UE_LOG(LogSMILTools, Log, TEXT("GeneratePCAWeightsSimple: %s"), *SuccessMsg);
    if (bShowDebugMessages && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, *SuccessMsg);
    }

    return true;
}

float USMILTools::GenerateNormalRandom(FRandomStream& RandomStream, float Mean, float StandardDeviation)
{
    // Box-Muller transform for generating normally distributed random numbers
    // For simplicity, we'll generate fresh values each time to avoid static complications
    float U1, U2;
    do
    {
        U1 = RandomStream.FRand(); // Uniform [0,1]
        U2 = RandomStream.FRand(); // Uniform [0,1]
    } while (U1 <= SMALL_NUMBER); // Ensure U1 is not zero

    float Z0 = FMath::Sqrt(-2.0f * FMath::Loge(U1)) * FMath::Cos(2.0f * PI * U2);

    return Z0 * StandardDeviation + Mean;
}

float USMILTools::GenerateNormalRandomGlobal(float Mean, float StandardDeviation)
{
    // Box-Muller transform using global random
    static bool bHasCachedValue = false;
    static float CachedValue = 0.0f;

    if (bHasCachedValue)
    {
        bHasCachedValue = false;
        return CachedValue * StandardDeviation + Mean;
    }

    float U1, U2;
    do
    {
        U1 = FMath::FRand(); // Uniform [0,1]
        U2 = FMath::FRand(); // Uniform [0,1]
    } while (U1 <= SMALL_NUMBER); // Ensure U1 is not zero

    float Z0 = FMath::Sqrt(-2.0f * FMath::Loge(U1)) * FMath::Cos(2.0f * PI * U2);
    float Z1 = FMath::Sqrt(-2.0f * FMath::Loge(U1)) * FMath::Sin(2.0f * PI * U2);

    // Cache the second value for next call
    CachedValue = Z1;
    bHasCachedValue = true;

    return Z0 * StandardDeviation + Mean;
}

void USMILTools::LogPCAWeights(const TArray<float>& Weights, float StandardDeviation)
{
    UE_LOG(LogSMILTools, Log, TEXT("=== PCA Weights Generation Summary ==="));
    UE_LOG(LogSMILTools, Log, TEXT("Number of Weights: %d"), Weights.Num());
    UE_LOG(LogSMILTools, Log, TEXT("Standard Deviation: %.6f"), StandardDeviation);

    if (Weights.Num() > 0)
    {
        // Calculate statistics
        float Sum = 0.0f;
        float SumSquares = 0.0f;
        float MinValue = Weights[0];
        float MaxValue = Weights[0];

        for (float Weight : Weights)
        {
            Sum += Weight;
            SumSquares += Weight * Weight;
            MinValue = FMath::Min(MinValue, Weight);
            MaxValue = FMath::Max(MaxValue, Weight);
        }

        float Mean = Sum / Weights.Num();
        float Variance = (SumSquares / Weights.Num()) - (Mean * Mean);
        float ActualStdDev = FMath::Sqrt(FMath::Max(0.0f, Variance));

        UE_LOG(LogSMILTools, Log, TEXT("Generated Weights Statistics:"));
        UE_LOG(LogSMILTools, Log, TEXT("  Mean: %.6f (expected: 0.0)"), Mean);
        UE_LOG(LogSMILTools, Log, TEXT("  Std Dev: %.6f (requested: %.6f)"), ActualStdDev, StandardDeviation);
        UE_LOG(LogSMILTools, Log, TEXT("  Min: %.6f"), MinValue);
        UE_LOG(LogSMILTools, Log, TEXT("  Max: %.6f"), MaxValue);

        // Log first few weights for verification
        int32 NumToShow = FMath::Min(5, Weights.Num());
        UE_LOG(LogSMILTools, Log, TEXT("Sample Weights (first %d):"), NumToShow);
        for (int32 i = 0; i < NumToShow; i++)
        {
            UE_LOG(LogSMILTools, Log, TEXT("  PC_%d: %.6f"), i + 1, Weights[i]);
        }

        if (Weights.Num() > 5)
        {
            UE_LOG(LogSMILTools, Log, TEXT("  ... and %d more"), Weights.Num() - 5);
        }
    }

    UE_LOG(LogSMILTools, Log, TEXT("=== End PCA Weights Summary ==="));
}

bool USMILTools::SamplePCATransforms(const FPCAMorphData& PCAData, const TArray<float>& TranslationWeights, const TArray<float>& ScaleWeights, bool bShowDebugMessages, TArray<FString>& OutBoneNames, TArray<FTransform>& OutTransforms)
{
    // Initialize outputs
    OutBoneNames.Empty();
    OutTransforms.Empty();

    // Validate input data
    if (!PCAData.IsValid())
    {
        FString ErrorMsg = TEXT("SamplePCATransforms: Invalid PCA data provided");
        UE_LOG(LogSMILTools, Error, TEXT("%s"), *ErrorMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *ErrorMsg);
        }
        return false;
    }

    // Validate translation weights count matches component count
    if (TranslationWeights.Num() != PCAData.NumComponents)
    {
        FString ErrorMsg = FString::Printf(TEXT("SamplePCATransforms: Translation weight count (%d) doesn't match component count (%d)"), 
            TranslationWeights.Num(), PCAData.NumComponents);
        UE_LOG(LogSMILTools, Error, TEXT("%s"), *ErrorMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *ErrorMsg);
        }
        return false;
    }

    // Validate scale weights count matches component count
    if (ScaleWeights.Num() != PCAData.NumComponents)
    {
        FString ErrorMsg = FString::Printf(TEXT("SamplePCATransforms: Scale weight count (%d) doesn't match component count (%d)"), 
            ScaleWeights.Num(), PCAData.NumComponents);
        UE_LOG(LogSMILTools, Error, TEXT("%s"), *ErrorMsg);
        if (bShowDebugMessages && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *ErrorMsg);
        }
        return false;
    }

    UE_LOG(LogSMILTools, Log, TEXT("SamplePCATransforms: Starting sampling for %d bones using %d components"), 
        PCAData.NumBones, PCAData.NumComponents);
    UE_LOG(LogSMILTools, Log, TEXT("SamplePCATransforms: Using separate translation and scale weight arrays"));

    // Prepare output arrays
    OutBoneNames = PCAData.BoneNames; // Copy bone names directly
    OutTransforms.SetNum(PCAData.NumBones);

    // Process each bone
    for (int32 BoneIndex = 0; BoneIndex < PCAData.NumBones; BoneIndex++)
    {
        FVector AccumulatedScale = FVector::ZeroVector;
        FVector AccumulatedTranslation = FVector::ZeroVector;

        // Apply weighted sum of all principal components for this bone
        for (int32 ComponentIndex = 0; ComponentIndex < PCAData.NumComponents; ComponentIndex++)
        {
            if (!PCAData.PrincipalComponents.IsValidIndex(ComponentIndex))
            {
                UE_LOG(LogSMILTools, Warning, TEXT("SamplePCATransforms: Missing component %d"), ComponentIndex);
                continue;
            }

            const FPCAComponentData& Component = PCAData.PrincipalComponents[ComponentIndex];
            if (!Component.BoneTransforms.IsValidIndex(BoneIndex))
            {
                UE_LOG(LogSMILTools, Warning, TEXT("SamplePCATransforms: Missing bone %d in component %d"), BoneIndex, ComponentIndex);
                continue;
            }

            const FPCABoneTransform& BoneTransform = Component.BoneTransforms[BoneIndex];
            float TranslationWeight = TranslationWeights[ComponentIndex];
            float ScaleWeight = ScaleWeights[ComponentIndex];

            // Accumulate weighted transformations using separate weights
            AccumulatedScale += BoneTransform.Scale * ScaleWeight;
            AccumulatedTranslation += BoneTransform.Translation * TranslationWeight;

            UE_LOG(LogSMILTools, VeryVerbose, TEXT("SamplePCATransforms: Bone '%s' PC_%d: TransWeight=%.6f, ScaleWeight=%.6f, Scale=(%.6f,%.6f,%.6f), Trans=(%.6f,%.6f,%.6f)"),
                *PCAData.BoneNames[BoneIndex], ComponentIndex + 1, TranslationWeight, ScaleWeight,
                BoneTransform.Scale.X, BoneTransform.Scale.Y, BoneTransform.Scale.Z,
                BoneTransform.Translation.X, BoneTransform.Translation.Y, BoneTransform.Translation.Z);
        }

        // Create final transform for this bone
        FTransform& FinalTransform = OutTransforms[BoneIndex];
        
        // Set scale (base scale of 1.0 + accumulated PCA scale)
        FinalTransform.SetScale3D(FVector::OneVector + AccumulatedScale);
        
        // Set translation (accumulated PCA translation)
        FinalTransform.SetTranslation(AccumulatedTranslation);
        
        // Set rotation to identity (no rotation as requested)
        FinalTransform.SetRotation(FQuat::Identity);

        UE_LOG(LogSMILTools, VeryVerbose, TEXT("SamplePCATransforms: Bone '%s' - Accumulated: Scale=(%.6f,%.6f,%.6f), Trans=(%.6f,%.6f,%.6f)"),
            *PCAData.BoneNames[BoneIndex],
            AccumulatedScale.X, AccumulatedScale.Y, AccumulatedScale.Z,
            AccumulatedTranslation.X, AccumulatedTranslation.Y, AccumulatedTranslation.Z);

        UE_LOG(LogSMILTools, VeryVerbose, TEXT("SamplePCATransforms: Bone '%s' - Final: Scale=(%.6f,%.6f,%.6f), Trans=(%.6f,%.6f,%.6f)"),
            *PCAData.BoneNames[BoneIndex],
            FinalTransform.GetScale3D().X, FinalTransform.GetScale3D().Y, FinalTransform.GetScale3D().Z,
            FinalTransform.GetTranslation().X, FinalTransform.GetTranslation().Y, FinalTransform.GetTranslation().Z);
    }

    // Log sampling results
    LogPCATransforms(OutBoneNames, OutTransforms, TranslationWeights, ScaleWeights);

    // Display success message
    FString SuccessMsg = FString::Printf(TEXT("SMILTools: Sampled %d bone transforms"), OutBoneNames.Num());
    UE_LOG(LogSMILTools, Log, TEXT("SamplePCATransforms: %s"), *SuccessMsg);
    if (bShowDebugMessages && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, *SuccessMsg);
    }

    return true;
}

void USMILTools::LogPCATransforms(const TArray<FString>& BoneNames, const TArray<FTransform>& Transforms, const TArray<float>& TranslationWeights, const TArray<float>& ScaleWeights)
{
    UE_LOG(LogSMILTools, Log, TEXT("=== PCA Transform Sampling Summary ==="));
    UE_LOG(LogSMILTools, Log, TEXT("Number of Bones: %d"), BoneNames.Num());
    UE_LOG(LogSMILTools, Log, TEXT("Number of Transforms: %d"), Transforms.Num());
    UE_LOG(LogSMILTools, Log, TEXT("Number of Translation Weights: %d"), TranslationWeights.Num());
    UE_LOG(LogSMILTools, Log, TEXT("Number of Scale Weights: %d"), ScaleWeights.Num());

    if (BoneNames.Num() > 0 && Transforms.Num() > 0)
    {
        // Calculate transform statistics
        FVector MinScale = Transforms[0].GetScale3D();
        FVector MaxScale = Transforms[0].GetScale3D();
        FVector MinTranslation = Transforms[0].GetTranslation();
        FVector MaxTranslation = Transforms[0].GetTranslation();

        for (const FTransform& Transform : Transforms)
        {
            FVector Scale = Transform.GetScale3D();
            FVector Translation = Transform.GetTranslation();

            MinScale.X = FMath::Min(MinScale.X, Scale.X);
            MinScale.Y = FMath::Min(MinScale.Y, Scale.Y);
            MinScale.Z = FMath::Min(MinScale.Z, Scale.Z);

            MaxScale.X = FMath::Max(MaxScale.X, Scale.X);
            MaxScale.Y = FMath::Max(MaxScale.Y, Scale.Y);
            MaxScale.Z = FMath::Max(MaxScale.Z, Scale.Z);

            MinTranslation.X = FMath::Min(MinTranslation.X, Translation.X);
            MinTranslation.Y = FMath::Min(MinTranslation.Y, Translation.Y);
            MinTranslation.Z = FMath::Min(MinTranslation.Z, Translation.Z);

            MaxTranslation.X = FMath::Max(MaxTranslation.X, Translation.X);
            MaxTranslation.Y = FMath::Max(MaxTranslation.Y, Translation.Y);
            MaxTranslation.Z = FMath::Max(MaxTranslation.Z, Translation.Z);
        }

        UE_LOG(LogSMILTools, Log, TEXT("Transform Statistics:"));
        UE_LOG(LogSMILTools, Log, TEXT("  Scale Range: (%.6f,%.6f,%.6f) to (%.6f,%.6f,%.6f)"),
            MinScale.X, MinScale.Y, MinScale.Z, MaxScale.X, MaxScale.Y, MaxScale.Z);
        UE_LOG(LogSMILTools, Log, TEXT("  Translation Range: (%.6f,%.6f,%.6f) to (%.6f,%.6f,%.6f)"),
            MinTranslation.X, MinTranslation.Y, MinTranslation.Z, MaxTranslation.X, MaxTranslation.Y, MaxTranslation.Z);

        // Log weights used
        UE_LOG(LogSMILTools, Log, TEXT("Translation Weights Used:"));
        for (int32 i = 0; i < FMath::Min(TranslationWeights.Num(), 10); i++) // Show first 10 weights
        {
            UE_LOG(LogSMILTools, Log, TEXT("  PC_%d: %.6f"), i + 1, TranslationWeights[i]);
        }
        if (TranslationWeights.Num() > 10)
        {
            UE_LOG(LogSMILTools, Log, TEXT("  ... and %d more"), TranslationWeights.Num() - 10);
        }

        UE_LOG(LogSMILTools, Log, TEXT("Scale Weights Used:"));
        for (int32 i = 0; i < FMath::Min(ScaleWeights.Num(), 10); i++) // Show first 10 weights
        {
            UE_LOG(LogSMILTools, Log, TEXT("  PC_%d: %.6f"), i + 1, ScaleWeights[i]);
        }
        if (ScaleWeights.Num() > 10)
        {
            UE_LOG(LogSMILTools, Log, TEXT("  ... and %d more"), ScaleWeights.Num() - 10);
        }

        // Log sample bone transforms
        int32 NumToShow = FMath::Min(5, BoneNames.Num());
        UE_LOG(LogSMILTools, Log, TEXT("Sample Bone Transforms (first %d):"), NumToShow);
        for (int32 i = 0; i < NumToShow; i++)
        {
            const FTransform& Transform = Transforms[i];
            UE_LOG(LogSMILTools, Log, TEXT("  [%d] %s:"), i, *BoneNames[i]);
            UE_LOG(LogSMILTools, Log, TEXT("    Scale: (%.6f, %.6f, %.6f)"), 
                Transform.GetScale3D().X, Transform.GetScale3D().Y, Transform.GetScale3D().Z);
            UE_LOG(LogSMILTools, Log, TEXT("    Translation: (%.6f, %.6f, %.6f)"), 
                Transform.GetTranslation().X, Transform.GetTranslation().Y, Transform.GetTranslation().Z);
        }

        if (BoneNames.Num() > 5)
        {
            UE_LOG(LogSMILTools, Log, TEXT("  ... and %d more bones"), BoneNames.Num() - 5);
        }
    }

    UE_LOG(LogSMILTools, Log, TEXT("=== End PCA Transform Summary ==="));
} 