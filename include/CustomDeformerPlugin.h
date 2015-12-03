#ifndef CUSTOM_DEFORMER_PLUGIN

#include <maya/MFnPlugin.h>
#include "../include/HelloWorldCmd.h"
#include "../include/PushDeformerNode.h"


// The base of the plugin

// Initializing the plugin will create the differnet commands and the
// nodes that can be created.
// (One hello world command and one deformer node)
MStatus initializePlugin(MObject obj);
MStatus uninitializePlugin(MObject obj);

#define CUSTOM_DEFORMER_PLUGIN
#endif