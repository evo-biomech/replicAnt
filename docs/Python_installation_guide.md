# FARTS
**F**abi **A**nd **R**ené's **T**raining-data **S**ynthesizer

![](documentation_images/06_launch.png)

Generating synthetic datasets to improve inference on all sorts of computer-vision tasks that specifically involve insects. Like, lots of them.

### Python Environment (for parser notebooks)

To convert the generated datasets into formats accepted by common computer vision frameworks,
we have provided a number of [parsers](UE5_parsers) as interactive Jupyter notebooks.

You require a python installation on your system to make use of them. For ease of use, we
provide an example conda environment, as well as a list of dependencies, in case you want to
use a custom python installation / environment:

**Install dependencies via conda**

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
  - notebook
  - numpy
  - matplotlib
  - opencv
  - json5
  - pandas
  - pathlib
  - imutils
  - scikit-learn