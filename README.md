# Maya_Custom_Deformer_Plugin
A plugin for Maya that writes Hello World

Based on the tutorials by Chad Vernon:
* http://www.chadvernon.com/blog/resources/maya-api-programming/your-first-plug-in/
* http://www.chadvernon.com/blog/maya/compiling-maya-plug-ins-with-cmake/

And this tutorial for deformer plugins:
* http://www.ngreen.org/2014/08/implementing-a-simple-maya-python-deformer/

Build on OSX:
  * mkdir build
  * cd build
  * cmake -G "Unix Makefiles" -DMAYA_VERSION=2015 ../ (or whatever maya version)

Compile on OSX:
  * cd in to build/ directory
  * sudo cmake --build . --config Release --target install

For OSX, the compiler will build the file sampleplugin.bundle,
  * Open Maya and go to Window -> Settings/Preferences -> Plug-in Manager
  * Browse for sampleplugin.bundle and open.

Now the helloWorld function can be run from the python terminal by:
  * import maya.cmds as cmds
  * cmds.helloWorld()

The deformer plugin is used as follows:
  * Create an object to be deformed
  * Select the object
  * Apply the deformer with:
  	cmds.deformer(type='pushDeformer')
  * By changing the parameters "envelope" and "inflation" the deformer changes the mesh