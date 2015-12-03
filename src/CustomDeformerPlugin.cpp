#include "../include/CustomDeformerPlugin.h"

MStatus initializePlugin(MObject obj) {
  MStatus status;
  MFnPlugin plugin(obj, "Rasmus, Tim, Kalle", "1.0", "Any");
 
  // Make a deformer node
  status = plugin.registerNode("shapeMatchDeformer", ShapeDeformerNode::id, ShapeDeformerNode::creator,
                               ShapeDeformerNode::initialize, MPxNode::kDeformerNode);
  CHECK_MSTATUS_AND_RETURN_IT(status);

  // Make a hello world command
  status = plugin.registerCommand("helloWorld", HelloWorld::creator);
  CHECK_MSTATUS_AND_RETURN_IT(status);
 
  return status;
}
 
MStatus uninitializePlugin(MObject obj) {
  MStatus     status;
  MFnPlugin plugin(obj);
 
  status = plugin.deregisterNode(ShapeDeformerNode::id);
  CHECK_MSTATUS_AND_RETURN_IT(status);
 
  status = plugin.deregisterCommand("helloWorld");
  CHECK_MSTATUS_AND_RETURN_IT(status);
  return status;
}