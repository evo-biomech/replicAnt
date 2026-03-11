# Epic MegaGrants — replicAnt Proposal Draft

---

## APPLICANT INFORMATION

| Field | Value |
|-------|-------|
| Applying as | Company/team |
| How did you hear about us? | Other — (TODO: specify) |
| Country | Germany |
| Team size | <5 |
| Company/Institution | (TODO: scAnt3D / university affiliation?) |
| Company website | (TODO: scAnt3D website once updated) |

---

## PROJECT INFORMATION

| Field | Value |
|-------|-------|
| Project name | replicAnt |
| Project media link | (TODO: add demo video / trailer link) |
| Project build link | (TODO: optional) |
| Project website | https://github.com/evo-biomech/replicAnt |
| Phase | Prototype |
| Engine | Unreal Engine |
| Current engine version | UE5 (5.4) |
| Open source? | My project is open source AND gives back to the 3D community |

### Open Source Explanation

replicAnt and all associated tools are released under the MIT License. The ecosystem comprises four interconnected open-source projects — replicAnt (synthetic data generation, UE5), scAnt (3D scanning), OmniTrax (multi-animal tracking), and SMILify (3D pose and shape reconstruction) — collectively serving a growing community of biologists and computer vision researchers worldwide. All source code, 3D assets, and documentation are freely available. The project has been published in Nature Communications and the Journal of Open Source Software, and all associated datasets and trained models are shared openly via Zenodo.

---

## PROJECT ELEVATOR PITCH (max 225 chars)

replicAnt uses Unreal Engine to generate annotated synthetic training data of animals, enabling deep learning for behavioural research — across species, without manual labelling.

(224 characters)

---

## FULL PROJECT DETAILS

### Origin

replicAnt began as a research project to solve a fundamental bottleneck in animal behaviour research: the prohibitive cost of manually annotating training data for deep learning. Modern computer vision methods — detection, tracking, pose estimation, segmentation — have transformed how we study animal behaviour, but they demand thousands of labelled images per species, per task. For the vast majority of the animal kingdom, such datasets do not exist, and creating them by hand is impractical.

We recognised that Unreal Engine's photorealistic rendering, procedural generation capabilities, and Blueprint system made it uniquely suited to address this problem. By placing rigged 3D animal models into procedurally generated environments with full control over lighting, materials, camera parameters, and animal pose, we could generate unlimited annotated training data — automatically, at scale, and for any species with a 3D model.

### What replicAnt Does

replicAnt is a configurable synthetic data generation pipeline built entirely on Unreal Engine 5.4. Researchers provide a textured, rigged 3D model of their study animal — often created with our open-source 3D scanning platform, scAnt — and the pipeline:

- Places models in procedurally generated scenes with complex ground topology, scattered 3D assets, layered materials, decals, and randomised lighting
- Renders multi-pass image data: RGB, depth, instance segmentation, and surface normals
- Produces automatic annotations compatible with YOLO, DeepLabCut, SLEAP, MMDetection, and COCO formats
- Supports configurable randomisation of every scene element to maximise training data diversity

The latest version (March 2026) adds native support for parametric mesh models through a custom C++ Blueprint Function Library (SMILTools), enabling PCA-driven morphological variation during data generation. This means a single parametric model can produce training images spanning the full range of body proportions observed across dozens of species — critical for building robust, generalisable computer vision models.

### The Open-Source Ecosystem

replicAnt is the centrepiece of an interconnected suite of open-source tools we have built for 3D animal data:

**scAnt** (239 GitHub stars, 45 forks) — An open-source, low-cost 3D scanning platform for creating high-fidelity digital models of arthropods and other small animals. scAnt provides the 3D models that feed into replicAnt. Published in *PeerJ* (2021).

**OmniTrax** (39 stars) — A Blender Add-on for deep learning-driven multi-animal tracking and pose estimation. Detection and pose models trained on replicAnt synthetic data are deployed in OmniTrax for real-world experiments. Published in *JOSS* (2024).

**SMILify** — A framework extending parametric body modelling (analogous to SMPL for humans) to arbitrary animal body plans. SMILify learns species-specific shape spaces from 3D scan collections (via scAnt), replicAnt generates the synthetic training data for SMILify's neural inference networks, and SMILify reconstructs dense 3D pose and shape from video. Currently demonstrated in ants (80 species), stick insects (3 species), and mice.

