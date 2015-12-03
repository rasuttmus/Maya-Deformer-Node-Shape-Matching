#include "../include/HelloWorldCmd.h"

void* HelloWorld::creator() { return new HelloWorld; }
 
MStatus HelloWorld::doIt(const MArgList& argList) {
  MGlobal::displayInfo("Hello World!");
  
  return MS::kSuccess;
}