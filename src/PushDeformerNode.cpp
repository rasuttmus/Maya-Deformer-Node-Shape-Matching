#include "../include/PushDeformerNode.h"

MTypeId PushDeformerNode::id(0x00000002);
MObject PushDeformerNode::time;
MObject PushDeformerNode::gravityMagnitude;
MObject PushDeformerNode::directionX;
MObject PushDeformerNode::directionY;
MObject PushDeformerNode::directionZ;
 
void* PushDeformerNode::creator() { return new PushDeformerNode; }

MStatus PushDeformerNode::deform(MDataBlock& data, MItGeometry& it_geo,
                          const MMatrix &local_to_world_matrix, unsigned int m_index) {
  MStatus status;
  
  // Fetch the envelope and the inflation input value
  float env = data.inputValue(envelope).asFloat();
  MTime time_data = data.inputValue(time).asTime();
  double gravity = data.inputValue(gravityMagnitude).asDouble();
  double gravityDirectionX = data.inputValue(directionX).asDouble();
  double gravityDirectionY = data.inputValue(directionY).asDouble();
  double gravityDirectionZ = data.inputValue(directionZ).asDouble();

  // Get the input mesh (fn_input_mesh)
  MArrayDataHandle h_input = data.outputArrayValue( input, &status );
  CHECK_MSTATUS_AND_RETURN_IT( status )
  status = h_input.jumpToElement( m_index );
  CHECK_MSTATUS_AND_RETURN_IT( status )
  MObject o_input_geom = h_input.outputValue().child( inputGeom ).asMesh();
  MFnMesh fn_input_mesh( o_input_geom );

  // Get the normal array from the input mesh
  MFloatVectorArray normals = MFloatVectorArray();
  fn_input_mesh.getVertexNormals(true, normals, MSpace::kTransform);

  // Loop through the geometry and set vertex positions
    
  for (; !it_geo.isDone(); it_geo.next()) {
    
    int idx = it_geo.index();
    MVector nrm = MVector(normals[idx]);
    MPoint pos = it_geo.position();

    MPoint new_pos = pos * local_to_world_matrix;

    if(new_pos.y < 0.0)
      new_pos.y = 0.0;

    it_geo.setPosition(new_pos * local_to_world_matrix.inverse());
  }

  return MS::kSuccess;
}
 
MStatus PushDeformerNode::initialize() {

  MFnTypedAttribute tAttr;
  MFnNumericAttribute nAttr;
  MFnUnitAttribute uAttr;

  time = uAttr.create("time", "tm", MFnUnitAttribute::kTime, 0.0);
  uAttr.setDefault(MAnimControl::currentTime().as(MTime::kFilm));
  uAttr.setChannelBox(true);

  // Create a numeric attribute "gravityMagnitude"
  gravityMagnitude = nAttr.create("gravityMagnitude", "gm", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setMin(0.0);
  nAttr.setMax(10.0);
  nAttr.setChannelBox(true);

  directionX = nAttr.create("directionX", "dirx", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setMin(-1.0);
  nAttr.setMax(1.0);
  nAttr.setChannelBox(true);

  directionY = nAttr.create("directionY", "diry", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setMin(-1.0);
  nAttr.setMax(1.0);
  nAttr.setChannelBox(true);

  directionZ = nAttr.create("directionZ", "dirz", MFnNumericData::kDouble, 0.0);
  nAttr.setDefault(0.0);
  nAttr.setMin(-1.0);
  nAttr.setMax(1.0);
  nAttr.setChannelBox(true);

  // Add the attribute
  addAttribute(time);
  addAttribute(gravityMagnitude);
  addAttribute(directionX);
  addAttribute(directionY);
  addAttribute(directionZ);
  attributeAffects(time, outputGeom);
  attributeAffects(gravityMagnitude, outputGeom);
  attributeAffects(directionX, outputGeom);
  attributeAffects(directionY, outputGeom);
  attributeAffects(directionZ, outputGeom);

  // Make the deformer weights paintable (maybe wait with this)
  // MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer PushDeformerNode weights;");
 
  return MS::kSuccess;
}