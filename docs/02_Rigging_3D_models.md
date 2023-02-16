# FARTS

**F**abi **A**nd **R**ené's **T**raining-data **S**ynthesizer

![](../images/06_launch.png)

Generating synthetic datasets to improve inference on all sorts of computer-vision tasks that specifically involve
insects. Like, lots of them.

## Rigging 3D models (subjects)

### Requirements:

* [Blender](https://www.blender.org/) (v3.0.1 or later)
* A ([retopologised](01_Retopologising_3D_models.md)) 3D model (of an arthropod) you wish to add to the generator

The key to great synthetic data are great 3D models.
Here, we are going to assume that you already have a [retopologised](01_Retopologising_3D_models.md) 3D model you wish
to use.

![](../images/clean_up_24_rig_01.png)

rendering of a [retopologised](01_Retopologising_3D_models.md) low-polygonal 3D model (with a wireframe overlay)

### Important Blender **shortcuts** to make your life easier:

Now for the most important shortcuts to make interacting with blender easier.

Relating to the viewport:

* **Middle Mouse Button**: (*Hold and move your mouse to*) rotate the viewport.

* **Middle Mouse Button + SHIFT**: (*Hold and move your mouse to*) translate the viewport.

* **num-pad keys**: Moves the viewport to a pre-defined position.

The following shortcuts work in any blender panel, so they are useful to remember:

* **G**: Move – move a selected object around. Click the left mouse button to confirm the new position.

* **S**: Scale – scale a selected object up or down. Here, this will help you to adjust the size of the tracking marker.

* **R**: Rotate – rotate a selected object. Adding the rotation of a marker can help retain its identity across frames.

You can also add constraints to the above commands to specify their effect. *After* pressing any of the keys above you
can additionally use:

* **X** / **Y** / **Z**: Restrict the axis in which you are performing the action.

* **CTRL**: Jump between whole number steps

* **SHIFT**: Perform the change in finer increments. Helps a lot when making small changes.

You always confirm your change by clicking the **LEFT Mouse Button**.

* And, of course, **CTRL** + **Z** is your friend to revert your last steps. By default, you can go back **32 steps**,
  but you can increase that number under **Edit/Preferences…/System** menu, if you want to.

For a handy "cheat-sheet", have a look
at [Blender 3.0 Shortcuts](https://projects.vrac.iastate.edu/reu2022/wp-content/uploads/Blender-3.0-Shortcuts-v1.2.pdf)

For a "not-so handy HotKeys In-depth Reference", refer to the
official [HotKeys In-depth Reference](https://download.blender.org/documentation/BlenderHotkeyReference.pdf)

### Rigging workflow

To make use of our provided animation blueprints, follow
our [standard rigging convention](../example_data/base_rig.blend).

<img src=figures/rig_layout_dark.png#gh-dark-mode-only >
<img src=figures/rig_layout_bright.png#gh-light-mode-only > 

In theory, any model and rig combination is possible. Make sure, your model is posed with all tarsi in ground
contact in a somewhat natural pose. This is important, as we are going to use IK solvers to have the appendages
interact with elements in the generated environment.

In this guide we will only cover **rigging** using our pre-configured armature. More advanced users may wish to create
their own custom **armatures** and blue-prints.

#### Bringing the pre-configured armature into your project

Open the **Blender** project file containing your [retopologised](01_Retopologising_3D_models.md) 3D model and in click 
on **File > Append**. Navigate to the **example_data** directory and select the **base_rig.blend** file.

![](../images/rig_02.PNG)

Navigate to the **Objects** folder, select the **Armature** and import it.

![](../images/rig_03.PNG)


#### WIP

### Next up:

> [**03 Bringing 3D_models into Unreal**](03_Bringing_3D_models_into_Unreal_guide.md)