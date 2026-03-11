# OmniTrax — Resource Summary

## Overview
**OmniTrax** is an open-source Blender Add-on for deep learning-driven multi-animal tracking and pose estimation. Published in *JOSS* (Journal of Open Source Software).

## Key Facts
- **Repository**: https://github.com/FabianPlum/OmniTrax
- **Stars**: 39 | **Forks**: 4
- **License**: MIT
- **JOSS paper**: https://joss.theoj.org/papers/48a3822e63785944c2014ea7696037c3
- **Zenodo DOI**: 10.5281/zenodo.10817892

## What It Does
- Integrates YOLOv3/v4 detection with buffer-and-recover tracking in Blender's motion tracking pipeline
- Supports DeepLabCut-Live for marker-less pose estimation on arbitrary numbers of animals
- Designed as plug-and-play for biologists — no programming expertise required
- Handles large video files with numerous freely moving subjects

## Use Cases
- Population monitoring in changing environments (where background subtraction fails)
- Detection models trained on synthetic data from replicAnt
- Annotation of training/validation data for detector and tracker networks
- Size classification and unsupervised behavioural clustering

## Connection to replicAnt
- OmniTrax detection and pose estimation models can be trained entirely on replicAnt synthetic data
- Demonstrates the practical downstream value of replicAnt's synthetic data generation
- Part of the same open-source ecosystem by the same research group

## Grant Relevance
- Shows that replicAnt's synthetic data has practical, validated downstream applications
- Published in a peer-reviewed software journal (JOSS)
- Active user community in biology research
