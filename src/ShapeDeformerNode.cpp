#include <string>

#include "../include/ShapeDeformerNode.h"

#define SIGN(a) (a < 0 ? -1 : 1)

MTypeId ShapeDeformerNode::id(0x00000002);
MObject ShapeDeformerNode::GravityMagnitude;
MObject ShapeDeformerNode::GravityDirection;

MObject ShapeDeformerNode::CurrentTime;
MObject ShapeDeformerNode::Mass;
MObject ShapeDeformerNode::Stiffness;
MObject ShapeDeformerNode::Flappyness;
MObject ShapeDeformerNode::Deformation;
MObject ShapeDeformerNode::Elasticity;
MObject ShapeDeformerNode::StaticFriction;
MObject ShapeDeformerNode::DynamicFriction;
MObject ShapeDeformerNode::InitialVelocity;

bool ShapeDeformerNode::firstFrame;
ParticleSystem* ShapeDeformerNode::ps;
MTime ShapeDeformerNode::tPrevious;

void* ShapeDeformerNode::creator() { return new ShapeDeformerNode; }

MStatus ShapeDeformerNode::deform(MDataBlock& data, MItGeometry& itGeo,
                          const MMatrix &localToWorldMatrix, unsigned int mIndex)
{
  MTime tNow = data.inputValue(CurrentTime).asTime();
  if (firstFrame || tNow.value() == 1)
  { // Ugly hack (constructor here)...
    //Can we access the geometry in the constructor instead?
    if(ps)
      delete ps;

    tPrevious = data.inputValue(CurrentTime).asTime();
    std::vector<glm::vec3> p0;
    glm::vec3 v0 = to_glm(data.inputValue(InitialVelocity).asVector());
    for (; !itGeo.isDone(); itGeo.next()) {
      // Positions in world coordinates
      MPoint vertexPos = itGeo.position() * localToWorldMatrix;
      glm::vec3 p0i(vertexPos.x, vertexPos.y, vertexPos.z);
      p0.push_back(p0i);
    }
    ps = new ParticleSystem(p0, v0);

    firstFrame = false;
    return MS::kSuccess;
  }
  else
  { // Update 
    MStatus status;
    // Currently we don't use the envelope
    // (would be weird since we have our own particle system)
    float env;
    PhysicsArguments pArg;
    MPoint vertexPos;

    // Fetch the attribute values
    env = data.inputValue(envelope).asFloat();
    tNow = data.inputValue(CurrentTime).asTime();
    MTime timeDiff = tNow - tPrevious;
    tPrevious = tNow;
    pArg.gravity = to_glm(data.inputValue(GravityMagnitude).asDouble() *
        data.inputValue(GravityDirection).asVector());
    pArg.mass = data.inputValue(Mass).asDouble();
    pArg.stiffness = data.inputValue(Stiffness).asDouble();
    pArg.flappyness = data.inputValue(Flappyness).asDouble();
    pArg.deformation = data.inputValue(Deformation).asDouble();
    pArg.elasticity = data.inputValue(Elasticity).asDouble();
    pArg.dynamicFriction = data.inputValue(DynamicFriction).asDouble();
    pArg.staticFriction = data.inputValue(StaticFriction).asDouble();

    // Update the particle systems positions with dynamics simulation and
    // Shape matching
    if (ps)
    {
      int updates = timeDiff.value();
      int updatesPerTimeStep = 2;
      for (int i = 0; i < abs(updates) * updatesPerTimeStep; ++i)
      {
        ps->stepPhysics(1 / 24.0 / updatesPerTimeStep * SIGN(updates), pArg);
        ps->matchShape(1 / 24.0 / updatesPerTimeStep * SIGN(updates), pArg);
      }
    }
    else
    {
      MGlobal::displayInfo("ps == NULL");
    }

    // Update output positions
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
  GravityMagnitude = nAttr.create("GravityMagnitude", "gm", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setMin(0.0);
  nAttr.setMax(10.0);
  nAttr.setChannelBox(true);

  GravityDirection = nAttr.create("GravityDirection", "gd", MFnNumericData::k3Double, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setMin(-1.0);
  nAttr.setMax(1.0);
  nAttr.setChannelBox(true);

  CurrentTime = uAttr.create("CurrentTime", "ct", MFnUnitAttribute::kTime, 0.0);
  uAttr.setDefault(MAnimControl::currentTime().as(MTime::kFilm));
  uAttr.setChannelBox(true);

  Mass = nAttr.create("Mass", "mas", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(1.0);
  nAttr.setChannelBox(true);
  nAttr.setMin(0.0);
  nAttr.setMax(10.0);

  Stiffness = nAttr.create("Stiffness", "st", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.5);
  nAttr.setChannelBox(true);
  nAttr.setMin(0.0);
  nAttr.setMax(1.0);

  Flappyness = nAttr.create("Flappyness", "fl", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.5);
  nAttr.setChannelBox(true);
  nAttr.setMin(0.0);
  nAttr.setMax(1.0);

  Deformation = nAttr.create("Deformation", "de", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.5);
  nAttr.setChannelBox(true);
  nAttr.setMin(0.0);
  nAttr.setMax(1.0);
  
  Elasticity = nAttr.create("Elasticity", "el", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.5);
  nAttr.setChannelBox(true);
  nAttr.setMin(0.0);
  nAttr.setMax(1.0);
  
  StaticFriction = nAttr.create("StaticFriction", "sf", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);
  nAttr.setMin(0.0);
  nAttr.setMax(1.0);
  
  DynamicFriction = nAttr.create("DynamicFriction", "df", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.5);
  nAttr.setChannelBox(true);
  nAttr.setMin(0.0);
  nAttr.setMax(1.0);
  
  InitialVelocity = nAttr.create("InitialVelocity", "iv", MFnNumericData::k3Double, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setMin(-10.0);
  nAttr.setMax(10.0);
  nAttr.setChannelBox(true);

  // Add the attributes
  addAttribute(CurrentTime);
  addAttribute(GravityMagnitude);
  addAttribute(GravityDirection);
  addAttribute(Mass);
  addAttribute(Stiffness);
  addAttribute(Flappyness);
  addAttribute(Deformation);
  addAttribute(Elasticity);
  addAttribute(StaticFriction);
  addAttribute(DynamicFriction);
  addAttribute(InitialVelocity);

  // Affecting (What does this do?)
  attributeAffects(CurrentTime, outputGeom);
  attributeAffects(GravityMagnitude, outputGeom);
  attributeAffects(GravityDirection, outputGeom);
  attributeAffects(Mass, outputGeom);
  attributeAffects(Stiffness, outputGeom);
  attributeAffects(Flappyness, outputGeom);
  attributeAffects(Deformation, outputGeom);
  attributeAffects(Elasticity, outputGeom);
  attributeAffects(StaticFriction, outputGeom);
  attributeAffects(DynamicFriction, outputGeom);
  attributeAffects(InitialVelocity, outputGeom);

  // Make the deformer weights paintable (maybe wait with this)
  // MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer ShapeDeformerNode weights;");
 
  return MS::kSuccess;
}