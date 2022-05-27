# FARTS
Fabi And René's Training-data Synthesizer

For now, just the parsers are hosted here, but we'll soon include the entire generator source code as well.

# Parser TODOs

* Update all parsers to use new **json** data format
  * YOLO
    * DONE
  * DLC
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

* Add guide on how to solve EXCEPTION_ACCESS_VIOLATION issues under Windows (10 & 11)
