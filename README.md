## csgparser

File *csgparser.cpp* implements a simple reference parser of [CSG format](https://github.com/megaton/csg-format).
It depends on two libs:

* [json11](https://github.com/dropbox/json11)
* [lars::parser](https://github.com/TheLartians/Parser)

Which already included (hopefully legally) into this repository.

JSON format is used as intermediate data representation in order to not introduce yet another new format.

## csg2json

File *csg2json.cpp* implements a simple CSG to JSON back and forth converter which serves for  the number of important tasks:
* It is a sample of using csgparser
* It performs validation of CSG files (or at least it should)
* It converts CSG files to JSON so you may stick to JSON both for import and export in your app

## csgviewer

File *csgviewer.cpp* implements a simple OpenGL based CSG 3d viewer.

* Firstly it converts CSG tree to distance field on 3d grid.
* Then it uploads 3d grid to GPU (3d texture) and renders it interactively.

## license

MIT License
