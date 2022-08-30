# FARTS
Fabi And René's Training-data Synthesizer

![](documentation_images/06_launch.png)

Generating synthetic datasets to improve inference on all sorts of computer-vision tasks that specifically involve insects. Like, lots of them.

## Installing Unreal Engine
### Minimum system requirements:
•	**Windows 10** (other operating systems may work but are untested)
•	>50 GB of disk space (the faster the better)
o	Unreal engine itself will occupy roughly 30 GB
o	Another ~5GB for the complete project including 3D assets and materials
o	>5 GB per 10k sample dataset at 2k resolution, using all available passes
•	Dedicated GPU with 6GB VRAM (currently, only tested on NVIDIA GPUs)
•	16 GB RAM

### Unreal Installation Guide
1.	You will first need to create an [Epic games account](https://www.epicgames.com/site/login) which we will later link to your Github profile. This grants you access to the Unreal Engine source code, including Blender Plugins (i.e., Send2Unreal), and as a bonus, grant access to Quixel’s asset library to add additional meshes and materials to your generator environment:

[Epic Games account creation](https://www.epicgames.com/site/login)

2.	Download and install the Epic Games Launcher. From there, you can manage your (to be) installed Unreal Engine environments and update them:

[Epic Games Launcher](https://www.epicgames.com/site/en-US/home)

![](documentation_images/00_epic_unreal.PNG)

3.	Open the installed **Epic Games Launcher** and click on **Unreal Engine** on the left side of the window.

![](documentation_images/01_epic_unreal.PNG)

Now, click on **Library** and click on the **+** icon to install a new version of Unreal Engine. As we built FARTS on Unreal Engine 5, select the latest Unreal Engine 5 release, and follow the installation guide. 

Unless you are planning on running extensive debugging or further development, only selecting **core components** should be sufficient. All additionally required functionality is provided in our project environment and, alternatively, can be installed later on.

![](documentation_images/02_epic_unreal.PNG)

4.	While your computer is busy installing Unreal Engine, connect your GitHub account to your newly created Epic Games account. For a more thorough guide, refer to the (official documentation](https://www.unrealengine.com/en-US/ue-on-github)

In short, head over to your [Epic Games account]( https://www.unrealengine.com/account/connections), and under **Connections**, connect to your GitHub profile. Simply follow the instructions prompted in your browser and authorize Epic Games. You will then receive a confirmation email to join the Epic Games organisation on GitHub to access all source code and plugins.

![](documentation_images/03_link_account.PNG)

If all has gone well, you should be able to see on your GitHub profile that you have successfully joined Epic Games!

![](documentation_images/04_link_account.PNG)

5.	Once the **Unreal Engine** installation has completed in the background, restart your computer. Afterwards, we’ll set up the project.

###Setting up the FARTS project
1.	If you have not done so already, clone the FARTS repository to your computer. The total project size will be ~5GB, including 3D assets and materials to populate the procedurally generated world.

```bash
git clone https://github.com/FabianPlum/FARTS
``` 

This will take a while. There are a lot of files to be copied.

2.	Once the download has finished, you will next need to download all additional assets (3D meshes and materials) we have provided externally. NOTE: This project is running under a non-commercial license and any assets used for the generation of synthetic datasets may not be used or re-distributed for commercial purposes.

[Download FARTS external content files]( https://drive.google.com/file/d/1FiboPJmrhqv6cDB2Ara-2n3-yDdHg0sh/view?usp=sharing)

Download and unpack the files into the **Content** directory of the FARTS project.

![](documentation_images/05_external_files.png)


# NOTES
## External files

Additional assets need to be dowloaded and placed into the content folder, which are hosted externally under the following link:
[Google Drive](https://drive.google.com/file/d/1FiboPJmrhqv6cDB2Ara-2n3-yDdHg0sh/view?usp=sharing)

## Generator TODOs

(Hosted externally in HacknPlan for now)

## Parser TODOs

* Update all parsers to use new **json** data format
  * YOLO
    * DONE
  * DLC
    * DONE
  * HERO DLC
    * DONE
  * COCO
    * DONE
  * Custom 3D
    * read in new json format files
    * check new 3D camera intrinsics and extrinsics and ensure the reprojected 3D coordinates align with the original 2D coordinates

* Generate new example datasets
  * Single
  * Multi
