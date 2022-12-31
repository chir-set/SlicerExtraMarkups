/*==============================================================================

  Copyright (c) The Intervention Centre
  Oslo University Hospital, Oslo, Norway. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Rafael Palomar (The Intervention Centre,
  Oslo University Hospital) and was supported by The Research Council of Norway
  through the ALive project (grant nr. 311393).

==============================================================================*/

#include "vtkMRMLMarkupsShapeNode.h"
#include "vtkMRMLMeasurementShape.h"
#include "vtkMRMLMarkupsShapeJsonStorageNode.h"
#include "vtkMRMLMarkupsDisplayNode.h"

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkMRMLSliceNode.h>
#include <vtkCollection.h>
#include <vtkCallbackCommand.h>
#include <vtkMRMLScene.h>

//--------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsShapeNode);

//--------------------------------------------------------------------------------
vtkMRMLMarkupsShapeNode::vtkMRMLMarkupsShapeNode()
{
  this->SetShapeName(Sphere);
  
  this->OnPointPositionUndefinedCallback = vtkSmartPointer<vtkCallbackCommand>::New();
  this->OnPointPositionUndefinedCallback->SetClientData( reinterpret_cast<void *>(this) );
  this->OnPointPositionUndefinedCallback->SetCallback( vtkMRMLMarkupsShapeNode::OnPointPositionUndefined );
  this->AddObserver(vtkMRMLMarkupsNode::PointPositionUndefinedEvent, this->OnPointPositionUndefinedCallback);
  
  this->OnJumpToPointCallback = vtkSmartPointer<vtkCallbackCommand>::New();
  this->OnJumpToPointCallback->SetClientData( reinterpret_cast<void *>(this) );
  this->OnJumpToPointCallback->SetCallback( vtkMRMLMarkupsShapeNode::OnJumpToPoint );
}

