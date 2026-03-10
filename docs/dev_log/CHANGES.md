# replicAnt Pipeline — Comprehensive Change Log

## Overview

This document details all changes made to the replicAnt project, including:
1. **Engine migration** from Unreal Engine 5.0.3 → 5.4
2. **SMIL (parametric) mesh model support** — new C++ module, Blueprint nodes, PCA data pipeline
3. **Three new subject models** — SMILyANT, SMILySTICK, and Mouse
4. **New SubjectBase classes** — QuadrupedBase and ArachBase alongside existing InsectBase
5. **Supporting PCA data files** — morph and shape CSV data for the SMIL pipeline

---

## 1. Unreal Engine 5.0.3 → 5.4 Migration

### 1.1 Project File (`replicAnt.uproject`)

```diff
- "EngineAssociation": "5.0"
+ "EngineAssociation": "5.4"
```

**New plugins enabled for 5.4:**
- `AnimationWarping`
- `MotionWarping`
- `PoseSearch`
- `JsonBlueprintUtilities`
- `OpenCV`
- `DataprepEditor` / `DataprepGeometryOperations`
- `CodeEditor` / `CodeView`
- `PythonFoundationPackages`
- `AssetSearch`
- `BlueprintMaterialTextureNodes`
- `GeometryScripting`
- `VisualStudioTools`
- `HairStrands` / `AlembicHairImporter`
- `SkeletalMeshModelingTools`

### 1.2 Build Targets (`Source/*.Target.cs`)

```diff
- DefaultBuildSettings = BuildSettingsVersion.V2;
+ DefaultBuildSettings = BuildSettingsVersion.V5;
```

Applied to both `replicAntTarget` (Game) and `replicAntEditorTarget` (Editor).

### 1.3 Config Changes

#### `DefaultEngine.ini`

| Setting | Change |
|---------|--------|
| `r.DynamicGlobalIlluminationMethod` | Set to `2` (Lumen) |
| `r.ReflectionMethod` | Set to `2` (Lumen) |
| `r.SkinCache.CompileShaders` | `True` |
| `r.RayTracing` | `True` |
| `r.Shadow.Virtual.Enable` | `1` (Virtual Shadow Maps) |
| `r.PathTracing` | `True` |
| `r.Lumen.HardwareRayTracing` | `True` |
| `r.Lumen.HardwareRayTracing.LightingMode` | `2` |
| `r.GPUSkin.UnlimitedBoneInfluences` | `True` |
| `r.Substrate` | `True` (new UE 5.4 material system) |
| `r.Substrate.Debug.AdvancedVisualizationShaders` | `True` |
| `r.Lumen.TranslucencyReflections.FrontLayer.EnableForProject` | `True` |
| `DefaultGraphicsRHI` | `DefaultGraphicsRHI_DX12` |
| Shader formats | Added `PCD3D_SM6` alongside `PCD3D_SM5` |
| Linux targeted RHIs | Added `SF_VULKAN_SM6` alongside `SF_VULKAN_SM5` |
| `gc.MaxObjectsInEditor` | `214748364` (raised limit) |
| `gc.MaxObjectsInGame` | `214748364` (raised limit) |
| `bCanBlueprintsTickByDefault` | `False` |
| `MinDesiredFrameRate` | `0.1` |
| `MaximumLoopIterationCount` | `0` (unlimited) |

#### `DefaultEditor.ini`
- Unit enums migrated from `EUnit::` prefix format to plain string format (e.g. `EUnit::Centimeters` → `Centimeters`)

---

## 2. SMIL (Parametric) Model Support — C++ Module

### 2.1 New Source Files

The core SMIL support is implemented as a **Blueprint Function Library** in native C++, exposing new nodes to the UE5 Blueprint graph.

#### `Source/replicAnt/Public/SMILTools.h`
**New file** — Header defining the SMIL tools library and all associated data structures.

**Structs (all `BlueprintType`, exported with `REPLICANT_API`):**

| Struct | Purpose |
|--------|---------|
| `FPCABoneTransform` | Holds per-bone PCA transform data: `FVector Scale` + `FVector Translation` |
| `FPCAComponentData` | Array of `FPCABoneTransform` for one principal component |
| `FPCAMorphData` | Complete PCA dataset: bone names, all PC components, counts, validity flag, `IsValid()` helper |

**Blueprint-callable functions (`USMILTools : UBlueprintFunctionLibrary`):**

