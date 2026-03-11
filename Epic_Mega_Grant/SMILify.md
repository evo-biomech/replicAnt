# SMILify — Resource Summary

## Overview
**SMILify** is an open-source framework extending parametric body modelling (like SMPL for humans) to virtually any rigged 3D animal mesh. It enables species-agnostic 3D pose and shape reconstruction from video.

## Key Facts
- **Repository**: https://github.com/FabianPlum/SMILify
- **Stars**: 7 | **Forks**: 0
- **Description**: "Like SMPL but for anything"
- **Based on**: SMALify (Biggs et al.), extended for arbitrary armature configurations
- **Dependencies**: PyTorch, PyTorch3D, SLEAP/DeepLabCut for 2D pose input

## What It Does
1. **Blender Add-on** for building parametric animal models from 3D scan collections
2. **Learns anatomically plausible shape spaces** (PCA) from scan data — constraining what body proportions are possible for a species
3. **Neural inference pipeline** that takes 2D pose estimates (SLEAP/DLC) or triangulated 3D keypoints to produce dense 3D reconstructions of body pose and shape
4. **Synthetic-to-real transfer** — trains first on synthetic data (from replicAnt), then adapts to real recordings

## Demonstrated Applications
- **Stick insects** — multi-species parametric model (Brockphasma, Peruphasma, Sungaya)
- **Ants** — 80-species parametric model with PCA-driven morphological variation
- **Mice** — parametric mouse model with multi-camera (18-camera) reconstruction
- Single- and multi-camera systems supported

## Connection to replicAnt
- replicAnt generates the synthetic training data that SMILify's neural inference networks learn from
- The SMIL C++ module in replicAnt (SMILTools) enables direct PCA-driven parametric variation during synthetic data generation
- PCA morph data files (CSV) bridge the Blender modelling pipeline and UE5 rendering pipeline
- replicAnt's SMILyANT, SMILySTICK, and SMILyMOUSE subjects are the UE5-side counterparts of SMILify's parametric models

## Grant Relevance
- Represents the cutting-edge downstream application of replicAnt's synthetic data
- Demonstrates the full pipeline: 3D scanning (scAnt) → synthetic data (replicAnt) → neural 3D reconstruction (SMILify)
- Novel contribution to computer vision for biology — no existing tool offers species-agnostic parametric body modelling