//--------------------------------------------------------------------------------
vtkMRMLMarkupsShapeNode::~vtkMRMLMarkupsShapeNode()
{
  this->RemoveObserver(this->OnPointPositionUndefinedCallback);
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkMRMLStorageNode* vtkMRMLMarkupsShapeNode::CreateDefaultStorageNode()
{
  vtkMRMLScene* scene = this->GetScene();
  if (scene == nullptr)
  {
    vtkErrorMacro("CreateDefaultStorageNode failed: scene is invalid");
    return nullptr;
  }
  return vtkMRMLStorageNode::SafeDownCast(
    scene->CreateNodeByClass("vtkMRMLMarkupsShapeJsonStorageNode"));
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::CreateDefaultDisplayNodes()
{
  vtkMRMLScene* scene = this->GetScene();
  if (scene == nullptr)
  {
    vtkErrorMacro("CreateDefaultDisplayNodes failed: scene is invalid");
    return;
  }
  vtkMRMLMarkupsNode::CreateDefaultDisplayNodes();
  /*
   * This function gets called twice on creation.
   * this->GetDisplayNode()->HasObserver(vtkMRMLMarkupsDisplayNode::JumpToPointEvent, this->OnJumpToPointCallback)
   * does not help. Using one-time flag DisplayNodeObserved.
   */
  if (this->GetDisplayNode() && !this->DisplayNodeObserved)
  {
    this->GetDisplayNode()->AddObserver(vtkMRMLMarkupsDisplayNode::JumpToPointEvent, this->OnJumpToPointCallback);
    this->DisplayNodeObserved = true;
  }
}

//----------------------------------------------------------------------------
const char* vtkMRMLMarkupsShapeNode::GetShapeNameAsString(int shapeName)
{
  switch (shapeName)
  {
    case vtkMRMLMarkupsShapeNode::Sphere:
      return "Sphere";
    case vtkMRMLMarkupsShapeNode::Ring:
      return "Ring";
    case vtkMRMLMarkupsShapeNode::Disk:
      return "Disk";
    case vtkMRMLMarkupsShapeNode::Tube:
      return "Tube";
    case vtkMRMLMarkupsShapeNode::Cylinder:
      return "Cylinder";
    case vtkMRMLMarkupsShapeNode::Cone:
      return "Cone";
    default:
      break;
  }
  return "";
}

//-----------------------------------------------------------
int vtkMRMLMarkupsShapeNode::GetShapeNameFromString(const char* name)
{
  if (name == nullptr)
  {
    // invalid name
    return -1;
  }
  for (int i = 0; i < vtkMRMLMarkupsShapeNode::ShapeName_Last; i++)
  {
    if (strcmp(name, vtkMRMLMarkupsShapeNode::GetShapeNameAsString(i)) == 0)
    {
      // found a matching name
      return i;
    }
  }
  // unknown name
  return -1;
}

//----------------------------------------------------------------------------
const char* vtkMRMLMarkupsShapeNode::GetRadiusModeAsString(int radiusMode)
{
  switch (radiusMode)
  {
    case vtkMRMLMarkupsShapeNode::Centered:
      return "Centered";
    case vtkMRMLMarkupsShapeNode::Circumferential:
      return "Circumferential";
    default:
      break;
  }
  return "";
}

//-----------------------------------------------------------
int vtkMRMLMarkupsShapeNode::GetRadiusModeFromString(const char* mode)
{
  if (mode == nullptr)
  {
    // invalid name
    return -1;
  }
  for (int i = 0; i < vtkMRMLMarkupsShapeNode::RadiusMode_Last; i++)
  {
    if (strcmp(mode, vtkMRMLMarkupsShapeNode::GetRadiusModeAsString(i)) == 0)
    {
      // found a matching name
      return i;
    }
  }
  // unknown name
  return -1;
}

//----------------------------------------------------------------------------
const char* vtkMRMLMarkupsShapeNode::GetDrawMode2DAsString(int drawMode2D)
{
  switch (drawMode2D)
  {
    case vtkMRMLMarkupsShapeNode::Intersection:
      return "Intersection";
    case vtkMRMLMarkupsShapeNode::Projection:
      return "Projection";
    default:
      break;
  }
  return "";
}

//-----------------------------------------------------------
int vtkMRMLMarkupsShapeNode::GetDrawMode2DFromString(const char* mode)
{
  if (mode == nullptr)
  {
    // invalid name
    return -1;
  }
  for (int i = 0; i < vtkMRMLMarkupsShapeNode::DrawMode2D_Last; i++)
  {
    if (strcmp(mode, vtkMRMLMarkupsShapeNode::GetDrawMode2DAsString(i)) == 0)
    {
      // found a matching name
      return i;
    }
  }
  // unknown name
  return -1;
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::SetShapeName(int shapeName)
{
  this->ShapeName = shapeName;
  switch (shapeName)
  {
    case Sphere :
      this->RequiredNumberOfControlPoints = 2;
      this->MaximumNumberOfControlPoints = 2;
      this->ForceSphereMeasurements();
      break;
    case Ring:
      // Third point is used to calculate normal relative to the center in 3D view.
      this->RequiredNumberOfControlPoints = 3;
      this->MaximumNumberOfControlPoints = 3;
      this->ForceRingMeasurements();
      break;
    case Disk:
      // Point 0 : always the center.
      this->RequiredNumberOfControlPoints = 3;
      this->MaximumNumberOfControlPoints = 3;
      this->ForceDiskMeasurements();
      break;
    case Tube:
      /*
       * RequiredNumberOfControlPoints should be 4, but toolbar new control point button remains grayed for ever.
       * With -1, it still remains grayed, but hovering on a control point activates the new control point button.
       * 
       * N.B. - control points need not and are not required to be on the surface.
       * A control point pair merely defines a radius value and a middle point for the spline (centerline).
       * In practice, we would place points on the walls of a diseased artery, a short part of it.
       * All this is useless for healthy arteries : we have real structures using segmentation,
       * that do not have perfectly circular cross-sections.
       */
      this->RequiredNumberOfControlPoints = -1;
      this->MaximumNumberOfControlPoints = -1;
      this->ForceTubeMeasurements();
      break;
    case Cylinder:
      // Points 0 : centre at one end; point 2 : radius; point 3 : centre of the opposite end
      this->RequiredNumberOfControlPoints = 3;
      this->MaximumNumberOfControlPoints = 3;
      this->ForceCylinderMeasurements();
      break;
    case Cone:
      // Points 0 : centre of the base; point 2 : radius; point 3 : tip
      this->RequiredNumberOfControlPoints = 3;
      this->MaximumNumberOfControlPoints = 3;
      this->ForceConeMeasurements();
      break;
    default :
      vtkErrorMacro("Unknown shape.");
      return;
  };
  if (this->MaximumNumberOfControlPoints > 0)
  {
    while (this->GetNumberOfControlPoints() > this->MaximumNumberOfControlPoints)
    {
      this->RemoveNthControlPoint(this->MaximumNumberOfControlPoints);
    }
  }
  
  this->SplineWorld = nullptr;
  this->CappedTubeWorld = nullptr;

  this->Modified();
}

//---------------------- For disk shape -------------------------------------
bool vtkMRMLMarkupsShapeNode::DescribeDiskPointSpacing(double * closestPoint, double * farthestPoint,
                                                     double& innerRadius, double& outerRadius)
{
  if (this->ShapeName != Disk)
  {
    vtkErrorMacro("Current shape is not a disk.");
    return false;
  }
  if (this->GetNumberOfDefinedControlPoints(true) != 3)
  {
    return false;
  }
  double p1[3] = { 0.0 }; // center
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, p1);
  this->GetNthControlPointPositionWorld(1, p2);
  this->GetNthControlPointPositionWorld(2, p3);
  
  double distance2 = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  double distance3 = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p3));
  
  if (distance2 <= distance3)
  {
    closestPoint[0] = p2[0];
    closestPoint[1] = p2[1];
    closestPoint[2] = p2[2];
    farthestPoint[0] = p3[0];
    farthestPoint[1] = p3[1];
    farthestPoint[2] = p3[2];
    innerRadius = distance2;
    outerRadius = distance3;
  }
  else
  {
    closestPoint[0] = p3[0];
    closestPoint[1] = p3[1];
    closestPoint[2] = p3[2];
    farthestPoint[0] = p2[0];
    farthestPoint[1] = p2[1];
    farthestPoint[2] = p2[2];
    innerRadius = distance3;
    outerRadius = distance2;
  }
  return true;
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::FindLinearCoordinateByDistance(const double * p1, const double * p2,
                                                            double * result, const double difference)
{
  // Relative to p1, itself placed at origin. Simplifies reasoning.
  const double rp2[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
  const double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  const double factor = (1.0 + (difference / lineLength));
  result[0] = p1[0] + (rp2[0] * factor);
  result[1] = p1[1] + (rp2[1] * factor);
  result[2] = p1[2] + (rp2[2] * factor);
}

//----------------------------API only----------------------------------------
void vtkMRMLMarkupsShapeNode::SetRadius(double radius)
{
  if (this->ShapeName == Disk)
  {
    vtkErrorMacro("Current shape is a disk. Use SetInnerRadius and SetOuterRadius");
    return;
  }
  if (this->ShapeName == Tube)
  {
    vtkErrorMacro("Current shape is a tube. Use SetRadiusAtNthControlPoint.");
    return;
  }
  if (radius <= 0)
  {
    vtkErrorMacro("Radius must be greater than zero.");
    return;
  }
  double rasP1[3] = { 0.0 };
  double rasP2[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, rasP1);
  this->GetNthControlPointPositionWorld(1, rasP2);
  const double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(rasP1, rasP2));
  double currentRadius = 0.0;
  double difference = 0.0;
  if ((this->GetShapeName() != this->Cone) && (this->GetShapeName() != this->Cylinder))
  {
    currentRadius = (this->RadiusMode == Centered)
                                ? lineLength : lineLength / 2;
  }
  else
  {
    currentRadius = lineLength;
  }
  difference = radius - currentRadius;
  
  double rasP2Shifted[3] = { 0.0 };
  this->FindLinearCoordinateByDistance(rasP1, rasP2, rasP2Shifted, difference);
  
  this->SetNthControlPointPositionWorld(1, rasP2Shifted);
  if ((this->GetShapeName() != this->Cone) && (this->GetShapeName() != this->Cylinder))
  {
    // Don't move center, move p1.
    if (this->RadiusMode == Circumferential)
    {
      double rasP1Shifted[3] = { 0.0 };
      this->FindLinearCoordinateByDistance(rasP2, rasP1, rasP1Shifted, difference);
      this->SetNthControlPointPositionWorld(0, rasP1Shifted);
    }
    // Text actor does not move until mouse is hovered on a control point.
  }
}

// Merging all shapes introduces complexity, API becomes bad looking, with shape specific functions.
//----------------------------API only----------------------------------------
void vtkMRMLMarkupsShapeNode::SetInnerRadius(double radius)
{
  if (this->ShapeName != Disk)
  {
    vtkErrorMacro("Current shape is not a disk. Use SetRadius.");
    return;
  }
  if (radius <= 0)
  {
    vtkErrorMacro("Radius must be greater than zero.");
    return;
  }
  double closestPoint[3] = { 0.0 };
  double farthestPoint[3] = { 0.0 };
  double innerRadius = 0.0, outerRadius = 0.0;
  if (!this->DescribeDiskPointSpacing(closestPoint, farthestPoint, innerRadius, outerRadius))
  {
    vtkDebugMacro("Point proximity description failure.");
    return;
  }
  if (radius >= outerRadius)
  {
    vtkErrorMacro("Inner radius must be less than outer radius.");
    return;
  }
  double rasP1[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, rasP1);
  
  const double difference = radius - innerRadius;
  double closestPointShifted[3] = { 0.0 };
  this->FindLinearCoordinateByDistance(rasP1, closestPoint, closestPointShifted, difference);
  
  this->SetNthControlPointPositionWorld(this->GetClosestControlPointIndexToPositionWorld(closestPoint), closestPointShifted);
}

//----------------------------API only----------------------------------------
void vtkMRMLMarkupsShapeNode::SetOuterRadius(double radius)
{
  if (this->ShapeName != Disk)
  {
    vtkErrorMacro("Current shape is not a disk. Use SetRadius.");
    return;
  }
  if (radius <= 0)
  {
    vtkErrorMacro("Radius must be greater than zero.");
    return;
  }
  double closestPoint[3] = { 0.0 };
  double farthestPoint[3] = { 0.0 };
  double innerRadius = 0.0, outerRadius = 0.0;
  if (!this->DescribeDiskPointSpacing(closestPoint, farthestPoint, innerRadius, outerRadius))
  {
    vtkDebugMacro("Point proximity description failure.");
    return;
  }
  if (radius <= innerRadius)
  {
    vtkErrorMacro("Outer radius must be greater than inner radius.");
    return;
  }
  double rasP1[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, rasP1);
  
  const double difference = radius - outerRadius;
  double farthestPointShifted[3] = { 0.0 };
  this->FindLinearCoordinateByDistance(rasP1, farthestPoint, farthestPointShifted, difference);
  
  this->SetNthControlPointPositionWorld(this->GetClosestControlPointIndexToPositionWorld(farthestPoint), farthestPointShifted);
}

//----------------------------API only----------------------------------------
void vtkMRMLMarkupsShapeNode::SetHeight(double height)
{
  if ((this->ShapeName != this->Cone) && (this->GetShapeName() != this->Cylinder))
  {
    vtkErrorMacro("Current shape is not a Cone nor a Cylinder.");
    return;
  }
  if (height <= 0)
  {
    vtkErrorMacro("Height must be greater than zero.");
    return;
  }
  double rasP1[3] = { 0.0 };
  double rasP3[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, rasP1);
  this->GetNthControlPointPositionWorld(2, rasP3);
  const double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(rasP1, rasP3));
  const double difference = height - lineLength;
  double rasP3Shifted[3] = { 0.0 };
  this->FindLinearCoordinateByDistance(rasP1, rasP3, rasP3Shifted, difference);
  
  this->SetNthControlPointPositionWorld(2, rasP3Shifted);
}

