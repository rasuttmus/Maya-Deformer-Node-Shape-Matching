# Soft Body Maya Deformer Using Shape Matching
A plugin for Maya to deform objects using shape matching.

Base from a simple hello world maya plugin:
https://github.com/kbladin/Maya_Plugin_Hello_World

The deformation is based on this paper:
http://www.beosil.com/download/MeshlessDeformations_SIG05.pdf

Build project files with cmake:
  * OSX
    * mkdir build
    * cd build
    * cmake -G "Unix Makefiles" -DMAYA_VERSION=2015 ../ (or whatever maya version)
    * You may have to specify MAYA_LIBRARY_DIR in cmake (then reconfigure) as well as MAYA_INCLUDE_DIR if they are not found
  * Linux
    * ..
  * Windows
    * ..

Compile:
  * OSX
    * cd in to build/ directory
    * sudo cmake --build . --config Release --target install
  * Linux
    * ..
  * Windows
    * ..

Load plugin:
  * OSX:
    * The compiler will build the file sampleplugin.bundle,
  * Linux
    * ..
  * Windows
    * ..
  * Open Maya and go to Window -> Settings/Preferences -> Plug-in Manager
  * Browse for sampleplugin and open.

Now the helloWorld function can be run from the python terminal by:
  * import maya.cmds as cmds
  * cmds.helloWorld()

The deformer plugin is used as follows:
  * Create an object to be deformed
  * Select the object
  * Apply the deformer with:
  	cmds.deformer(type='shapeMatchDeformer')

A default scene can be created by running the script melScripts/createExampleScene.mel
This will create a sphere and add a shapeMatchDeformer-node to it. It will
also connect the attributes (gravity and time) so that the simulation can be run.

If Armadillo needs to be compiled:
  * OSX
    * Go to /ext/armadillo/armadillo-6.300.2
    * cd build
    * cmake ..
    * Copy libarmadillo.6.30.2.dylib, libarmadillo.6.dylib and libarmadillo.dylib to /ext/armadillo/lib
  * Linux
    * ..
  * Windows
    * ..