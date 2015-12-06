#include <string>

#include "../include/ShapeDeformerNode.h"

MTypeId ShapeDeformerNode::id(0x00000002);
MObject ShapeDeformerNode::aGravityMagnitude;
MObject ShapeDeformerNode::aGravityDirection;

MObject ShapeDeformerNode::aCurrentTime;
MObject ShapeDeformerNode::aLastSceneTime;
MObject ShapeDeformerNode::aVelocity;
MObject ShapeDeformerNode::aSpin;
MObject ShapeDeformerNode::aCenterOfMass;
MObject ShapeDeformerNode::aImpulse;
MObject ShapeDeformerNode::aImpulsePosition;
MObject ShapeDeformerNode::aSpinImpulse;
MObject ShapeDeformerNode::aMass;
MObject ShapeDeformerNode::aVolume;
MObject ShapeDeformerNode::aBounciness;
MObject ShapeDeformerNode::aDamping;
MObject ShapeDeformerNode::aStaticFriction;
MObject ShapeDeformerNode::aDynamicFriction;
MObject ShapeDeformerNode::aForce;
MObject ShapeDeformerNode::aTorque;

bool ShapeDeformerNode::firstFrame;
ParticleSystem* ShapeDeformerNode::ps;
 
void* ShapeDeformerNode::creator() { return new ShapeDeformerNode; }

MStatus ShapeDeformerNode::deform(MDataBlock& data, MItGeometry& itGeo,
                          const MMatrix &localToWorldMatrix, unsigned int mIndex)
{
  if (firstFrame)
  { // Ugly hack (constructor here)
    std::vector<glm::vec3> p0;
    for (; !itGeo.isDone(); itGeo.next()) {
      MPoint vertexPos = itGeo.position();
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
    MTime tNow, tPrev, dt;
    double
      m,        // Mass
      vol,      // Volume
      b,        // Bounciness
      dp,       // Damping
      sf,       // Static friction
      df;       // Dynamic friction

    MVector
      g,        // Gravity vector
      V,        // Velocity
      spin,     // Angular velocity
      com,      // Center of Mass
      imp,      // Impulse
      impPos,   // Impulse Position
      spinImp,  // Spin Impulse
      F,        // Net force
      T;        // Net torque
    MPoint vertexPos;

    // Fetch the attribute values
    env = data.inputValue(envelope).asFloat();
    tNow = data.inputValue(aCurrentTime).asTime();
    tPrev = data.inputValue(aLastSceneTime).asTime();
    dt = tPrev - tNow;
    g = data.inputValue(aGravityMagnitude).asDouble() *
        data.inputValue(aGravityDirection).asVector();
    V = data.inputValue(aVelocity).asVector();
    spin = data.inputValue(aSpin).asVector();
    com = data.inputValue(aCenterOfMass).asVector();
    imp = data.inputValue(aImpulse).asVector();
    impPos = data.inputValue(aImpulsePosition).asVector();
    spinImp = data.inputValue(aSpinImpulse).asVector();
    m = data.inputValue(aMass).asDouble();
    vol = data.inputValue(aVolume).asDouble();
    b = data.inputValue(aBounciness).asDouble();
    dp = data.inputValue(aDamping).asDouble();
    df = data.inputValue(aStaticFriction).asDouble();
    sf = data.inputValue(aDynamicFriction).asDouble();
    F = data.inputValue(aForce).asVector();
    T = data.inputValue(aTorque).asVector();
    
    // Get the input mesh (fnInputMesh)
    MArrayDataHandle hInput = data.outputArrayValue( input, &status );
    CHECK_MSTATUS_AND_RETURN_IT( status )
    status = hInput.jumpToElement( mIndex );
    CHECK_MSTATUS_AND_RETURN_IT( status )
    MObject oInputGeom = hInput.outputValue().child( inputGeom ).asMesh();
    MFnMesh fnInputMesh( oInputGeom );
    
    if (ps)
    {
      ps->stepPhysics(1 / 24.0);
      ps->matchShape(1 / 24.0);
    }
    else
    {
      MGlobal::displayInfo("ps == NULL");
    }
    for (; !itGeo.isDone(); itGeo.next()) {
      int idx = itGeo.index();

      glm::vec3 p = ps->getPosition(idx);
      MPoint newPos(p.x, p.y, p.z);
      itGeo.setPosition(newPos);
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

  aLastSceneTime = uAttr.create("aLastSceneTime", "lst", MFnUnitAttribute::kTime, 0.0);
  uAttr.setDefault(MAnimControl::currentTime().as(MTime::kFilm));
  uAttr.setChannelBox(true);
  
  aVelocity = nAttr.create("aVelocity", "vel", MFnNumericData::k3Double, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aSpin = nAttr.create("aSpin", "spn", MFnNumericData::k3Double, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aCenterOfMass = nAttr.create("aCenterOfMass", "com", MFnNumericData::k3Double, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aImpulse = nAttr.create("aImpulse", "imp", MFnNumericData::k3Double, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aImpulsePosition = nAttr.create("aImpulsePosition", "ipo", MFnNumericData::k3Double, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aSpinImpulse = nAttr.create("aSpinImpulse", "sim", MFnNumericData::k3Double, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aMass = nAttr.create("aMass", "mas", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(1.0);
  nAttr.setChannelBox(true);

  aVolume = nAttr.create("aVolume", "vol", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aBounciness = nAttr.create("aBounciness", "b", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aDamping = nAttr.create("aDamping", "dp", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aStaticFriction = nAttr.create("aStaticFriction", "sf", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aDynamicFriction = nAttr.create("aDynamicFriction", "df", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aForce = nAttr.create("aForce", "for", MFnNumericData::k3Double, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  aTorque = nAttr.create("aTorque", "tor", MFnNumericData::k3Double, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setChannelBox(true);

  // Add the attributes
  addAttribute(aCurrentTime);
  addAttribute(aLastSceneTime);
  addAttribute(aGravityMagnitude);
  addAttribute(aGravityDirection);
  addAttribute(aVelocity);
  addAttribute(aSpin);
  addAttribute(aCenterOfMass);
  addAttribute(aImpulse);
  addAttribute(aImpulsePosition);
  addAttribute(aSpinImpulse);
  addAttribute(aMass);
  addAttribute(aVolume);
  addAttribute(aBounciness);
  addAttribute(aDamping);
  addAttribute(aStaticFriction);
  addAttribute(aDynamicFriction);
  addAttribute(aForce);
  addAttribute(aTorque);

  // Affecting (What does this do?)
  attributeAffects(aCurrentTime, outputGeom);
  attributeAffects(aLastSceneTime, outputGeom);
  attributeAffects(aGravityMagnitude, outputGeom);
  attributeAffects(aGravityDirection, outputGeom);
  attributeAffects(aVelocity, outputGeom);
  attributeAffects(aSpin, outputGeom);
  attributeAffects(aCenterOfMass, outputGeom);
  attributeAffects(aImpulse, outputGeom);
  attributeAffects(aImpulsePosition, outputGeom);
  attributeAffects(aSpinImpulse, outputGeom);
  attributeAffects(aMass, outputGeom);
  attributeAffects(aVolume, outputGeom);
  attributeAffects(aBounciness, outputGeom);
  attributeAffects(aDamping, outputGeom);
  attributeAffects(aStaticFriction, outputGeom);
  attributeAffects(aDynamicFriction, outputGeom);
  attributeAffects(aForce, outputGeom);
  attributeAffects(aTorque, outputGeom);

  // Make the deformer weights paintable (maybe wait with this)
  // MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer ShapeDeformerNode weights;");
 
  return MS::kSuccess;
}