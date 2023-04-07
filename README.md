# replicAnt

"a synthetic data-driven approach to animal behavioural research"

by [Fabian **Plum**](https://twitter.com/fabian_plum), 
[René **Bulla**](https://twitter.com/renebulla), 
[Hendrik **Beck**](https://twitter.com/Hendrik_Beck), 
[Natalie **Imirzian**](https://twitter.com/nimirzy), 
and [David **Labonte**](https://twitter.com/EvoBiomech) (2023)

___

![](images/06_launch_new.png)

***replicAnt*** - aka  ***F**abi **A**nd **R**ené's **T**raining-data **S**ynthesizer*

***
<img src=docs/figures/Fig_1_dark.png#gh-dark-mode-only >
<img src=docs/figures/Fig_1_bright.png#gh-light-mode-only > 

**replicAnt** produces procedurally generated and richly annotated image samples from 3D models of arthropod specimens, 
which form the basis of a wide range of computer vision applications. (**a**) The input to the replicAnt pipeline are 
digital 3D models, which we generated with the [open-source photogrammetry platform - scAnt](https://github.com/evo-biomech/scAnt). 
Each model comprises a (**b**) textured mesh, (**c**) an armature which provides control over animal pose, and 
(**d**) a low-polygonal collision mesh to control the interaction of the model with objects in its environment. 
(**e**) 3D models are placed within scenes procedurally generated with the free software 
[Unreal Engine 5](https://www.unrealengine.com/en-US/unreal-engine-5). (**f**) Every scene consists of the same core elements, 
each with configurable randomisation routines to maximise variability in the generated data. 3D assets are scattered 
on a ground topology; layered materials, decals, and light sources provide significant variability for the generated 
scenes. From each scene, we generate  (**g**) image, (**h**) ID, (**i**) depth, and normal 
passes, accompanied by (**j**) an annotation data text file which contains rich information on image contents. 
The combination of image passes, and data files constitute synthetic datasets that can be used to train networks 
specialised for a large variety of deep learning-based computer vision applications, including (k) 
[detection](UE5_parsers/Generate_YOLO_Style_Dataset_JSON.ipynb), 
(**l**) [tracking](https://github.com/FabianPlum/OmniTrax), 
(**m**) [2D](UE5_parsers/Generate_DLC_Style_Dataset.ipynb) 
and [3D](UE5_parsers/Generate_Custom_3D_Dataset.ipynb) pose estimation, 
(**n**) and [semantic segmentation](UE5_parsers/Generate_mmlab_segmentation_Style_Dataset.ipynb).

***

## Installation Guide
### Minimum system requirements:

*	Windows 10 (other operating systems may work but are untested)
* ~ 50 GB of disk space (the faster the better)
  * Unreal engine itself will occupy roughly 30 GB
  * Another ~5GB for the complete project including 3D assets and materials
  *	~5 GB per 10k sample dataset at 2k resolution (when using all available passes)
*	Dedicated GPU with 6GB VRAM (currently, only tested on NVIDIA GPUs)
*	16 GB RAM

### Python Environment (for parser notebooks)

To convert the generated datasets into formats accepted by common computer vision frameworks,
we have provided a number of [parsers](UE5_parsers) as interactive Jupyter notebooks.

You require a python installation on your system to make use of them. For ease of use, we
provide an example conda environment, as well as a list of dependencies, in case you want to
use a custom python installation / environment:

**Install dependencies via conda**

```bash
cd conda_environment
conda env create -f conda_replicAnt.yml
```

After the environment has been created successfully, re-start the terminal, and run the following line to activate the environment, and to continue the installation.

 ```bash
conda activate replicAnt
```

If you do not wish to install the pre-configured environment, here are the dependencies:

  - python >= 3.7
  - pip
  - notebook
  - numpy
  - matplotlib
  - opencv
  - json5
  - pandas
  - pathlib
  - imutils
  - scikit-learn

### Unreal Engine Installation
1.	You will first need to create an [Epic games account](https://www.epicgames.com/site/login) which we will later link to your Github profile. This grants you access to the Unreal Engine source code, including Blender Plugins (i.e., Send2Unreal), and as a bonus, grant access to Quixel’s asset library to add additional meshes and materials to your generator environment:

[Epic Games account creation](https://www.epicgames.com/site/login)

2.	Download and install the Epic Games Launcher. From there, you can manage your (to be) installed Unreal Engine environments and update them:

[Epic Games Launcher](https://www.epicgames.com/site/en-US/home)

![](images/00_epic_unreal.PNG)

3.	Open the installed **Epic Games Launcher** and click on **Unreal Engine** on the left side of the window.

![](images/01_epic_unreal.PNG)

Now, click on **Library** and click on the **+** icon to install a new version of Unreal Engine. As we built replicAnt on Unreal Engine 5, select the latest Unreal Engine 5 release, and follow the installation guide. 

Unless you are planning on running extensive debugging or further development, only selecting **core components** should be sufficient. All additionally required functionality is provided in our project environment and, alternatively, can be installed later on.

![](images/02_epic_unreal.PNG)

4.	While your computer is busy installing Unreal Engine, connect your GitHub account to your newly created Epic Games account. For a more thorough guide, refer to the (official documentation](https://www.unrealengine.com/en-US/ue-on-github)

In short, head over to your [Epic Games account]( https://www.unrealengine.com/account/connections), and under **Connections**, connect to your GitHub profile. Simply follow the instructions prompted in your browser and authorize Epic Games. You will then receive a confirmation email to join the Epic Games organisation on GitHub to access all source code and plugins.

![](images/03_link_account.PNG)

If all has gone well, you should be able to see on your GitHub profile that you have successfully joined Epic Games!

![](images/04_link_account.PNG)

5.	Once the **Unreal Engine** installation has completed in the background, restart your computer. Afterwards, we’ll set up the project.

### Setting up the replicAnt project
1.	If you have not done so already, clone the replicAnt repository to your computer. The total project size will be ~5GB, including 3D assets and materials to populate the procedurally generated world.

```bash
git clone https://github.com/FabianPlum/replicAnt
``` 

This will take a while. There are a lot of files to be copied.

2.	Once the download has finished, you will next need to download all additional assets (3D meshes and materials) we have provided externally. NOTE: This project is running under a non-commercial license and any assets used for the generation of synthetic datasets may not be used or re-distributed for commercial purposes.

[Download replicAnt external content files](https://drive.google.com/file/d/1h6p040Gy7vvwY12C7zOBdEdjgBTOihhO/view?usp=share_link)

Download and unpack the files into the **Content** directory of the **replicAnt** project.

![](images/05_external_files.png)

3. Launch **replicAnt.uproject** by double-clicking on the file. When opening the project for the first time, it may take up to 30 minutes to compile all shaders. If this is the case you can use the spare time to install [Blender](https://www.blender.org/) and the latest version of [Send2Unreal](https://github.com/EpicGames/BlenderTools/releases). 

![](images/06_launch.png)

4. In the content browser, right-click on the file named **replicAnt_Interface** and select **Run Editor Utility Widget**.

![](images/07_add_replicAnt_interface.PNG)

Now, you should be able to see the replicAnt interface on the left side of your screen, where you can configure everything part of the generator, from file types and simulated colonies, to adding further animals, and controlling the generator seed for benchmarking and debugging purposes.

![](images/08_show_replicAnt_interface.PNG)

In theory, you can now start [generating your first datasets](docs/04_Generating_your_first_dataset.md) right away 
(if you are planning on only using the provided subject models). 

In case you want to bring your own (insect) models into the generator follow our detailed guides:

> * [**01** Retopologising 3D models](docs/01_Retopologising_3D_models.md)
> * [**02** Rigging 3D models](docs/02_Rigging_3D_models.md)
> * [**03** Bringing 3D models into Unreal](docs/03_Bringing_3D_models_into_Unreal_guide.md)
> * [**04** Generating your first dataset](docs/04_Generating_your_first_dataset.md)
> * [**05** Adding custom assets and scatterers](docs/05_Adding_custom_assets_and_scatterers.md)

***More guides on advanced usage and customisation will follow soon!***

## Notes and Resources

### External files

* Additional assets need to be downloaded and placed into the content folder, which are hosted externally under the following link:
[Google Drive](https://drive.google.com/file/d/1FiboPJmrhqv6cDB2Ara-2n3-yDdHg0sh/view?usp=sharing)
* We regularly update our library of pre-configured subject models, which can be [downloaded here](https://drive.google.com/drive/u/0/folders/1l9g7tlFZ3HWIA1z8x-My-LKYPrqz77aq).
The following subject models are currently available:
  * **Leafcutter ants** - _Atta vollenweideri_ (various worker sizes, ranging from 1.1 mg to 50.1 mg)
  * **Desert termites** - _Gnathamitermes_ (worker and soldier)
  * **Praying mantis** - _Gongylus gongylodes_
  * **Stick insects** - _Sungaya inexpectata_ (various instars)
  * **Stick insects** - _Peruphasma schultei_ (male and female)
  * **Leaf-footed bug** - _Leptoglossus zonatus_ (adult)
  * **Desert ants** - _Pogonomyrmex desertorum_ (worker)

### Troubleshooting

The **replicAnt** pipeline comprises many moving parts and experimental software tools, so it is only natural for
new additions to the pipeline (or updates on the side of Unreal and Blender) to cause minor hiccups.
Therefore, we have compiled a list of common issues and fow to fix them:

[**Troubleshooting guide**](docs/troubleshooting.md)

In case the issue you are encountering is not listed in the [Troubleshooting guide](docs/troubleshooting.md), feel free
to open an issue, using to our [**bug report template**](.github/ISSUE_TEMPLATE/bug_report.md).

### References

When using **replicAnt** and/or our other fun projects in your work, 
please cite some of the following:

    @article{PlumLabonte2021,
        title = {scAnt — An open-source platform for the creation of 3D models of arthropods (and other small objects)},
        author = {Plum, Fabian and Labonte, David},
        doi = {10.7717/peerj.11155},
        issn = {21678359},
        journal = {PeerJ},
        keywords = {3D,Digitisation,Macro imaging,Morphometry,Photogrammetry,Zoology},
        volume = {9},
        year = {2021}
        }
    
    @misc{Plum2022,
        title = {OmniTrax},
        author = {Plum, Fabian},
        resource = {GitHub repository},
        howpublished = {https://github.com/FabianPlum/OmniTrax},
        year = {2022}
        }

    @misc{Plumetal2023,
        title = {replicAnt},
        author = {Plum, Fabian; Bulla, Rene; Beck, Hendrik; Imirzian, Natalie; David, Labonte},
        resource = {GitHub repository},
        howpublished = {https://github.com/FabianPlum/OmniTrax},
        year = {2023}
        }

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
© Fabian Plum, Rene Bulla, David Labonte 2023
[MIT License](https://choosealicense.com/licenses/mit/)
