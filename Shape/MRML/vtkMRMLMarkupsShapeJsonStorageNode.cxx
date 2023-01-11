#include "vtkMRMLMarkupsShapeJsonStorageNode.h"

#include "vtkMRMLMarkupsJsonElement.h"
#include "vtkMRMLMarkupsShapeNode.h"
#include "vtkMRMLScene.h"

#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsShapeJsonStorageNode);

//----------------------------------------------------------------------------
vtkMRMLMarkupsShapeJsonStorageNode::vtkMRMLMarkupsShapeJsonStorageNode()
{
}

//----------------------------------------------------------------------------
vtkMRMLMarkupsShapeJsonStorageNode::~vtkMRMLMarkupsShapeJsonStorageNode() = default;

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeJsonStorageNode::CanReadInReferenceNode(vtkMRMLNode *refNode)
{
  return refNode->IsA("vtkMRMLMarkupsShapeNode");
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeJsonStorageNode::WriteBasicProperties(
  vtkMRMLMarkupsJsonWriter* writer, vtkMRMLMarkupsNode* markupsNode)
{
 if (!vtkMRMLMarkupsJsonStorageNode::WriteBasicProperties(writer, markupsNode))
    {
    return false;
    }

  vtkMRMLMarkupsShapeNode* shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(markupsNode);
  if (!shapeNode)
    {
    vtkErrorWithObjectMacro(this, "vtkMRMLMarkupsShapeJsonStorageNode::WriteBasicProperties failed: invalid markupsNode");
    return false;
    }

  writer->WriteStringProperty("shapeName", shapeNode->GetShapeNameAsString(shapeNode->GetShapeName()));
  writer->WriteStringProperty("radiusMode", shapeNode->GetRadiusModeAsString(shapeNode->GetRadiusMode()));
  writer->WriteStringProperty("drawMode2D", shapeNode->GetDrawMode2DAsString(shapeNode->GetDrawMode2D()));
  writer->WriteDoubleProperty("resolution", shapeNode->GetResolution());
  // Ignoring shapeNode->ResliceNode.

  return true;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeJsonStorageNode::UpdateMarkupsNodeFromJsonValue(vtkMRMLMarkupsNode* markupsNode, vtkMRMLMarkupsJsonElement* markupsObject)
{
  vtkMRMLMarkupsShapeNode* shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(markupsNode);
  if (!shapeNode)
    {
    vtkErrorWithObjectMacro(this, "vtkMRMLMarkupsShapeJsonStorageNode::UpdateMarkupsNodeFromJsonValue failed: invalid markupsNode");
    return false;
    }

  MRMLNodeModifyBlocker blocker(shapeNode);

  if (markupsObject->HasMember("shapeName"))
    {
    shapeNode->SetShapeName(shapeNode->GetShapeNameFromString(markupsObject->GetStringProperty("shapeName").c_str()));
    }
  if (markupsObject->HasMember("radiusMode"))
    {
    shapeNode->SetRadiusMode(shapeNode->GetRadiusModeFromString(markupsObject->GetStringProperty("radiusMode").c_str()));
    }
  if (markupsObject->HasMember("drawMode2D"))
    {
    shapeNode->SetDrawMode2D(shapeNode->GetDrawMode2DFromString(markupsObject->GetStringProperty("drawMode2D").c_str()));
    }
  double resolution = 0.0;
  if (markupsObject->GetDoubleProperty("resolution", resolution))
    {
    shapeNode->SetResolution(resolution);
    }

  /* 
   * Reimplemented to observe JumpToPointEvent.
   * On loading a save scene, this is needed to track active control point;
   * used to reslice a Tube at active control point.
  */
  shapeNode->CreateDefaultDisplayNodes();
  
  return Superclass::UpdateMarkupsNodeFromJsonValue(markupsNode, markupsObject);
}