//----------------------------API only----------------------------------------
void vtkMRMLMarkupsShapeNode::SetAperture(double aperture)
{
  if (this->ShapeName != this->Cone)
  {
    vtkErrorMacro("Current shape is not a Cone.");
    return;
  }
  if (aperture <= 0 || aperture >= 180)
  {
    vtkErrorMacro("Aperture must be greater than zero and less than 180.0.");
    return;
  }
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, p1);
  this->GetNthControlPointPositionWorld(1, p2);
  this->GetNthControlPointPositionWorld(2, p3);
  
  const double height = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p3));
  const double newRadius = std::tan(vtkMath::RadiansFromDegrees(aperture / 2.0)) * height;
  this->SetRadius(newRadius);
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ResliceToControlPoints()
{
  switch (this->ShapeName)
  {
    case Sphere :
      this->ResliceToLine();
      break;
    case Ring:
      this->ResliceToPlane();
      break;
    case Disk:
      this->ResliceToPlane();
      break;
    case Tube:
      break;
    case Cylinder:
      break;
    case Cone:
      break;
    default :
      vtkErrorMacro("Unknown shape.");
      return;
  };
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ResliceToPlane(int pointIndex1, int pointIndex2, int pointIndex3)
{
  if (!this->ResliceNode || this->GetNumberOfControlPoints() < 3
    || pointIndex1 < 0 || pointIndex2 < 0 || pointIndex3 < 0)
  {
    return;
  }
  vtkMRMLSliceNode * resliceNode = vtkMRMLSliceNode::SafeDownCast(this->ResliceNode);
  if (!resliceNode)
  {
    return;
  }
  double rasP1[3] = { 0.0 };
  double rasP2[3] = { 0.0 };
  double rasP3[3] = { 0.0 };
  double rasNormal[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(pointIndex1, rasP1);
  this->GetNthControlPointPositionWorld(pointIndex2, rasP2);
  this->GetNthControlPointPositionWorld(pointIndex3, rasP3);
  
  // Relative to rasP1 (center)
  double rRasP2[3] = { rasP2[0] - rasP1[0], rasP2[1] - rasP1[1], rasP2[2] - rasP1[2] };
  double rRasP3[3] = { rasP3[0] - rasP1[0], rasP3[1] - rasP1[1], rasP3[2] - rasP1[2] };
  
  vtkMath::Cross(rRasP2, rRasP3, rasNormal);
  if (rasNormal[0] == 0.0 && rasNormal[1] == 0.0 && rasNormal[2] == 0.0)
  {
    return;
  }
  resliceNode->SetSliceToRASByNTP(
    rasNormal[0], rasNormal[1], rasNormal[2],
    rasP2[0], rasP2[1], rasP2[2],
    rasP1[0], rasP1[1], rasP1[2],
    0);
  resliceNode->UpdateMatrices();
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ResliceToLine(int pointIndex1, int pointIndex2)
{
  if (!this->ResliceNode || this->GetNumberOfControlPoints() < 2
    || pointIndex1 < 0 || pointIndex2 < 0)
  {
    return;
  }
  vtkMRMLSliceNode * resliceNode = vtkMRMLSliceNode::SafeDownCast(this->ResliceNode);
  if (!resliceNode)
  {
    return;
  }
  double rasP1[3] = { 0.0 };
  double rasP2[3] = { 0.0 };
  double rasNormal[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(pointIndex1, rasP1);
  this->GetNthControlPointPositionWorld(pointIndex2, rasP2);
  
  vtkMath::Cross(rasP1, rasP2, rasNormal);
  if (rasNormal[0] == 0.0 && rasNormal[1] == 0.0 && rasNormal[2] == 0.0)
  {
    return;
  }
  resliceNode->SetSliceToRASByNTP(
    rasNormal[0], rasNormal[1], rasNormal[2],
    rasP2[0], rasP2[1], rasP2[2],
    rasP1[0], rasP1[1], rasP1[2],
    0);
  resliceNode->UpdateMatrices();
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceDiskMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddMeasurement("innerRadius", true);
  this->AddMeasurement("outerRadius", true);
  this->AddMeasurement("width", false);
  this->AddAreaMeasurement("area");
  this->AddAreaMeasurement("innerArea");
  this->AddAreaMeasurement("outerArea");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceRingMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddMeasurement("radius", true);
  this->AddAreaMeasurement("area");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceSphereMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddMeasurement("radius", true);
  this->AddAreaMeasurement("area");
  this->AddVolumeMeasurement("volume");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceTubeMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddAreaMeasurement("area");
  this->AddVolumeMeasurement("volume", true);
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceCylinderMeasurements()
{
  this->RemoveAllMeasurements();

  this->AddMeasurement("radius", true);
  this->AddMeasurement("height", true);
  this->AddAreaMeasurement("area");
  this->AddVolumeMeasurement("volume");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceConeMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddMeasurement("radius", true);
  this->AddMeasurement("height", true);
  this->AddMeasurement("slant");
  this->AddMeasurement("aperture", false, "%-#4.4g %s", "°");
  this->AddAreaMeasurement("area");
  this->AddVolumeMeasurement("volume");
}

//----------------------------------------------------------------------------
/*
 * Tube : remove an adjacent point.
 * Toggling a point status in the markups module complicates things, don't react here.
 */
void vtkMRMLMarkupsShapeNode::OnPointPositionUndefined(vtkObject* caller, unsigned long event, void* clientData, void* callData)
{
  vtkMRMLMarkupsShapeNode * client = reinterpret_cast<vtkMRMLMarkupsShapeNode*>(clientData);
  if (!client || client->GetShapeName() != Tube
      || client->GetNumberOfUndefinedControlPoints() > 0)
  {
    return;
  }
  
  if (client->RemovingPairControlPoint || client->GetNumberOfControlPoints() == 0)
  {
    // Point removal was triggered by this function, not in UI.
    client->RemovingPairControlPoint = false;
    return;
  }
  
  const int removedIndex = *(static_cast<int*> (callData));
  if ((removedIndex % 2) == 0 )
  {
    // Already removed point in UI is not last point of an uneven number of points.
    if (client->GetNumberOfControlPoints() > removedIndex)
    {
      client->RemovingPairControlPoint = true;
      client->RemoveNthControlPoint(removedIndex);
    }
    else
    {
      // If the last point of an uneven number of points is removed in UI, there is no next point to remove.
      client->RemovingPairControlPoint = false;
    }
  }
  else 
  {
    client->RemovingPairControlPoint = true;
    client->RemoveNthControlPoint(removedIndex - 1);
  }
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::OnJumpToPoint(vtkObject* caller, unsigned long event, void* clientData, void* callData)
{
  vtkMRMLMarkupsShapeNode * client = reinterpret_cast<vtkMRMLMarkupsShapeNode*>(clientData);
  if (!client || client->GetNumberOfUndefinedControlPoints() > 0)
  {
    return;
  }
  vtkMRMLMarkupsDisplayNode * displayNode = vtkMRMLMarkupsDisplayNode::SafeDownCast(client->GetDisplayNode());
  if (!displayNode)
  {
    return;
  }
  client->ActiveControlPoint = displayNode->GetActiveControlPoint();
}

//----------------------------------------------------------------------------
double vtkMRMLMarkupsShapeNode::GetRadiusAtNthControlPoint(int n)
{
  if (this->GetShapeName() != Tube)
  {
    vtkErrorMacro("Not a Tube shape.");
    return -1.0;
  }
  if (n < 0
    || this->GetNumberOfUndefinedControlPoints() > 0
    || this->GetNumberOfDefinedControlPoints() < 4
    || (this->GetNumberOfDefinedControlPoints() % 2) != 0
    || n >= this->GetNumberOfDefinedControlPoints())
  {
    vtkErrorMacro("Tube shape has undefined control points, or odd number of control points,"
                  " or less than 4 control points, or fewer control points than requested.");
    return -1.0;
  }
  double radius = 0.0;
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  if ((n % 2) == 0)
  {
    this->GetNthControlPointPositionWorld(n, p1);
    this->GetNthControlPointPositionWorld(n + 1, p2);
  }
  else {
    this->GetNthControlPointPositionWorld(n, p2);
    this->GetNthControlPointPositionWorld(n - 1, p1);
  }
  radius = (std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2))) / 2.0;
  return radius;
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::SetRadiusAtNthControlPoint(int n, double radius)
{
  if (radius <= 0.0)
  {
    vtkErrorMacro("Requested radius must be greater than 0.0.");
    return;
  }
  double currentRadius = this->GetRadiusAtNthControlPoint(n);
  if (currentRadius <= 0)
  {
    return;
  }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  if ((n % 2) == 0)
  {
    this->GetNthControlPointPositionWorld(n, p1);
    this->GetNthControlPointPositionWorld(n + 1, p2);
  }
  else {
    this->GetNthControlPointPositionWorld(n, p2);
    this->GetNthControlPointPositionWorld(n - 1, p1);
  }
  const double middlePoint[3] = { (p1[0] + p2[0]) / 2.0,
                            (p1[1] + p2[1]) / 2.0,
                            (p1[2] + p2[2]) / 2.0};
  double p1New[3] = { 0.0 };
  double p2New[3] = { 0.0 };
  const double radiusDifference = radius - currentRadius;
  this->FindLinearCoordinateByDistance(middlePoint, p1, p1New, radiusDifference);
  this->FindLinearCoordinateByDistance(middlePoint, p2, p2New, radiusDifference);
  
  if ((n % 2) == 0)
  {
    this->SetNthControlPointPositionWorld(n, p1New);
    this->SetNthControlPointPositionWorld(n + 1, p2New);
  }
  else
  {
    this->SetNthControlPointPositionWorld(n, p2New);
    this->SetNthControlPointPositionWorld(n - 1, p1New);
  }
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::AddMeasurement(const char* name, bool enabled,
                                                   const char* format, const char* units)
{
  vtkNew<vtkMRMLMeasurementShape> measurement;
  measurement->SetName(name);
  measurement->SetUnits(units);
  measurement->SetPrintFormat(format);
  measurement->SetInputMRMLNode(this);
  measurement->SetEnabled(enabled);
  this->Measurements->AddItem(measurement);
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::AddAreaMeasurement(const char* name, bool enabled,
                                                 const char* format, double coefficient, const char* units)
{
  vtkNew<vtkMRMLMeasurementShape> measurement;
  measurement->SetName(name);
  measurement->SetUnits(units);
  measurement->SetDisplayCoefficient(coefficient);
  measurement->SetPrintFormat(format);
  measurement->SetInputMRMLNode(this);
  measurement->SetEnabled(enabled);
  this->Measurements->AddItem(measurement);
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::AddVolumeMeasurement(const char* name, bool enabled, const char* format, double coefficient, const char* units)
{
  vtkNew<vtkMRMLMeasurementShape> measurement;
  measurement->SetName(name);
  measurement->SetUnits(units);
  measurement->SetDisplayCoefficient(coefficient);
  measurement->SetPrintFormat(format);
  measurement->SetInputMRMLNode(this);
  measurement->SetEnabled(enabled);
  this->Measurements->AddItem(measurement);
}
