#include <string>

#include "../include/ShapeDeformerNode.h"


#define SIGN(a) (a < 0 ? -1 : 1)

MTypeId ShapeDeformerNode::id(0x00000002);
MObject ShapeDeformerNode::aGravityMagnitude;
MObject ShapeDeformerNode::aGravityDirection;

MObject ShapeDeformerNode::aCurrentTime;
MObject ShapeDeformerNode::aMass;
MObject ShapeDeformerNode::aStiffness;
MObject ShapeDeformerNode::aStaticFriction;
MObject ShapeDeformerNode::aDynamicFriction;

bool ShapeDeformerNode::firstFrame;
ParticleSystem* ShapeDeformerNode::ps;
MTime ShapeDeformerNode::tPrevious;

void* ShapeDeformerNode::creator() { return new ShapeDeformerNode; }

MStatus ShapeDeformerNode::deform(MDataBlock& data, MItGeometry& itGeo,
                          const MMatrix &localToWorldMatrix, unsigned int mIndex)
{
  MTime tNow = data.inputValue(aCurrentTime).asTime();
  if (firstFrame || tNow.value() == 1)
  { // Ugly hack (constructor here)
    tPrevious = data.inputValue(aCurrentTime).asTime();
    std::vector<glm::vec3> p0;
    for (; !itGeo.isDone(); itGeo.next()) {
      // Positions in world coordinates
      MPoint vertexPos = itGeo.position() * localToWorldMatrix;
      glm::vec3 p0i(vertexPos.x, vertexPos.y, vertexPos.z);
      p0.push_back(p0i);
    }
    ps = new ParticleSystem(p0);

    firstFrame = false;
    return MS::kSuccess;
  }
  else
  { // Update 
    MStatus status;
    
    // Data used in the calculations
    float env;
    double
      m,        // Mass
      st,       // Damping
      sf,       // Static friction
      df;       // Dynamic friction

    MVector
      g;        // Gravity vector
    MPoint vertexPos;

    // Fetch the attribute values
    env = data.inputValue(envelope).asFloat();
    tNow = data.inputValue(aCurrentTime).asTime();
    MTime timeDiff = tNow - tPrevious;
    tPrevious = tNow;
    g = data.inputValue(aGravityMagnitude).asDouble() *
        data.inputValue(aGravityDirection).asVector();
    m = data.inputValue(aMass).asDouble();
    st = data.inputValue(aStiffness).asDouble();
    df = data.inputValue(aStaticFriction).asDouble();
    sf = data.inputValue(aDynamicFriction).asDouble();
    
    // Get the input mesh (fnInputMesh)
    MArrayDataHandle hInput = data.outputArrayValue( input, &status );
    CHECK_MSTATUS_AND_RETURN_IT( status )
    status = hInput.jumpToElement( mIndex );
    CHECK_MSTATUS_AND_RETURN_IT( status )
    MObject oInputGeom = hInput.outputValue().child( inputGeom ).asMesh();
    MFnMesh fnInputMesh( oInputGeom );

    //std::string output = "timeDiff = ";
    //output += std::to_string(timeDiff.value());
    //MGlobal::displayInfo(output.c_str());

    if (ps)
    {
      int updates = timeDiff.value();
      int updatesPerTimeStep = 1;
      for (int i = 0; i < abs(updates) * updatesPerTimeStep; ++i)
      {
        ps->stepPhysics(1 / 24.0 / updatesPerTimeStep * SIGN(updates));
        ps->matchShape(1 / 24.0 / updatesPerTimeStep * SIGN(updates));
      }
    }
    else
    {
      MGlobal::displayInfo("ps == NULL");
    }
    MMatrix localToWorldMatrixInv = localToWorldMatrix.inverse();
    for (; !itGeo.isDone(); itGeo.next())
    {
      int idx = itGeo.index();

      glm::vec3 p = ps->getPosition(idx);
      MPoint newPos(p.x, p.y, p.z);
      // Transform back to model coordinates
      itGeo.setPosition(newPos * localToWorldMatrixInv);
    }
  }

  return MS::kSuccess;
}

MStatus ShapeDeformerNode::initialize()
{
  firstFrame = true;

  MFnTypedAttribute tAttr;
  MFnNumericAttribute nAttr;
  MFnUnitAttribute uAttr;

  // Create numeric attributes needed in the deformation
  aGravityMagnitude = nAttr.create("aGravityMagnitude", "gm", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setMin(0.0);
  nAttr.setMax(10.0);
  nAttr.setChannelBox(true);

  aGravityDirection = nAttr.create("aGravityDirection", "gd", MFnNumericData::k3Double, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setMin(-1.0);
  nAttr.setMax(1.0);
  nAttr.setChannelBox(true);

  aCurrentTime = uAttr.create("aCurrentTime", "ct", MFnUnitAttribute::kTime, 0.0);
  uAttr.setDefault(MAnimControl::currentTime().as(MTime::kFilm));
  uAttr.setChannelBox(true);

  aMass = nAttr.create("aMass", "mas", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(1.0);
  nAttr.setChannelBox(true);

  aStiffness = nAttr.create("aStiffness", "st", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aStaticFriction = nAttr.create("aStaticFriction", "sf", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aDynamicFriction = nAttr.create("aDynamicFriction", "df", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  // Add the attributes
  addAttribute(aCurrentTime);
  addAttribute(aGravityMagnitude);
  addAttribute(aGravityDirection);
  addAttribute(aMass);
  addAttribute(aStiffness);
  addAttribute(aStaticFriction);
  addAttribute(aDynamicFriction);

  // Affecting (What does this do?)
  attributeAffects(aCurrentTime, outputGeom);
  attributeAffects(aGravityMagnitude, outputGeom);
  attributeAffects(aGravityDirection, outputGeom);
  attributeAffects(aMass, outputGeom);
  attributeAffects(aStiffness, outputGeom);
  attributeAffects(aStaticFriction, outputGeom);
  attributeAffects(aDynamicFriction, outputGeom);

  // Make the deformer weights paintable (maybe wait with this)
  // MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer ShapeDeformerNode weights;");
 
  return MS::kSuccess;
}