Together, these tools form a complete pipeline: **physical specimen → 3D scan → parametric model → synthetic training data → trained neural network → 3D behavioural analysis**.

### Publications and Track Record

The ecosystem has produced peer-reviewed publications in leading journals:

- **Plum, Bulla, Beck, Imirzian & Labonte (2023).** replicAnt: a pipeline for generating annotated images of animals in complex environments using Unreal Engine. *Nature Communications*, 14. — Demonstrates that models trained on replicAnt synthetic data match or exceed the performance of models trained on manually annotated real data, across detection, tracking, pose estimation, and segmentation.
- **Plum & Labonte (2021).** scAnt — an open-source platform for the creation of 3D models of arthropods. *PeerJ*, 9:e11155.
- **Plum (2024).** OmniTrax. *Journal of Open Source Software*.

The scientific impact of this work has been recognised through competitive grants and awards:

- **BBSRC AI for Bioscience Grant** (2023, €330,000) — Awarded for "Synthetic data for robust and versatile animal pose estimation in 2D and 3D," directly funding the development of the SMILify pipeline that depends on replicAnt for synthetic training data. In collaboration with David Labonte and Talmo Pereira.
- **ERC Proof-of-Concept Grant** (2022, €150,000) — Awarded to develop the scAnt scanning platform into a commercially viable solution for high-resolution digitisation of small objects. scAnt provides the 3D models that feed directly into replicAnt.
- **UKRI Impact Acceleration Funding** (£70,000) — Supporting the commercialisation of scAnt into an accessible, low-cost scanning solution for automated digitisation.
- **1st Prize, Amazon PhD Prizes for Outstanding Achievement in Robotics** (2021) — Awarded for developing an open-source macro 3D scanner and its use in synthetic data-driven machine learning applications — the foundational work that led to replicAnt.
- **1st Prize, Open Electronics Session, Society for Experimental Biology** (2021) — Recognising the open-source hardware and software contributions of the scAnt platform.

These awards — totalling over €550,000 in research funding — validate the scientific significance and practical impact of the ecosystem. Critically, the BBSRC AI for Bioscience grant explicitly funds work that relies on replicAnt as its synthetic data backbone, demonstrating that the Unreal Engine-based pipeline is already recognised as essential infrastructure by major research funders.

### Why Unreal Engine

Unreal Engine is not a convenience choice — it is the enabling technology. replicAnt depends on UE for:

- **Photorealistic rendering** — Lumen global illumination, hardware ray tracing, path tracing, and the Substrate material system produce training images realistic enough to transfer directly to real-world applications
- **Procedural generation** — Blueprint-driven scene construction with configurable randomisation ensures training data diversity without manual scene design
- **Multi-pass rendering** — simultaneous output of RGB, depth, segmentation, and normal passes from a single render, with automatic annotation
- **Extensibility** — the C++ / Blueprint architecture allows us to integrate novel features (like PCA-driven parametric models) as native engine functionality
- **Performance** — GPU-accelerated rendering enables generation of tens of thousands of annotated images in hours rather than weeks
- **Cross-platform potential** — Linux support enables deployment on HPC clusters for large-scale dataset generation

No other engine or rendering framework offers this combination of photorealism, procedural flexibility, native C++ extensibility, and multi-pass output in a single, well-supported package.

### Discriminative ML, Not Generative AI

It is important to emphasise that replicAnt is a tool for *discriminative* machine learning — training networks to detect, track, and reconstruct animals from images — not generative AI. The goal is robust, reproducible, quantitative outcomes for scientific research: accurate pose estimates, reliable tracking, faithful 3D reconstructions. Synthetic data from replicAnt is not an end in itself; it enables the training of models that produce measurable, verifiable results in real experiments. This distinction matters: scientific applications demand reproducibility and quantitative accuracy, not plausible-looking outputs.

---

## UNIQUE FEATURES

1. **Species-agnostic by design** — replicAnt works with any rigged 3D model, from ants to mice to arachnids. New body plans can be supported by creating new SubjectBase archetypes (InsectBase, QuadrupedBase, ArachBase), making the system extensible to virtually any animal.

2. **Parametric morphological variation** — The integrated SMIL system generates anatomically plausible body shape variation from PCA-learned shape spaces, producing training data that spans the morphological diversity of entire taxa from a single parametric model.

