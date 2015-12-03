#include "../include/HelloWorldCmd.h"

void* HelloWorld::creator() { return new HelloWorld; }
 
MStatus HelloWorld::doIt(const MArgList& arg_list) {
  MGlobal::displayInfo("Hello World!");
  return MS::kSuccess;
}