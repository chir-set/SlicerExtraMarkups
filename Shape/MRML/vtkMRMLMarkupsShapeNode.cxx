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
#include <vtkPolyLineSource.h>
#include <vtkPointData.h>

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
    case vtkMRMLMarkupsShapeNode::Arc:
      return "Arc";
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
    case Arc:
      // Points 0 : centre; point 2 : radius and polar vector; point 3 : normal and angle
      this->RequiredNumberOfControlPoints = 3;
      this->MaximumNumberOfControlPoints = 3;
      this->ForceArcMeasurements();
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
  vtkMath::GetPointAlongLine(rasP2Shifted, rasP1, rasP2, difference);
  
  this->SetNthControlPointPositionWorld(1, rasP2Shifted);
  if ((this->GetShapeName() != this->Cone) && (this->GetShapeName() != this->Cylinder))
  {
    // Don't move center, move p1.
    if (this->RadiusMode == Circumferential)
    {
      double rasP1Shifted[3] = { 0.0 };
      vtkMath::GetPointAlongLine(rasP1Shifted, rasP2, rasP1, difference);
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
  vtkMath::GetPointAlongLine(closestPointShifted, rasP1, closestPoint, difference);
  
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
  vtkMath::GetPointAlongLine(farthestPointShifted, rasP1, farthestPoint, difference);
  
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
  vtkMath::GetPointAlongLine(rasP3Shifted, rasP1, rasP3, difference);
  
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
      if (this->SnapNthControlPointToTubeSurface(this->ActiveControlPoint, false))
      {
        this->ResliceToTubeCrossSection(this->ActiveControlPoint);
      }
      break;
    case Cylinder:
      this->ResliceToPlane();
      break;
    case Cone:
      this->ResliceToPlane();
      break;
    case Arc:
      this->ResliceToPlane();
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
void vtkMRMLMarkupsShapeNode::ForceArcMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddMeasurement("angle", true, "%-#4.4g %s", "°");
  this->AddMeasurement("radius");
  this->AddAreaMeasurement("area");
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
  
  if (client->RemovingPairControlPoint
    || client->GetNumberOfControlPoints() == 0
    || !callData)
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
double vtkMRMLMarkupsShapeNode::GetNthControlPointRadius(int n)
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
void vtkMRMLMarkupsShapeNode::SetNthControlPointRadius(int n, double radius)
{
  if (radius <= 0.0)
  {
    vtkErrorMacro("Requested radius must be greater than 0.0.");
    return;
  }
  double currentRadius = this->GetNthControlPointRadius(n);
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
  double middlePoint[3] = { (p1[0] + p2[0]) / 2.0,
                            (p1[1] + p2[1]) / 2.0,
                            (p1[2] + p2[2]) / 2.0};
  double p1New[3] = { 0.0 };
  double p2New[3] = { 0.0 };
  const double radiusDifference = radius - currentRadius;
  vtkMath::GetPointAlongLine(p1New, middlePoint, p1, radiusDifference);
  vtkMath::GetPointAlongLine(p2New, middlePoint, p2, radiusDifference);
  
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
bool vtkMRMLMarkupsShapeNode::SnapNthControlPointToTubeSurface(int pointIndex, bool bypassLockedState)
{
  if (this->GetShapeName() != Tube)
  {
    vtkErrorMacro("Not a Tube shape.");
    return false;
  }
  if (!bypassLockedState && this->GetLocked())
  {
    vtkErrorMacro("Markups node is locked, aborting.");
    return false;
  }
  if (pointIndex < 0
    || this->GetNumberOfUndefinedControlPoints() > 0
    || this->GetNumberOfDefinedControlPoints() < 4
    || (this->GetNumberOfDefinedControlPoints() % 2) != 0
    || pointIndex >= this->GetNumberOfDefinedControlPoints())
  {
    vtkErrorMacro("Tube shape has undefined control points, or odd number of control points,"
    " or less than 4 control points, or fewer control points than requested.");
    return false;
  }
  const double radius = this->GetNthControlPointRadius(pointIndex);
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  if ((pointIndex % 2) == 0)
  {
    this->GetNthControlPointPositionWorld(pointIndex, p1);
    this->GetNthControlPointPositionWorld(pointIndex + 1, p2);
  }
  else {
    this->GetNthControlPointPositionWorld(pointIndex, p2);
    this->GetNthControlPointPositionWorld(pointIndex - 1, p1);
  }
  
  // Middle point between p1 and p2.
  double middlePoint[3] = { (p1[0] + p2[0]) / 2.0,
                          (p1[1] + p2[1]) / 2.0,
                          (p1[2] + p2[2]) / 2.0};
  double splineMiddlePoint[3] = { 0.0 };
  double splineMiddlePointNeighbour[3] = { 0.0 };
  vtkIdType id = this->SplineWorld->FindPoint(middlePoint);
  vtkIdType idNeighbour = (id == this->SplineWorld->GetNumberOfPoints() - 1) // Last point
                        ? id -1
                        : id + 1;
  // Closest point on spline to calculated middle point. Use this one onwards.
  // NB : if the spline is a ball of wool, result is not predictable.
  this->SplineWorld->GetPoint(id, splineMiddlePoint);
  this->SplineWorld->GetPoint(idNeighbour, splineMiddlePointNeighbour);
  // Put splineMiddlePoint at origin.
  double rSplineMiddlePointNeighbour[3] = { splineMiddlePointNeighbour[0] - splineMiddlePoint[0],
                                          splineMiddlePointNeighbour[1] - splineMiddlePoint[1],
                                          splineMiddlePointNeighbour[2] - splineMiddlePoint[2]};
  double rPerpendicular1[3] = { 0.0 };
  double rPerpendicular2[3] = { 0.0 };
  // Perpendiculars at origin.
  vtkMath::Perpendiculars(rSplineMiddlePointNeighbour, rPerpendicular1, rPerpendicular2, 0.0);
  // First perpendicular at splineMiddlePoint.
  double perpendicular1[3] = { splineMiddlePoint[0] + rPerpendicular1[0],
                              splineMiddlePoint[1] + rPerpendicular1[1],
                              splineMiddlePoint[2] + rPerpendicular1[2]};
  // This is usually 1.0 mm.
  const double distance = std::sqrt(vtkMath::Distance2BetweenPoints(splineMiddlePoint, perpendicular1));
  double newP1[3] = { 0.0 };
  double newP2[3] = { 0.0 };
  // radius may be less than distance.
  vtkMath::GetPointAlongLine(newP1, splineMiddlePoint, perpendicular1, radius - distance);
  vtkMath::GetPointAlongLine(newP2, newP1, splineMiddlePoint, radius);
  if ((pointIndex % 2) == 0)
  {
    this->SetNthControlPointPositionWorld(pointIndex, newP1);
    this->SetNthControlPointPositionWorld(pointIndex + 1, newP2);
  }
  else {
    this->SetNthControlPointPositionWorld(pointIndex, newP2);
    this->SetNthControlPointPositionWorld(pointIndex - 1, newP1);
  }
  return true;
}

//--------------------------- API only ---------------------------------------
bool vtkMRMLMarkupsShapeNode::SnapAllControlPointsToTubeSurface(bool bypassLockedState)
{
  if (this->GetShapeName() != Tube)
  {
    vtkErrorMacro("Not a Tube shape.");
    return false;
  }
  if (!bypassLockedState && this->GetLocked())
  {
    vtkErrorMacro("Markups node is locked, aborting.");
    return false;
  }
  if (this->GetNumberOfUndefinedControlPoints() > 0
    || this->GetNumberOfDefinedControlPoints() < 4
    || (this->GetNumberOfDefinedControlPoints() % 2) != 0)
  {
    vtkErrorMacro("Tube shape has undefined control points, or odd number of control points,"
    " or less than 4 control points.");
    return false;
  }
  for (int i = 0; i < this->GetNumberOfDefinedControlPoints(); i = i + 2)
  {
    this->SnapNthControlPointToTubeSurface(i, true);
  }
  return true;
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ResliceToTubeCrossSection(int pointIndex)
{
  
  if (this->GetShapeName() != Tube)
  {
    vtkErrorMacro("Not a Tube shape.");
    return;
  }
  if (!this->ResliceNode)
  {
    return;
  }
  vtkMRMLSliceNode * resliceNode = vtkMRMLSliceNode::SafeDownCast(this->ResliceNode);
  if (!resliceNode)
  {
    return;
  }
  if (pointIndex < 0
    || this->GetNumberOfUndefinedControlPoints() > 0
    || this->GetNumberOfDefinedControlPoints() < 4
    || (this->GetNumberOfDefinedControlPoints() % 2) != 0
    || pointIndex >= this->GetNumberOfDefinedControlPoints())
  {
    vtkErrorMacro("Tube shape has undefined control points, or odd number of control points,"
    " or less than 4 control points, or fewer control points than requested.");
    return;
  }
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  if ((pointIndex % 2) == 0)
  {
    this->GetNthControlPointPositionWorld(pointIndex, p1);
    this->GetNthControlPointPositionWorld(pointIndex + 1, p2);
  }
  else {
    this->GetNthControlPointPositionWorld(pointIndex, p2);
    this->GetNthControlPointPositionWorld(pointIndex - 1, p1);
  }
  
  // Middle point between p1 and p2.
  double middlePoint[3] = { (p1[0] + p2[0]) / 2.0,
                        (p1[1] + p2[1]) / 2.0,
                        (p1[2] + p2[2]) / 2.0};
  double splineMiddlePoint[3] = { 0.0 };
  double splineMiddlePointNeighbour[3] = { 0.0 };
  vtkIdType id = this->SplineWorld->FindPoint(middlePoint);
  vtkIdType idNeighbour = (id == this->SplineWorld->GetNumberOfPoints() - 1) // Last point
                        ? id -1
                        : id + 1;
  // Closest point on spline to calculated middle point. Use this one onwards.
  // NB : if the spline is a ball of wool, result is not predictable.
  this->SplineWorld->GetPoint(id, splineMiddlePoint);
  this->SplineWorld->GetPoint(idNeighbour, splineMiddlePointNeighbour);
  // Put splineMiddlePoint at origin.
  double rSplineMiddlePointNeighbour[3] = { splineMiddlePointNeighbour[0] - splineMiddlePoint[0],
                                          splineMiddlePointNeighbour[1] - splineMiddlePoint[1],
                                          splineMiddlePointNeighbour[2] - splineMiddlePoint[2]};
  double rPerpendicular1[3] = { 0.0 };
  double rPerpendicular2[3] = { 0.0 };
  // Perpendiculars at origin.
  vtkMath::Perpendiculars(rSplineMiddlePointNeighbour, rPerpendicular1, rPerpendicular2, 0.0);

  resliceNode->SetSliceToRASByNTP(
    rSplineMiddlePointNeighbour[0], rSplineMiddlePointNeighbour[1], rSplineMiddlePointNeighbour[2],
    rPerpendicular1[0], rPerpendicular1[1], rPerpendicular1[2],
    splineMiddlePoint[0], splineMiddlePoint[1], splineMiddlePoint[2],
    0);
  resliceNode->UpdateMatrices();
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeNode::GetTrimmedSplineWorld(vtkPolyData * trimmedSpline,
                                                    int numberOfPointsToTrimAtStart, int numberOfPointsToTrimAtEnd)
{
  /*
   * At each end, the tube is bell shaped. This is apparent when any tube end is made very large.
   * This is a problem for modules that rely on radii along the spline. They expect a smooth radius variation.
   * Provide a spline that is trimmed at both ends by the specified number of points.
   */
  if (trimmedSpline == nullptr)
  {
    vtkErrorMacro("Trimmed spline parameter must not be NULL.");
    return false;
  }
  if (this->GetShapeName() != Tube)
  {
    vtkErrorMacro("Not a Tube shape.");
    return false;
  }
  vtkPoints * splinePoints = this->SplineWorld->GetPoints();
  const int numberOfSplinePoints = splinePoints->GetNumberOfPoints();
  if (numberOfPointsToTrimAtStart < 0
    || numberOfPointsToTrimAtEnd < 0
    || (numberOfPointsToTrimAtStart + numberOfPointsToTrimAtEnd) > (numberOfSplinePoints - 3))
  {
    vtkErrorMacro("Any number of points to trim must be greater than zero and there must remain at least 3 points.");
    return false;
  }
  
  vtkNew<vtkPolyLineSource> polyLineSpline;
  polyLineSpline->SetNumberOfPoints(numberOfSplinePoints - (numberOfPointsToTrimAtStart + numberOfPointsToTrimAtEnd));
  vtkIdType id = 0;
  for (int i = numberOfPointsToTrimAtStart; i < numberOfSplinePoints - numberOfPointsToTrimAtEnd; i++)
  {
    double p[3] = { 0.0 };
    this->SplineWorld->GetPoint(i, p);
    polyLineSpline->SetPoint(id, p[0], p[1], p[2]);
    id++;
  }
  polyLineSpline->Update();
  
  trimmedSpline->Initialize();
  trimmedSpline->DeepCopy(polyLineSpline->GetOutput());
  
  vtkDoubleArray * splineRadiusArray = vtkDoubleArray::SafeDownCast(this->SplineWorld->GetPointData()->GetArray("TubeRadius"));
  vtkSmartPointer<vtkDoubleArray> trimmedRadiusArray = vtkSmartPointer<vtkDoubleArray>::New();
  trimmedRadiusArray->SetName(splineRadiusArray->GetName());
  for (int i = numberOfPointsToTrimAtStart; i < numberOfSplinePoints - numberOfPointsToTrimAtEnd; i++)
  {
    trimmedRadiusArray->InsertNextValue(splineRadiusArray->GetValue(i));
  }
  trimmedSpline->GetPointData()->AddArray(trimmedRadiusArray);
  
  return true;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeNode::GetCenterWorld(double center[3])
{
  if (this->ShapeName == Tube || this->ShapeName == Cylinder)
  {
    vtkErrorMacro("GetCenterWorld is not implemented for Tube and Cylinder shapes.");
    return false;
  }
  if (this->GetNumberOfUndefinedControlPoints() || this->GetNumberOfUndefinedControlPoints(true))
  {
    vtkErrorMacro("Aborting because Shape node has undefined points.");
    return false;
  }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  switch (this->ShapeName)
  {
    case Sphere:
      if (this->GetNumberOfDefinedControlPoints() != 2)
      {
        vtkErrorMacro("Shape::Sphere node does not have 2 defined control points.");
        return false;
      }
      if (this->RadiusMode == Centered)
      {
        this->GetNthControlPointPositionWorld(0, center);
      }
      else
      {
        this->GetNthControlPointPositionWorld(0, p1);
        this->GetNthControlPointPositionWorld(1, p2);
        for (int i = 0; i < 3; i++)
        {
          center[i] = (p1[i] + p2[i]) / 2.0;
        }
      }
      break;
    case Ring:
      if (this->GetNumberOfDefinedControlPoints() != 3)
      {
        vtkErrorMacro("Shape::Ring node does not have 3 defined control points.");
        return false;
      }
      if (this->RadiusMode == Centered)
      {
        this->GetNthControlPointPositionWorld(0, center);
      }
      else
      {
        this->GetNthControlPointPositionWorld(0, p1);
        this->GetNthControlPointPositionWorld(1, p2);
        for (int i = 0; i < 3; i++)
        {
          center[i] = (p1[i] + p2[i]) / 2.0;
        }
      }
      break;
    case Disk:
      if (this->GetNumberOfDefinedControlPoints() != 3)
      {
        vtkErrorMacro("Shape::Disk node does not have 3 defined control points.");
        return false;
      }
      this->GetNthControlPointPositionWorld(0, center);
      break;
    case Cone:
      // Centre of mass : a quarter distance from base centre to tip.
      if (this->GetNumberOfDefinedControlPoints() != 3)
      {
        vtkErrorMacro("Shape::Cone node does not have 3 defined control points.");
        return false;
      }
      {
        this->GetNthControlPointPositionWorld(0, p1);
        this->GetNthControlPointPositionWorld(2, p3);
        const double height = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p3));
        vtkMath::GetPointAlongLine(center, p1, p3, -height * 0.75);
      }
      break;
    case Arc:
      if (this->GetNumberOfDefinedControlPoints() != 3)
      {
        vtkErrorMacro("Shape::Arc node does not have 3 defined control points.");
        return false;
      }
      this->GetNthControlPointPositionWorld(0, center);
      break;
    default:
      break;
  }
  return true;
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