| Function | Signature | Description |
|----------|-----------|-------------|
| `LoadPCADataFromCSV` | `(FilePath, bShowDebug) → (OutPCAData, OutNumComponents, OutNumBones)` | Loads and parses a PCA morph CSV file with full validation |
| `GeneratePCAWeights` | `(NumComponents, StdDev, bShowDebug, RandomStream) → (OutWeights)` | Generates normally-distributed PCA weights using a seeded `FRandomStream` for reproducibility |
| `GeneratePCAWeightsSimple` | `(NumComponents, StdDev, bShowDebug) → (OutWeights)` | Same as above but uses global random (no seed control) |
| `SamplePCATransforms` | `(PCAData, TranslationWeights, ScaleWeights, bShowDebug) → (OutBoneNames, OutTransforms)` | Samples bone transforms from PCA data using **separate** translation and scale weight arrays |

All functions are marked `CallInEditor` for testing convenience.

#### `Source/replicAnt/Private/SMILTools.cpp`
**New file** — 735-line implementation containing:

- **CSV Parsing Pipeline:**
  - `LoadPCADataFromCSV()` — Main entry point: file validation → header parsing → bone data extraction
  - `ParseCSVLine()` — Tokenizer splitting on commas with whitespace trimming
  - `ValidateCSVHeader()` — Validates `joint_name` first column, detects PC count from `PC_N_{scale|translation}_{x|y|z}` column pattern
  - `ParseBoneData()` — Extracts 6 floats per PC per bone (3× scale + 3× translation)

- **PCA Weight Generation:**
  - `GeneratePCAWeights()` — Box-Muller transform using `FRandomStream` for reproducible normal distribution sampling
  - `GeneratePCAWeightsSimple()` — Box-Muller with global random and value caching (generates pairs, caches second)
  - `GenerateNormalRandom()` / `GenerateNormalRandomGlobal()` — Core Box-Muller implementations

- **PCA Transform Sampling:**
  - `SamplePCATransforms()` — Computes weighted sum across all PCs for each bone, producing final `FTransform` array (scale = 1 + accumulated, translation = accumulated, rotation = identity)

- **Logging / Diagnostics:**
  - `LogPCADataInfo()` — Summary of loaded PCA data (bone count, component count, sample values)
  - `LogPCAWeights()` — Statistics of generated weights (mean, std dev, min/max, first 5 samples)
  - `LogPCATransforms()` — Transform statistics, weight values, sample bone transforms

All functions include on-screen debug messaging via `GEngine->AddOnScreenDebugMessage()` when `bShowDebugMessages` is enabled.

#### `Source/replicAnt/replicAnt.Build.cs`
```diff
+ PrivateDependencyModuleNames.AddRange(new string[] { "KismetCompiler", "BlueprintGraph", "ToolMenus" });
```
Added dependencies required for the Blueprint Function Library (`KismetCompiler`, `BlueprintGraph`) and editor integration (`ToolMenus`).

#### `Source/replicAnt/replicAnt.cpp`
```diff
+ #include "SMILTools.h"
```
Module implementation now includes the SMILTools header to ensure registration.

### 2.2 PCA Data Files (`SMIL_files/`)

| File | Description |
|------|-------------|
| `smil_morph_PC_data.csv` | PCA-transformed morph data — 13 PCs × 55 bones, 6 values per PC per bone (scale xyz + translation xyz). Header: `joint_name,PC_1_scale_x,...,PC_13_translation_z` |
| `smil_morph_data.csv` | Raw morph data per specimen — 80 ant species × 55 bones, scale + translation per bone per species |

---

## 3. New Subject Models

### 3.1 SMILyANT (`Content/Subjects/SMILyANT/`)

A **parametric ant model** driven by the SMIL PCA system. Uses the unposed SMPL mesh with PCA-driven bone transforms for generating morphologically diverse ant specimens.

| Asset | Type |
|-------|------|
| `BP_sbj_SMILyANT.uasset` | Subject Blueprint (parametric ant) |
| `SMPL_Object.uasset` | Skeletal Mesh (SMIL base mesh) |
| `SMPL_Object_Skeleton.uasset` | Skeleton asset |
| `SMPL_Object_PhysicsAsset.uasset` | Physics asset |
| `M_SMIL_textured.uasset` | Textured material |
| `Material.uasset` | Base material |
| `Atta_vollenweideri_albedo.uasset` | Albedo texture (Atta vollenweideri) |
| `Atta_vollenweideri_albedo_normal.uasset` | Normal map (Atta vollenweideri) |
| `Dynomyrmex_gigas_albedo.uasset` | Albedo texture (Dynomyrmex gigas) |
| `Dynomyrmex_gigas_albedo_normal.uasset` | Normal map (Dynomyrmex gigas) |
| `Mysterium_oberthueri_albedo.uasset` | Albedo texture (Mysterium oberthueri) |
| `Mysterium_oberthueri_albedo_normal.uasset` | Normal map (Mysterium oberthueri) |

