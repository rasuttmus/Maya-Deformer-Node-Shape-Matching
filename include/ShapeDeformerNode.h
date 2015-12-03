#ifndef PUSH_DEFORMER_NODE
#define PUSH_DEFORMER_NODE

#include <math.h>

#include <maya/MTime.h>
#include <maya/MAnimControl.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MGlobal.h>
#include <maya/MItGeometry.h>
#include <maya/MMatrix.h>
#include <maya/MPointArray.h>
#include <maya/MStatus.h>
 
#include <maya/MFnMesh.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatVectorArray.h>

#include <maya/MPxDeformerNode.h>

// Simple deformer node
class ShapeDeformerNode : public MPxDeformerNode {

public:
    ShapeDeformerNode() {}
    virtual MStatus deform(MDataBlock& data, MItGeometry& it_geo,
                         const MMatrix &local_to_world_matrix, unsigned int m_index);
    static void* creator();
    static MStatus initialize();

    // Unique id
    static MTypeId id;

    // Attributes
    static MObject time;
    static MObject gravityMagnitude;
    static MObject gravityDirection;
};

#endif