3. **End-to-end open-source pipeline** — From 3D scanning (scAnt) through synthetic data generation (replicAnt) to behavioural analysis (OmniTrax, SMILify), the entire workflow is open-source and freely available. No commercial dependencies beyond Unreal Engine itself.

4. **Validated against real data** — Published results in *Nature Communications* demonstrate that synthetic-data-trained models match manually-annotated-real-data-trained models across multiple CV tasks. This is not a proof of concept — it is a validated methodology.

5. **Multi-pass automatic annotation** — Every rendered frame simultaneously produces RGB, depth, instance segmentation, and normal maps with automatic annotations in multiple standard formats (YOLO, COCO, DLC, SLEAP, mmsegmentation). Zero manual labelling required.

6. **Fur and groom simulation** — The Mouse model demonstrates integration with UE5's HairStrands/Groom system for realistic fur rendering, extending the pipeline beyond rigid-body arthropods to mammals.

---

## FUNDING

| Field | Value |
|-------|-------|
| Funding range | $25,000–$50,000 |
| Additional funding secured? | Yes |
| Additional funding details | The broader ecosystem is supported by over €550,000 in competitively awarded research funding: a BBSRC AI for Bioscience grant (€330,000) funding synthetic data-driven animal pose estimation via SMILify and OmniTrax; an ERC Proof-of-Concept grant (€150,000) for the scAnt 3D scanning platform; and UKRI Impact Acceleration Funding (£70,000) for scAnt commercialisation. This MegaGrant would be the first dedicated funding for replicAnt-specific Unreal Engine development. |

### Budget Breakdown

The primary use of funds is to support **one year of dedicated development time** by a core contributor (René Bulla) who has built significant functionality of the project on a volunteer basis and who we aim to support through at least one year of full-time development. Specifically:

- **Developer salary / contract** (~80% of funds) — Full-time Unreal Engine development work:
  - Refactor the current prototype to reduce technical debt accumulated over multiple engine versions and feature additions
  - Rebuild the application architecture into a clean, well-documented, and extensible codebase
  - Streamline the SubjectBase system to make adding new body plans straightforward for non-expert users
  - Create editable procedural world generation templates for common research scenarios (laboratory environments, field settings)
  - Improve documentation and onboarding for new users

- **Infrastructure and testing** (~10% of funds) — Cloud GPU resources for large-scale dataset generation and validation, CI/CD setup

- **Community and dissemination** (~10% of funds) — Tutorial creation, workshop materials, conference presentations to grow the user community

### What This Funding Enables

We are directly collaborating with three laboratories across Germany, the United Kingdom, and the United States on the continued development of replicAnt. These groups work on crop protection, locomotion biomechanics, and behavioural ecology — each requiring bespoke synthetic data for species and scenarios where real training data is scarce or nonexistent.

The current codebase, while functional and validated in publication, carries significant technical debt from its long development timeline, engine migration (5.0 → 5.4), and organic feature growth. Grant funding would allow us to:

1. **Professionalise the codebase** — Transform the research prototype into a robust, maintainable tool that external contributors can extend
2. **Lower the barrier to entry** — Provide templates, documentation, and streamlined workflows so researchers without UE expertise can generate synthetic data for their species
3. **Expand body plan support** — The SubjectBase system currently directly supports insects (6-legged), quadrupeds (4-legged), and arachnids (8-legged); we aim to generalise this further
4. **Enable industry applications** — Partners working on crop protection and pest monitoring need production-quality synthetic data generation; cleaning up the pipeline enables these concrete applications
5. **Sustain open-source development** — The core developer has contributed thousands of hours as a volunteer; this grant would provide the first dedicated funding for replicAnt-specific UE development

---

## TO-DO BEFORE SUBMISSION

- [ ] Update scAnt3D company website
  - [ ] Finish landing page
  - [ ] Add synthetic data generation as a service
  - [ ] Add list of scAnt users (company/institution logos)
  - [ ] Add open tech page
  - [ ] Add full spec page of scAnt Pro with "request quote"
- [ ] Update replicAnt GitHub page (basic clean-up)
- [ ] Prepare project media link (demo video / trailer)
- [ ] Finalise company/institution name
- [ ] Confirm exact funding amount requested
- [ ] Review and polish elevator pitch (currently 224 chars, max 225)
- [ ] Add project build link (optional)
