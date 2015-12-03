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
    virtual MStatus deform(MDataBlock& data, MItGeometry& itGeo,
                         const MMatrix &localToWorldMatrix, unsigned int mIndex);
    static void* creator();
    static MStatus initialize();

    // Unique id
    static MTypeId id;

    // Attributes (a as in attribute on variable names)
    // Global attributes
    static MObject aGravityMagnitude;
    static MObject aGravityDirection;

    // Object attributes from rigid body
    static MObject aCurrentTime;
    static MObject aLastSceneTime;
    static MObject aVelocity;
    static MObject aSpin;
    static MObject aCenterOfMass;
    static MObject aImpulse;
    static MObject aImpulsePosition;
    static MObject aSpinImpulse;
    static MObject aMass;
    static MObject aVolume;
    static MObject aBounciness;
    static MObject aDamping;
    static MObject aStaticFriction;
    static MObject aDynamicFriction;
    static MObject aForce;
    static MObject aTorque;
};

#endif