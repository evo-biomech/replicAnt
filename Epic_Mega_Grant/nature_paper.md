# replicAnt — Nature Communications Paper

## Citation
Plum, F., Bulla, R., Beck, H.K., Imirzian, N. & Labonte, D. (2023). replicAnt: a pipeline for generating annotated images of animals in complex environments using Unreal Engine. *Nature Communications*, 14. DOI: [10.1038/s41467-023-42898-9](https://doi.org/10.1038/s41467-023-42898-9)

## Abstract (from paper)
Deep learning-based computer vision methods are transforming animal behavioural research. Training such models typically requires large, manually annotated datasets — a major bottleneck. replicAnt addresses this by generating annotated synthetic images of animals in complex environments using Unreal Engine. The pipeline takes textured, rigged 3D animal models and places them in procedurally generated scenes with configurable randomisation of terrain, materials, lighting, and camera parameters. It produces multi-pass image data (RGB, depth, instance segmentation, normals) with automatic annotations compatible with common computer vision frameworks. The authors demonstrate that models trained on synthetic data alone, or with minimal real-data fine-tuning, achieve performance competitive with models trained on manually annotated real data — across detection, tracking, 2D and 3D pose estimation, and semantic segmentation tasks.

## Key Contributions
1. First general-purpose synthetic data pipeline for animal computer vision built on Unreal Engine
2. Demonstrated competitive performance with real-data-trained models across multiple CV tasks
3. Dramatically reduces annotation effort — hours of manual labelling replaced by automated generation
4. Open-source, modular, and extensible to new species and body plans
5. Integration with existing tools (YOLO, DeepLabCut, SLEAP, MMDetection, etc.)

## Impact
- Published in *Nature Communications* — one of the highest-impact open-access journals
- Demonstrates Unreal Engine as serious scientific infrastructure
- Bridges the gap between game engine technology and biological research
- Directly credited as using Unreal Engine in its core methodology

## Grant Relevance
- Peer-reviewed validation that UE-based synthetic data matches real-data performance
- High-visibility publication demonstrating UE's value to the scientific community
- Concrete evidence of Unreal Engine enabling novel research that was previously impractical
