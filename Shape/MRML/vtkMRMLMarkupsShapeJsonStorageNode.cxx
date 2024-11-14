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
  writer->WriteBoolProperty("displayCappedTube", shapeNode->GetDisplayCappedTube());
  writer->WriteBoolProperty("scalarVisibility", shapeNode->GetScalarVisibility());
  // Ignoring shapeNode->ResliceNode.
  
  writer->WriteDoubleProperty("parametricN", shapeNode->GetParametricN());
  writer->WriteDoubleProperty("parametricN1", shapeNode->GetParametricN1());
  writer->WriteDoubleProperty("parametricN2", shapeNode->GetParametricN2());
  writer->WriteDoubleProperty("parametricRadius", shapeNode->GetParametricRadius());
  writer->WriteDoubleProperty("parametricRingRadius", shapeNode->GetParametricRingRadius());
  writer->WriteDoubleProperty("parametricCrossSectionRadius", shapeNode->GetParametricCrossSectionRadius());
  writer->WriteDoubleProperty("parametricMinimumU", shapeNode->GetParametricMinimumU());
  writer->WriteDoubleProperty("parametricMaximumU", shapeNode->GetParametricMaximumU());
  writer->WriteDoubleProperty("parametricMinimumV", shapeNode->GetParametricMinimumV());
  writer->WriteDoubleProperty("parametricMaximumV", shapeNode->GetParametricMaximumV());
  writer->WriteDoubleProperty("parametricMinimumW", shapeNode->GetParametricMinimumW());
  writer->WriteDoubleProperty("parametricMaximumW", shapeNode->GetParametricMaximumW());
  writer->WriteBoolProperty("parametricJoinU", shapeNode->GetParametricJoinU());
  writer->WriteBoolProperty("parametricJoinV", shapeNode->GetParametricJoinV());
  writer->WriteBoolProperty("parametricJoinW", shapeNode->GetParametricJoinW());
  writer->WriteBoolProperty("parametricTwistU", shapeNode->GetParametricTwistU());
  writer->WriteBoolProperty("parametricTwistV", shapeNode->GetParametricTwistV());
  writer->WriteBoolProperty("parametricTwistW", shapeNode->GetParametricTwistW());
  writer->WriteBoolProperty("parametricClockwiseOrdering", shapeNode->GetParametricClockwiseOrdering());
  
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
  if (markupsObject->HasMember("displayCappedTube"))
  {
    bool displayCappedTube = markupsObject->GetBoolProperty("displayCappedTube");
    shapeNode->SetDisplayCappedTube(displayCappedTube);
  }
  if (markupsObject->HasMember("scalarVisibility"))
  {
    bool scalarVisibility = markupsObject->GetBoolProperty("scalarVisibility");
    shapeNode->SetScalarVisibility(scalarVisibility);
  }

  double parametricN = 2.0;
  if (markupsObject->GetDoubleProperty("parametricN", parametricN))
  {
    shapeNode->SetParametricN(parametricN);
  }
  double parametricN1 = 1.0;
  if (markupsObject->GetDoubleProperty("parametricN1", parametricN1))
  {
    shapeNode->SetParametricN1(parametricN1);
  }
  double parametricN2 = 1.0;
  if (markupsObject->GetDoubleProperty("parametricN2", parametricN2))
  {
    shapeNode->SetParametricN2(parametricN2);
  }
  double parametricRadius = 1.0;
  if (markupsObject->GetDoubleProperty("parametricRadius", parametricRadius))
  {
    shapeNode->SetParametricRadius(parametricRadius);
  }
  double parametricRingRadius = 1.0;
  if (markupsObject->GetDoubleProperty("parametricRingRadius", parametricRingRadius))
  {
    shapeNode->SetParametricRingRadius(parametricRingRadius);
  }
  double parametricCrossSectionRadius = 0.5;
  if (markupsObject->GetDoubleProperty("parametricCrossSectionRadius", parametricCrossSectionRadius))
  {
    shapeNode->SetParametricCrossSectionRadius(parametricCrossSectionRadius);
  }
  double parametricMinimumU = 0.0;
  if (markupsObject->GetDoubleProperty("parametricMinimumU", parametricMinimumU))
  {
    shapeNode->SetParametricMinimumU(parametricMinimumU);
  }
  double parametricMaximumU = 0.0;
  if (markupsObject->GetDoubleProperty("parametricMaximumU", parametricMaximumU))
  {
    shapeNode->SetParametricMaximumU(parametricMaximumU);
  }
  double parametricMinimumV = 0.0;
  if (markupsObject->GetDoubleProperty("parametricMinimumV", parametricMinimumV))
  {
    shapeNode->SetParametricMinimumV(parametricMinimumV);
  }
  double parametricMaximumV = 0.0;
  if (markupsObject->GetDoubleProperty("parametricMaximumV", parametricMaximumV))
  {
    shapeNode->SetParametricMaximumV(parametricMaximumV);
  }
  double parametricMinimumW = 0.0;
  if (markupsObject->GetDoubleProperty("parametricMinimumW", parametricMinimumW))
  {
    shapeNode->SetParametricMinimumW(parametricMinimumW);
  }
  double parametricMaximumW = 0.0;
  if (markupsObject->GetDoubleProperty("parametricMaximumW", parametricMaximumW))
  {
    shapeNode->SetParametricMaximumW(parametricMaximumW);
  }
  if (markupsObject->HasMember("parametricJoinU"))
  {
    bool parametricJoinU = markupsObject->GetBoolProperty("parametricJoinU");
    shapeNode->SetParametricJoinU(parametricJoinU);
  }
  if (markupsObject->HasMember("parametricJoinV"))
  {
    bool parametricJoinV = markupsObject->GetBoolProperty("parametricJoinV");
    shapeNode->SetParametricJoinV(parametricJoinV);
  }
  if (markupsObject->HasMember("parametricJoinW"))
  {
    bool parametricJoinW = markupsObject->GetBoolProperty("parametricJoinW");
    shapeNode->SetParametricJoinW(parametricJoinW);
  }
  if (markupsObject->HasMember("parametricTwistU"))
  {
    bool parametricTwistU = markupsObject->GetBoolProperty("parametricTwistU");
    shapeNode->SetParametricTwistU(parametricTwistU);
  }
  if (markupsObject->HasMember("parametricTwistV"))
  {
    bool parametricTwistV = markupsObject->GetBoolProperty("parametricTwistV");
    shapeNode->SetParametricTwistV(parametricTwistV);
  }
  if (markupsObject->HasMember("parametricTwistW"))
  {
    bool parametricTwistW = markupsObject->GetBoolProperty("parametricTwistW");
    shapeNode->SetParametricTwistW(parametricTwistW);
  }
  if (markupsObject->HasMember("parametricClockwiseOrdering"))
  {
    bool parametricClockwiseOrdering = markupsObject->GetBoolProperty("parametricClockwiseOrdering");
    shapeNode->SetParametricClockwiseOrdering(parametricClockwiseOrdering);
  }
  
  /* 
   * Reimplemented to observe JumpToPointEvent.
   * On loading a save scene, this is needed to track an active control point;
   * used to reslice a Tube at the active control point.
  */
  shapeNode->CreateDefaultDisplayNodes();
  
  return Superclass::UpdateMarkupsNodeFromJsonValue(markupsNode, markupsObject);
}
