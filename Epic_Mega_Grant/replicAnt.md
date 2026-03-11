# replicAnt — Resource Summary

## Overview
**replicAnt** is an open-source pipeline for generating richly annotated synthetic images of animals in complex environments using Unreal Engine 5. Published in *Nature Communications* (2023).

## Key Facts
- **Repository**: https://github.com/evo-biomech/replicAnt
- **Stars**: 67 | **Forks**: 9
- **License**: MIT
- **Engine**: Unreal Engine 5.4 (migrated from 5.0.3)
- **Paper**: Plum, Bulla, Beck, Imirzian & Labonte (2023). *Nature Communications*, 14. DOI: [10.1038/s41467-023-42898-9](https://doi.org/10.1038/s41467-023-42898-9)
- **Zenodo DOI**: https://zenodo.org/badge/latestdoi/392289312

## What It Does
- Takes rigged, textured 3D animal models as input
- Places them in procedurally generated UE5 scenes with randomised ground topology, materials, decals, and lighting
- Produces multi-pass renders: RGB image, instance ID, depth, and normal maps
- Outputs annotation files compatible with major CV frameworks
- Supports detection (YOLO), tracking, 2D/3D pose estimation (DLC/SLEAP), and semantic segmentation

## Recent Additions (UE 5.4 Migration, March 2026)
- **SMIL parametric mesh support** — C++ Blueprint Function Library for PCA-driven morphological variation
- **Three new built-in subject models**: SMILyANT (parametric ant, 80 species), SMILySTICK (parametric stick insect), Mouse (with fur/groom simulation)
- **New SubjectBase archetypes**: QuadrupedBase (4-legged), ArachBase (8-legged), alongside existing InsectBase
- **Rendering upgrades**: Lumen GI with hardware ray tracing, Virtual Shadow Maps, Path Tracing, Substrate material system, DX12/SM6

## Pre-existing Subject Models (downloadable)
- Leafcutter ants (*Atta vollenweideri*) — various worker sizes
- Desert termites (*Gnathamitermes*) — worker and soldier
- Praying mantis (*Gongylus gongylodes*)
- Stick insects (*Sungaya inexpectata*, *Peruphasma schultei*)
- Leaf-footed bug (*Leptoglossus zonatus*)
- Desert ants (*Pogonomyrmex desertorum*)

## Downstream Applications
Synthetic data from replicAnt directly feeds into:
- **OmniTrax** — multi-animal tracking and pose estimation
- **SMILify** — 3D pose and shape reconstruction from 2D/3D keypoints
- Custom detection, segmentation, and pose estimation networks

## Grant Relevance
- Demonstrates deep integration with Unreal Engine as core infrastructure
- Open-source, MIT-licensed, community-driven
- Published in a top-tier journal (*Nature Communications*)
- Actively maintained with significant recent development
- Addresses a real scientific need: automated generation of training data for animal behaviour research