### 3.2 SMILySTICK (`Content/Subjects/SMILySTICK/`)

A **parametric stick insect model** using the SMIL framework with stick insect-specific textures.

| Asset | Type |
|-------|------|
| `BP_sbj_SMILySTICK.uasset` | Subject Blueprint (parametric stick insect) |
| `SMPL_Object.uasset` | Skeletal Mesh (SMIL base mesh) |
| `SMPL_Object_Skeleton.uasset` | Skeleton asset |
| `SMPL_Object_PhysicsAsset.uasset` | Physics asset |
| `M_SMILySTICK_textured.uasset` | Textured material |
| `SMIL_tex_brockphasma.uasset` | Albedo texture (Brockphasma) |
| `SMIL_tex_brockphasma_normal.uasset` | Normal map (Brockphasma) |
| `SMIL_tex_peruphasma.uasset` | Albedo texture (Peruphasma) |
| `SMIL_tex_peruphasma_normal.uasset` | Normal map (Peruphasma) |
| `SMIL_tex_sungaya_3rd_instar.uasset` | Albedo texture (Sungaya 3rd instar) |
| `SMIL_tex_sungaya_3rd_instar_normal.uasset` | Normal map (Sungaya 3rd instar) |

### 3.3 Mouse (`Content/Subjects/Mouse/`)

A **full mouse model** with fur simulation support using Unreal's HairStrands/Groom system.

