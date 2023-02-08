# FARTS
**F**abi **A**nd **R**ené's **T**raining-data **S**ynthesizer

![](documentation_images/06_launch.png)

Generating synthetic datasets to improve inference on all sorts of computer-vision tasks that specifically involve insects. Like, lots of them.

## Installing Unreal Engine
### Minimum system requirements:

*	Windows 10 (other operating systems may work but are untested)
* ~50 GB of disk space (the faster the better)
  * Unreal engine itself will occupy roughly 30 GB
  * Another ~5GB for the complete project including 3D assets and materials
  *	~5 GB per 10k sample dataset at 2k resolution, using all available passes
*	Dedicated GPU with 6GB VRAM (currently, only tested on NVIDIA GPUs)
*	16 GB RAM

### Python Environment (for parser notebooks)

```bash
cd conda_environment
conda env create -f conda_FARTS.yml
```

After the environment has been created successfully, re-start the terminal, and run the following line to activate the environment, and to continue the installation.

 ```bash
conda activate FARTS
```

If you do not wish to install the pre-configured environment, here are the dependencies:

  - python >= 3.7
  - pip
  - numpy
  - matplotlib
  - opencv
  - json5
  - pandas
  - pathlib
  - imutils
  - scikit-learn

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

3. Launch **FARTS.uproject** by double clicking on the file. When opening the project for the first time, it may take up to 30 minutes to compile all shaders. If this is the case you can use the spare time to install [Blender](https://www.blender.org/) and the latest version of [Send2Unreal](https://github.com/EpicGames/BlenderTools/releases). 

![](documentation_images/06_launch.png)

4. In the content browser, right click on the file named **FARTS_Interface** and select **Run Editor Utility Widget**.

![](documentation_images/07_add_FARTS_interface.PNG)

Now, you should be able to see the FARTS interface on the left side of your screen, where you can configure everything part of the generator, from file types and simulated colonies, to adding further animals, and controlling the generator seed for benchmarking and debugging purposes.

![](documentation_images/08_show_FARTS_interface.PNG)

In theory, you can now start generating your first datasets right away (if you are planning on only using the provided insect models). In case you want to bring your own insect models into the generator follow the "3D model to dataset" guide below.

## 3D model to dataset (Blender to Unreal)

### Requirements:
* [Blender](https://www.blender.org/) (v3.0.1 or later)
* [send2unreal](https://github.com/EpicGames/BlenderTools) (v2.1 or later)
*	Unreal engine (installed via Epic Games Launcher, see above, v5.0.2 or later)
*	*Patience.*

1. download [send2unreal](https://github.com/EpicGames/BlenderTools) and don't unzip the file. You can install the addon in its zipped form straight from Blender. In Blender, under **edit/preferences/Add-ons** click on **Install...** and navigate to the **send2ue** zip file on your computer.

![](documentation_images/10_install_Blender_plugin.PNG)

2. You should now see a new top at the top of your Blender Window, named **Pipeline**. Click on it, and then select **Export/Settings_Dialog** to configure how we want things to be sent over to Unreal.

![](documentation_images/11_config_send2ue.PNG)

3. Inside the **Paths** tab, enter the desired location of your mesh / textures / armatures inside the Unreal project. In our case, this will be **Game/Subjects/NAME_OF_YOUR_SPECIMEN**. You will have to manually create the **NAME_OF_YOUR_SPECIMEN** folder inside the **Subjects** folder of the Unreal project, but this will make it possible to make changes to our model in Blender and simply send the updated version over to Unreal without having to set up everything again.

![](documentation_images/12_config_path.PNG)

4. Double check the export settings, specifically the **.FBX** settings. We want **Apply Unit** to be checked, the Z axis to be up, and NO leaf bones to be added.

![](documentation_images/13_config_export.PNG)

5. If everything went according to plan, you should also be able to see an additional folder in your **Scene Collection** titled **Export**. Anything we place in here, Unreal will attempt to import, when we are ready to click on **Send to Unreal** (But don't do that now. We still have a few things to check first.)

![](documentation_images/14_send2ue_collection.PNG)

6. If you haven't done that already, launch **FARTS.uproject** by double clicking on the file. We need to check whether Unreal has the right plugins enabled to receive data from our Blender instance. Click on **Settings** (top left) and then on **Plugins**.

![](documentation_images/15_ue_plugins.PNG)

7. Check, whether the **Editor Scripting Utilities** and **Python Ediotor Script** plugins are activated. If they are not, enable them now.

![](documentation_images/16_ue_plugins_script.PNG)

Nice one! Installation-wise we are done! 
Time to prepare a model in Blender to send over and configure for generating some data!

## Preparing models in Blender for Unreal

Generally, the lower the vertex count, while still appearing realistic, the better. Unreal engine deals very well with reidiculous numbers of polygons, but simulating hundreds of high resolution insects can still be a challenging task for your machine. Aim for >20,000 triangles inside your model, and you are good. I'll update this page with a full retopology guide soon, but for now, just employ the techniques of your trade to get your model to roughly fit this suggestion. Larger models will work, but I hope you have a strong machine that can deal with them.

Further notes will also follow on the standard rigging convention used here, but in theory, any model and rig combination is possible. Make sure, your model is posed with all tarsi in ground contact in a somewhat natural pose. This is important, as we are going to use IK solvers to have the appendages interact with elements in the generated environment.

Let's assume your model is already **retopologised**, **rigged**, and **ready**!

1. **Apply** your **armature** in the **modifier** tab, and **delete all vertex groups**. We need to do this once, so all units and joint orientations will scale correctly.

![](documentation_images/17_apply_armature.PNG)

2. Apply all transforms (scale, rotation, position) to your mesh. In Object mode, select the mesh, hit "ctrl + a" and select All Transforms. Repeat this step for the armature. Without applying transforms, Unreal and Blender will throw all sorts of fun error messages at you. Nobody wants that.

![](documentation_images/18_apply_transforms.PNG)

3. Now select the **armature** and go into **pose mode**. Click **Pose > Apply > Apply Pose** as Rest Pose, to make the current pose the default.

![](documentation_images/19_rest_pose.PNG)

4. Parent the mesh back to the armature with automatic weights. Double check the influence of each joint via **weight painting**. If joints affect unwated areas, this will lead to unrealistic generated poses.

*	[Optional] Create Normal maps. This can be done either in Blender directly by baking a normal map as an additional image texture (see texture baking), in Photoshop, or, nowadays it seems, in literally any image editing program… However, it’s best to do it directly in Blender to preserve information between unwrapped regions, which avoids sharp artifacts between islands in the unwrapped texture.

5. Finally, in the **Scene Collection** move both your armature and your mesh into the **Export** collection. Now you can click on **Pipeline > Export > Send to Unreal**. Remember, your Unreal project needs to be open in order for this to work, and you must have created the specified folder referenced in the export settings (see step 3 of the installation guide above)

![](documentation_images/20_send2ue_export.PNG)

… and hope for the best.

## Including new models in the generator (Unreal)

1. Check your transfered models in the Content browser to ensure they made it over to the Unreal Project.

![](documentation_images/21_models_in_ue.PNG)

2. Create a material instance from **Subjects/M_Subject01_SSInsects** and drag it into your model folder.

![](documentation_images/22_ue_material_instance.PNG)

3. Open the material instance and replace the existing textures (albedo & optionally normal) with your own. (In case these materials are not editable, instead of making a **material instance** of **Subjects/M_Subject01_SSInsects**, make a **copy** of it and try again.)

![](documentation_images/23_ue_material_setup.PNG)

4. Assign the **SK_InsectBase_Skeleton** to your skeletal mesh by right clicking it and selecting **Skeleton > Assign Skeleton**.

![](documentation_images/24_assign_skeleton_A.PNG)

![](documentation_images/24_assign_skeleton_B.PNG)

5. Compute the low-poly collision mesh from the **Physics Asset** of your imported mesh. Open the **Physics Asset** and delete any hierarchy that is already present. Then, under Tools (right side) match the parameters shown in the image below and click on **Genrate All Bodies**.

![](documentation_images/25_update_collision.PNG)

6. Create a **Child Blueprint Class** from **SubjectBase/BP_sbj_InsectBase** and also move it into your mesh folder.

![](documentation_images/26_the_child.PNG)

7. Rename the **Child Blue Print Class** to something more fitting of the newly added mesh so we can find it when we want to select it inside the generator. Then open the file. In there, select the **Insect_SkeletalMesh** on the left side and then make the following changes on the right side:

* for the **Anim Class**, select **ABP_InsectBase_C**
* for the **Skeletal Mesh**, select your newly imported skeletal mesh
* for the **Materials > Element 0**, select the created **Material Instance** (not the original material, as otherwise the generator cannot apply variations to it at runtime)

![](documentation_images/27_the_blueprint.PNG)

8. Select your new mewly created **Child Blue Print Class** as part of your **Colony** in the **FARTS_Interface**, under **Colony > Types**. You can also add additional subjects here, the sky is the limit. By pressing on **Spawn Preview Colony** you can see what your generated colony will look like. (You may need to move to the top of the generated world, where the **Colony Preview Groundplane** is located.

![](documentation_images/28_the_colony.PNG)

9. **You did it!**  (well, hopefully) 
If everything has gone according to plan, you should now be able to hit **Generate** and watch the magic happen. In a follow-up section, I will detail what each of the generator parameters does. But for: **Happy Generatin'!**

![](documentation_images/29_examples.PNG)


# NOTES
## External files

Additional assets need to be dowloaded and placed into the content folder, which are hosted externally under the following link:
[Google Drive](https://drive.google.com/file/d/1FiboPJmrhqv6cDB2Ara-2n3-yDdHg0sh/view?usp=sharing)

## Generator TODOs

(Hosted externally in HacknPlan for now)
