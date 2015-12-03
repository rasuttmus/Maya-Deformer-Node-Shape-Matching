# Maya_Custom_Deformer_Plugin
A plugin for Maya to deform objects using shape matching.

Base from a simple hello world maya plugin:
https://github.com/kbladin/Maya_Plugin_Hello_World
The deformation is based on this paper:
http://www.beosil.com/download/MeshlessDeformations_SIG05.pdf

Build:
  * OSX
    * mkdir build
    * cd build
    * cmake -G "Unix Makefiles" -DMAYA_VERSION=2015 ../ (or whatever maya version)
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
also connect all the attributes needed from the rigid body to to be able to
calculate the deformations.