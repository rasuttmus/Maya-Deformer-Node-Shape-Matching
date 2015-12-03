#ifndef HELLOWORLD_H
#define HELLOWORLD_H
 
#include <maya/MArgList.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>

// Simple hello world command
class HelloWorld : public MPxCommand {
 public:
  HelloWorld() {};
  virtual MStatus doIt(const MArgList& arg_list);
  static void* creator();
};

#endif