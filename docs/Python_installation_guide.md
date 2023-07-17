# replicAnt

"generating annotated images of animals in complex environments with Unreal Engine"

by [Fabian **Plum**](https://twitter.com/fabian_plum), 
[René **Bulla**](https://twitter.com/renebulla), 
[Hendrik **Beck**](https://twitter.com/Hendrik_Beck), 
[Natalie **Imirzian**](https://twitter.com/nimirzy), 
and [David **Labonte**](https://twitter.com/EvoBiomech) (2023)

___

![](../images/06_launch_better_together.png)

___

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



___

> In case you encounter any problems, consult our [troubleshooting guide](troubleshooting.md), or consider raising an
> **issue** on the replicAnt GitHub page.
 
## License
© Fabian Plum, Rene Bulla, David Labonte 2023
[MIT License](https://choosealicense.com/licenses/mit/)
