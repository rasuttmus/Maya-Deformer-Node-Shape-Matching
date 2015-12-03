# Maya_Custom_Deformer_Plugin
A plugin for Maya that writes Hello World

Based on the tutorials by Chad Vernon:
* http://www.chadvernon.com/blog/resources/maya-api-programming/your-first-plug-in/
* http://www.chadvernon.com/blog/maya/compiling-maya-plug-ins-with-cmake/

And this tutorial for creating deformer plugins:
* http://www.ngreen.org/2014/08/implementing-a-simple-maya-python-deformer/

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