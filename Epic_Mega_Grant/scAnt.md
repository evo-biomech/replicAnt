# scAnt — Resource Summary

## Overview
**scAnt** is an open-source, low-cost macro 3D scanning platform for creating digital 3D models of arthropods and other small objects. Published in *PeerJ* (2021).

## Key Facts
- **Repository**: https://github.com/evo-biomech/scAnt
- **Stars**: 239 | **Forks**: 45
- **License**: MIT
- **Paper**: Plum & Labonte (2021). *PeerJ*, 9:e11155. DOI: [10.7717/peerj.11155](https://doi.org/10.7717/peerj.11155)
- **3D model gallery**: http://bit.ly/ScAnt-3D and [Sketchfab Collection](https://sketchfab.com/EvoBiomech/collections/scant-collection)

## What It Does
- Automated 3D scanning using FLIR Blackfly cameras or DSLRs with motorised turntable
- Focus stacking and masking of images for photogrammetry
- All structural components 3D-printable or laser-cuttable (files on Thingiverse)
- GUI-based pipeline from capture to processed image stacks
- Supports both Windows and Ubuntu

## Connection to replicAnt
- scAnt produces the high-fidelity 3D models that serve as input to replicAnt
- The scAnt → replicAnt pipeline demonstrates end-to-end: physical specimen → 3D scan → synthetic training data
- Many of replicAnt's built-in subject models were originally scanned with scAnt

## Connection to SMILify
- 3D scan collections from scAnt provide the morphological data from which SMILify learns species-specific shape spaces
- More scans → better parametric models → more realistic synthetic data → better neural inference

## Grant Relevance
- Upstream component of the full pipeline — demonstrates the ecosystem is end-to-end
- Most popular repository in the ecosystem (239 stars, 45 forks)
- Shows hardware + software integration — low-cost, accessible, open-source
- Published peer-reviewed paper with demonstrated impact