| Asset | Type |
|-------|------|
| **Mesh/** | |
| `BP_Mouse.uasset` | Subject Blueprint (standard mouse) |
| `BP_SMILyMOUSE.uasset` | Subject Blueprint (parametric mouse variant) |
| `SK_Mouse.uasset` | Skeletal Mesh |
| `SK_Mouse_Skeleton.uasset` | Skeleton |
| `SK_Mouse_PhysicsAsset.uasset` | Physics asset |
| `SM_Mouse.uasset` | Static Mesh variant |
| `CA_Mouse.uasset` | Control Rig / Animation asset |
| `M_Mouse.uasset` | Body material |
| `M_Mouse_Eye.uasset` | Eye material |
| `T_Mouse.uasset` | Body texture |
| `T_Mouse_N.uasset` | Normal map |
| `T_Fur.uasset` | Fur texture |
| `C_MouseFurColors.uasset` | Fur color curve/data asset |
| **fur/** | |
| `fur_SK_Mouse_Binding.uasset` | Groom binding (base fur) |
| `fur2_SK_Mouse_Binding.uasset` | Groom binding (variant 2) |
| `fur2.uasset` | Groom asset (variant 2) |
| `fur2B_SK_Mouse_Binding.uasset` | Groom binding (variant 2B) |
| `fur2B.uasset` | Groom asset (variant 2B) |
| `fur2C_SK_Mouse_Binding.uasset` | Groom binding (variant 2C) |
| `fur2C.uasset` | Groom asset (variant 2C) |
| `fur2D_SK_Mouse_Binding.uasset` | Groom binding (variant 2D) |
| `fur2D.uasset` | Groom asset (variant 2D) |
| `FurA_FollicleTexture.uasset` | Follicle placement texture |
| `FurA.uasset` | Groom asset (variant A) |
| `FurB_SK_Mouse_Binding.uasset` | Groom binding (variant B) |
| `FurB.uasset` | Groom asset (variant B) |
| **Root level** | |
| `M_Fur.uasset` | Fur material |
| `M_Mouse_Fur.uasset` | Mouse-specific fur material |
| `MF_Mouse_Fur_Color.uasset` | Fur color material function |
| `SSP_Mouse.uasset` | Subsurface profile |
| `lvl_Test_Mouse.umap` | Test level |
| `lvl_Test_Mouse2.umap` | Test level (variant 2) |

---

## 4. SubjectBase Classes (`Content/Internal/SubjectBase/`)

### 4.1 New Base Classes

The original replicAnt only had `InsectBase`. Three base subject archetypes now exist:

#### InsectBase (existing, updated)
| Asset | Purpose |
|-------|---------|
| `BP_sbj_InsectBase.uasset` | Base Blueprint for 6-legged insect subjects |
| `InsectBase/ABP_InsectBase.uasset` | Animation Blueprint |
| `InsectBase/IKR_InsectBase.uasset` | IK Rig |
| `InsectBase/SK_InsectBase_Skeleton.uasset` | Skeleton |
| `SC_InsectLegIK.uasset` | Insect leg IK solver control |

#### QuadrupedBase (NEW)
| Asset | Purpose |
|-------|---------|
| `BP_sbj_QuadrupedBase.uasset` | Base Blueprint for 4-legged subjects (e.g., Mouse) |
| `QuadrupedBase/ABP_QuadrupedBase.uasset` | Animation Blueprint |
| `QuadrupedBase/IKR_QuadrupedBase.uasset` | IK Rig |
| `QuadrupedBase/SK_QuadrupedBase_Skeleton.uasset` | Skeleton |
| `QuadrupedBase/SK_QuadrupedBase_PhysicsAsset.uasset` | Physics Asset |
| `QuadrupedBase/CR_Head.uasset` | Head control rig |
| `QuadrupedBase/CR_Tail.uasset` | Tail control rig |
| `SC_QuadrupedLegIK.uasset` | Quadruped leg IK solver control |

#### ArachBase (NEW)
| Asset | Purpose |
|-------|---------|
| `BP_sbj_ArachBase.uasset` | Base Blueprint for 8-legged subjects (arachnids) |
| `ArachBase/ABP_ArachBase.uasset` | Animation Blueprint |
| `ArachBase/IKR_ArachBase2.uasset` | IK Rig |
| `ArachBase/SK_ArachBase1.uasset` | Skeletal Mesh |
| `ArachBase/SK_ArachBase_Skeleton1.uasset` | Skeleton |
| `ArachBase/SK_ArachBase_Skeleton1_Sequence.uasset` | Skeleton Sequence |
| `ArachBase/SK_ArachBase_Physics.uasset` | Physics Asset |
| `SC_ArachLegIK.uasset` | Arachnid leg IK solver control |

### 4.2 Shared Components
| Asset | Purpose |
|-------|---------|
| `BP_SubjectBase.uasset` | Root subject base Blueprint (parent of all subject types) |
| `S_BoneTransforms.uasset` | Bone transform struct (used by SMIL pipeline) |
| `CR_BoneControls.uasset` | Shared bone control rig |
| `AC_Tail.uasset` | Tail animation component |
| `BPI_Tail.uasset` | Tail Blueprint Interface |

---

## 5. Generator / Level Configuration

### New Map
- `Content/Generator_config/Generator_Maus.umap` — Generator configuration level for Mouse subject

---

## 6. Rendering & Material Pipeline

### 6.1 Substrate Material System
UE 5.4's new **Substrate** material model is enabled:
```ini
r.Substrate=True
r.Substrate.Debug.AdvancedVisualizationShaders=True
```

### 6.2 Fur / Groom Rendering
The Mouse model uses **HairStrands** (Groom) for fur simulation:
- Multiple fur groom variants (fur2, fur2B, fur2C, fur2D, FurA, FurB)
- Dedicated fur materials (`M_Fur`, `M_Mouse_Fur`)
- Follicle placement textures
- Groom-to-skeleton bindings

### 6.3 Rendering Features Enabled
- Lumen GI with hardware ray tracing
- Virtual Shadow Maps
- Path Tracing
- Unlimited bone influences (`r.GPUSkin.UnlimitedBoneInfluences=True`)
- Custom depth stencil (mode 3)
- DX12 with SM6 support

---

## 7. Summary of File Counts

| Category | Files Added/Modified |
|----------|---------------------|
| C++ Source (SMILTools) | 2 new (.h, .cpp), 3 modified (.Build.cs, .cpp, .h) |
| Build targets | 2 modified (.Target.cs) |
| Project config | 1 modified (.uproject) |
| Engine config | 2 modified (.ini) |
| PCA data files | 2 new (.csv) |
| SMILyANT assets | 12 new (.uasset) |
| SMILySTICK assets | 11 new (.uasset) |
| Mouse assets | 33 new (.uasset, .umap) |
| SubjectBase (Quadruped) | 7 new (.uasset) |
| SubjectBase (Arach) | 8 new (.uasset) |
| SubjectBase (shared) | 5 new (.uasset) |
| Render passes | 7 modified (.uasset) |
| Generator config | 1 new (.umap) |

---

*Updated: 2026-03-10*
