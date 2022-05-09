# FARTS
Fabi And René's Training-data Synthesizer

For now, just the parsers are hosted here, but we'll soon include the entire generator source code as well.

# Parser TODOs

* Update all parsers to use new **json** data format
  * YOLO
    * update tight bounding boxes. Currently, only native bounding boxes are supported as during debugging only select body part reporters are used
    * update enforcing centred bounding boxes with optionally tying bounding boxes to just one keypoint (also reuqires full keypoint list
  * DLC
    * update keypoint naming convention
    * double check data parsing with complete skeleton
  * HERO DLC
    * read in new json format files
  * COCO
    * read in new json format files
  * Custom 3D
    * read in new json format files
    * check new 3D camera intrinsics and extrinsics and ensure the reprojected 3D coordinates align with the original 2D coordinates
* Move legacy generator parsers to separate folder as each parser is updated
* Generate new example datasets
  * Single
  * Multi
* Update colony files to include (for each subject)
  * unique ID (corresponding to colour value in ID pass)
  * model name (for classification / weight estimation)
  * scale (as float value, to compare size variation info)
