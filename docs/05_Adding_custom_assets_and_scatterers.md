# replicAnt

"a synthetic data-driven deep learning approach to animal behavioural research"

by [Fabian **Plum**](https://twitter.com/fabian_plum), 
[René **Bulla**](https://twitter.com/renebulla), 
[Hendrik **Beck**](https://twitter.com/Hendrik_Beck), 
[Natalie **Imirzian**](https://twitter.com/nimirzy), 
and [David **Labonte**](https://twitter.com/EvoBiomech) (2023)

___

![](../images/06_launch_new.png)

___

# Adding custom assets and scatterers

## Requirements:
* Unreal engine (installed via Epic Games Launcher, see above, v5.0.2 or later)
* Completed [generator setup](04_Generating_your_first_dataset.md)

In principle, any 3D asset in a compatible format can be used as part of the asset scatterers to populate the
generated environments. Here, we are going to show three possible routes of bringing additional 3D assets into
the generator.

* (A) **Direct import**
* (B) **Blender (send2unreal)**
* (C) **Quixel Bridge**

Below we outline the procedure for importing new 3D assets as well as how to set up new **asset scatterers**.

___

### (A) Direct import

The most straight forward way to bring your own 3D assets into Unreal is via direct import. In the **Content Browser**, 
simply **right-click** in the folder where you want to load your asset, and select **Import to...**

![](../images/asset_scatterers_01.PNG)

Then select your asset from your computer and **import** it.

![](../images/asset_scatterers_02.PNG)

___

### (B) Blender (send2unreal)

Similar to the process outlined in [03_Bringing_3D_models_into_Unreal](03_Bringing_3D_models_into_Unreal_guide.md), you
can move any mesh from **Blender** to Unreal, in this case with less optimisation required, compared to the workflow
detailed for randomisable subjects.

In **Blender**, when configuring the **Send to Unreal** plugin, instead of the **Subject** directory of the **Unreal** 
project, we import it to **Megascans/ExtraMeshes**. 
This setup is purely for the sake of clarity, and you can freely choose where to store your assets.

![](../images/asset_scatterers_03.PNG)

Ensure all meshes you wish to transfer are placed in the **Export** folder in the **Outliner**.
Apply all transforms.

![](../images/asset_scatterers_04.PNG)

Then, transfer the assets to Unreal by clicking on **Pipeline/Export/SendToUnreal**.

> **NOTE** : For additional details refer to 
> [03_Bringing_3D_models_into_Unreal](03_Bringing_3D_models_into_Unreal_guide.md)

___

### (C) Quixel Bridge

If you are looking to populate your scenes with readily available assets from the 
[Quixel Megascans](https://quixel.com/megascans/home) library (free to use for
Unreal Engine Projects), you can add assets to your **Content Browser** directly via the **Quixel Bridge** plugin.

In case the plugin is not yet enabled / installed, close the **Unreal Engine Editor** and open the **Epic Games 
Launcher**. In there, head to your installation of **Unreal Engine**.

![](../images/asset_scatterers_quixel_bridge_01.PNG)

Then scroll down to the section labeled **VAULT** and type **Bridge** into the search bar.

![](../images/asset_scatterers_quixel_bridge_02.PNG)

Install the **Quixel Bridge** Plugin for your version of **Unreal Engine**.

![](../images/asset_scatterers_quixel_bridge_03.PNG)

Then open the **replicAnt.uproject** file again.

Now, you can import assets directly from the **Quixel Megascans** library, either via the top bar...

![](../images/asset_scatterers_quixel_bridge_04.PNG)

...or within the **Content Browser** (by right-clicking in any of the directories.)

![](../images/asset_scatterers_quixel_bridge_04_b.PNG)

___

## Setting up Asset Scatterers

This process is very straight forward. You can either simply make a copy of one of the **asset scatterers** currently
listed in the **Outliner**, or add a new **asset scatterer** to your scene from the **Content Browser**.

If you decide to add a new asset scatterer, search for the word **scatter** and drag-and-drop the **Blueprint Class**
titled ```BP_Scatter``` into the viewport.

![](../images/asset_scatterers_05.PNG)

The new asset scatterer should now appear in the **Outliner**.
Make sure, to set the **Location** to the world origin (0, 0, 0).

![](../images/asset_scatterers_06.PNG)

With the newly added ```BP_Scatter``` selected in the **Outliner**, scroll down in the **Details** tab below to
configure the scatterer. 

Add all meshes you wish the scatterer to spawn by (**+**) adding to the **Array** of **Scatter Meshes**.

* ```Scatter Instances Min / Max``` - Range of elements that are spawned by the **asset scatterer**. One randomly drawn 
element from the **Array** of **Scatter Meshes** will be spawned.
* ```Min / Max ZOffset``` - Range of units below / above the ground plane in which assets can be spawned.

![](../images/asset_scatterers_07.PNG)

As the effect of the scale-related parameters will depend on your selected assets, click on ```Scatter in Editor``` to
preview the appearance of the scattered meshes in the environment.

![](../images/asset_scatterers_08.PNG)

___

**Happy Generating!**

___

> In case you encounter any problems, consult our [troubleshooting guide](troubleshooting.md), or consider raising an
> **issue** on the replicAnt GitHub page.
 
## License
© Fabian Plum, Rene Bulla, David Labonte 2023
[MIT License](https://choosealicense.com/licenses/mit/)


