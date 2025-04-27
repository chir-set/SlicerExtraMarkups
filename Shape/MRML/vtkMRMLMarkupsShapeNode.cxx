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
#include <vtkMRMLUnitNode.h>

//--------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsShapeNode);

//--------------------------------------------------------------------------------
vtkMRMLMarkupsShapeNode::vtkMRMLMarkupsShapeNode()
{
  const double pi = vtkMath::Pi();
  // Default UVW values for all parametric shapes; obtained from the header files.
  Parametrics[Ellipsoid] =     {-1.0 * pi, 1.0 * pi, -0.5 * pi, 0.5 * pi, 0.0, 1.0, 0, 0, 0, 0, 0, 0, 0};
  Parametrics[Toroid] =        {0,         2.0 * pi, 0,         2.0 * pi, 0.0, 1.0, 0, 0, 0, 0, 0, 0, 1};
  Parametrics[BohemianDome] =  {-1.0 * pi, 1.0 * pi, -1.0 * pi, 1.0 * pi, 0.0, 1.0, 1, 1, 0, 0, 0, 0, 0};
  Parametrics[Bour] =          {0,         1.0,      0,         4.0 * pi, 0.0, 1.0, 0, 0, 0, 0, 0, 0, 0};
  Parametrics[Boy] =           {0,         1.0 * pi, 0,         1.0 * pi, 0.0, 1.0, 1, 1, 0, 1, 1, 0, 0};
  Parametrics[CrossCap] =      {0,         1.0 * pi, 0,         1.0 * pi, 0.0, 1.0, 1, 1, 0, 1, 1, 0, 0};
  Parametrics[ConicSpiral] =   {0,         2.0 * pi, 0,         2.0 * pi, 0.0, 1.0, 0, 0, 0, 0, 0, 0, 0};
  Parametrics[Kuen] =          {-4.5,      4.5,      0.05,      1.0 * pi, 0.0, 1.0, 0, 0, 0, 0, 0, 0, 0};
  Parametrics[Mobius] =        {0,         2.0 * pi, -1.0,      1.0,      0.0, 1.0, 1, 0, 0, 0, 0, 0, 0};
  Parametrics[PluckerConoid] = {0,         3.0,      0.0,       2.0 * pi, 0.0, 1.0, 0, 0, 0, 0, 0, 0, 0};
  Parametrics[Roman] =         {0,         1.0 * pi, 0,         1.0 * pi, 0.0, 1.0, 1, 1, 0, 1, 0, 0, 0};
}

//--------------------------------------------------------------------------------
vtkMRMLMarkupsShapeNode::~vtkMRMLMarkupsShapeNode()
{
  this->RemoveAllMeasurements();
  this->RemoveObserver(this->OnPointPositionUndefinedCallback);
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
  if (this->GetDisplayNode() && !OnPointPositionUndefinedCallback && !OnJumpToPointCallback)
  {
    this->OnPointPositionUndefinedCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    this->OnPointPositionUndefinedCallback->SetClientData( reinterpret_cast<void *>(this) );
    this->OnPointPositionUndefinedCallback->SetCallback( vtkMRMLMarkupsShapeNode::OnPointPositionUndefined );
    this->AddObserver(vtkMRMLMarkupsNode::PointPositionUndefinedEvent, this->OnPointPositionUndefinedCallback);
    
    this->OnJumpToPointCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    this->OnJumpToPointCallback->SetClientData( reinterpret_cast<void *>(this) );
    this->OnJumpToPointCallback->SetCallback( vtkMRMLMarkupsShapeNode::OnJumpToPoint );
    
    this->GetDisplayNode()->AddObserver(vtkMRMLMarkupsDisplayNode::JumpToPointEvent, this->OnJumpToPointCallback);
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
    case vtkMRMLMarkupsShapeNode::Ellipsoid:
      return "Ellipsoid";
    case vtkMRMLMarkupsShapeNode::Toroid:
      return "Toroid";
    case vtkMRMLMarkupsShapeNode::BohemianDome:
      return "BohemianDome";
    case vtkMRMLMarkupsShapeNode::Bour:
      return "Bour";
    case vtkMRMLMarkupsShapeNode::Boy:
      return "Boy";
    case vtkMRMLMarkupsShapeNode::ConicSpiral:
      return "ConicSpiral";
    case vtkMRMLMarkupsShapeNode::CrossCap:
      return "CrossCap";
    case vtkMRMLMarkupsShapeNode::Kuen:
      return "Kuen";
    case vtkMRMLMarkupsShapeNode::Mobius:
      return "Mobius";
    case vtkMRMLMarkupsShapeNode::PluckerConoid:
      return "PluckerConoid";
    case vtkMRMLMarkupsShapeNode::Roman:
      return "Roman";
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
  if (this->ShapeName == shapeName)
  {
    return;
  }
  this->ShapeName = shapeName;
  switch (shapeName)
  {
    case Sphere :
      this->ShapeIsParametric = false;
      this->RequiredNumberOfControlPoints = 2;
      this->MaximumNumberOfControlPoints = 2;
      this->ForceSphereMeasurements();
      break;
    case Ring:
      this->ShapeIsParametric = false;
      // Third point is used to calculate normal relative to the center in 3D view.
      this->RequiredNumberOfControlPoints = 3;
      this->MaximumNumberOfControlPoints = 3;
      this->ForceRingMeasurements();
      break;
    case Disk:
      this->ShapeIsParametric = false;
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
       * For control points to lie on the surface of the tube, adjacent pairs must not be too close to each other.
       * In practice, we would place points on the walls of a diseased artery, a short part of it.
       * All this is useless for healthy arteries : we have real structures using segmentation,
       * and they do not have perfectly circular cross-sections.
       */
      this->ShapeIsParametric = false;
      this->RequiredNumberOfControlPoints = -1;
      this->MaximumNumberOfControlPoints = -1;
      this->ForceTubeMeasurements();
      break;
    case Cylinder:
      this->ShapeIsParametric = false;
      // Points 0 : centre at one end; point 2 : radius; point 3 : centre of the opposite end
      this->RequiredNumberOfControlPoints = 3;
      this->MaximumNumberOfControlPoints = 3;
      this->ForceCylinderMeasurements();
      break;
    case Cone:
      this->ShapeIsParametric = false;
      // Points 0 : centre of the base; point 2 : radius; point 3 : tip
      this->RequiredNumberOfControlPoints = 3;
      this->MaximumNumberOfControlPoints = 3;
      this->ForceConeMeasurements();
      break;
    case Arc:
      this->ShapeIsParametric = false;
      // Points 0 : centre; point 2 : radius and polar vector; point 3 : normal and angle
      this->RequiredNumberOfControlPoints = 3;
      this->MaximumNumberOfControlPoints = 3;
      this->ForceArcMeasurements();
      break;
    case Roman :
    case PluckerConoid :
    case Mobius :
    case Kuen :
    case ConicSpiral :
    case CrossCap :
    case Boy :
    case Bour :
    case BohemianDome :
    case Toroid :
    case Ellipsoid :
      this->ShapeIsParametric = true;
      this->RequiredNumberOfControlPoints = 4;
      this->MaximumNumberOfControlPoints = 4;
      break;
    case ShapeName_Last :
      break;
    default :
      vtkErrorMacro("Unknown shape.");
      return;
  };
  if (this->ShapeIsParametric)
  {
    switch (shapeName)
    {
      case Ellipsoid:
        this->ForceEllipsoidMeasurements();
        break;
      case Toroid:
        this->ForceToroidMeasurements();
        break;
      case BohemianDome:
        this->ForceBohemianDomeMeasurements();
        break;
      case Roman:
      case PluckerConoid:
      case Mobius:
      case Kuen:
      case Bour:
        this->ForceTransformScaledMeasurements();
        break;
      case ConicSpiral:
        this->ForceConicSpiralMeasurements();
        break;
      case CrossCap:
      case Boy:
        this->ForceTransformScaledMeasurements(true); // With volume measurement.
        break;
      case ShapeName_Last :
        break;
      default:
        vtkErrorMacro("Unknown shape.");
        return;
    }
  }
  if (this->MaximumNumberOfControlPoints > 0)
  {
    while (this->GetNumberOfControlPoints() > this->MaximumNumberOfControlPoints)
    {
      this->RemoveNthControlPoint(this->MaximumNumberOfControlPoints);
    }
  }
  if (this->ShapeIsParametric)
  {
    this->ApplyDefaultParametrics(); // Set default UVW values for this parametric shape.
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
  if (this->IsParametric())
  {
    vtkErrorMacro("Current shape is not managed.");
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
    case Disk:
    case Cylinder:
    case Cone:
    case Arc:
      this->ResliceToPlane();
      break;
    case Tube:
      if (this->SnapNthControlPointToTubeSurface(this->ActiveControlPoint, false))
      {
        this->ResliceToTubeCrossSection(this->ActiveControlPoint);
      }
      break;
    case Roman:
    case PluckerConoid:
    case Mobius:
    case Kuen:
    case CrossCap:
    case Boy:
    case Bour:
    case BohemianDome:
    case Toroid:
    case Ellipsoid:
      // Can be any combination; we want at least p1 and p4 since they control orientation.
      this->ResliceToPlane(0, 1, 3);
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
  
  this->AddLengthMeasurement("innerRadius", true);
  this->AddLengthMeasurement("outerRadius", true);
  this->AddLengthMeasurement("width", false);
  this->AddAreaMeasurement("area");
  this->AddAreaMeasurement("innerArea");
  this->AddAreaMeasurement("outerArea");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceRingMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddLengthMeasurement("radius", true);
  this->AddAreaMeasurement("area");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceSphereMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddLengthMeasurement("radius", true);
  this->AddAreaMeasurement("area");
  this->AddVolumeMeasurement("volume");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceTubeMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddAreaMeasurement("area");
  this->AddVolumeMeasurement("volume");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceCylinderMeasurements()
{
  this->RemoveAllMeasurements();

  this->AddLengthMeasurement("radius", true);
  this->AddLengthMeasurement("height", true);
  this->AddAreaMeasurement("area");
  this->AddVolumeMeasurement("volume");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceConeMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddLengthMeasurement("radius", true);
  this->AddLengthMeasurement("height", true);
  this->AddLengthMeasurement("slant");
  this->AddCustomMeasurement("aperture", false, nullptr, "°");
  this->AddAreaMeasurement("area");
  this->AddVolumeMeasurement("volume");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceArcMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddCustomMeasurement("angle", true, nullptr, "°");
  this->AddLengthMeasurement("radius");
  this->AddAreaMeasurement("area");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceEllipsoidMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddLengthMeasurement("radius-x");
  this->AddLengthMeasurement("radius-y");
  this->AddLengthMeasurement("radius-z");
  this->AddCustomMeasurement("n1", false, nullptr);
  this->AddCustomMeasurement("n2", false, nullptr);
  this->AddVolumeMeasurement("volume");
  this->AddAreaMeasurement("area");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceToroidMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddCustomMeasurement("radius-x-scalefactor", false);
  this->AddCustomMeasurement("radius-y-scalefactor", false);
  this->AddCustomMeasurement("radius-z-scalefactor", false);
  this->AddLengthMeasurement("radius-ring");
  this->AddLengthMeasurement("radius-crosssection");
  this->AddCustomMeasurement("n1", false, nullptr);
  this->AddCustomMeasurement("n2", false, nullptr);
  this->AddVolumeMeasurement("volume");
  this->AddAreaMeasurement("area");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceBohemianDomeMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddLengthMeasurement("a");
  this->AddLengthMeasurement("b");
  this->AddLengthMeasurement("c");
  this->AddAreaMeasurement("area");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceConicSpiralMeasurements()
{
  this->RemoveAllMeasurements();
  
  this->AddLengthMeasurement("x", false, false);
  this->AddLengthMeasurement("y", false, false);
  this->AddLengthMeasurement("z", false, false);
  this->AddCustomMeasurement("n", false, nullptr);
  this->AddAreaMeasurement("area");
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ForceTransformScaledMeasurements(bool withVolume)
{
  this->RemoveAllMeasurements();
  
  this->AddCustomMeasurement("x-scalefactor", false);
  this->AddCustomMeasurement("y-scalefactor", false);
  this->AddCustomMeasurement("z-scalefactor", false);
  if (withVolume)
  {
    this->AddVolumeMeasurement("volume");
  }
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
  if (this->GetShapeName() != Tube)
  {
    vtkErrorMacro("Current Shape node is not a Tube.");
    return;
  }
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
bool vtkMRMLMarkupsShapeNode::GetNthControlPointSplineIntersection(int pointIndex, vtkPoints * point)
{
  if (this->GetShapeName() != Tube)
  {
    vtkErrorMacro("Not a Tube shape.");
    return false;
  }
  if (!point || point->GetNumberOfPoints())
  {
    vtkErrorMacro("Parameter 'point' is NULL or is not empty.");
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
  vtkIdType id = this->SplineWorld->FindPoint(middlePoint);
  // Closest point on spline to calculated middle point.
  // NB : if the spline is a ball of wool, result is not predictable.
  this->SplineWorld->GetPoint(id, splineMiddlePoint);
  point->InsertNextPoint(splineMiddlePoint);

  return true;
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

  vtkNew<vtkPoints> result;
  this->GetNthControlPointSplineIntersection(pointIndex, result);
  double * splineMiddlePoint = result->GetPoint(0);
  double splineMiddlePointNeighbour[3] = { 0.0 };
  vtkIdType id = this->SplineWorld->FindPoint(splineMiddlePoint);
  vtkIdType idNeighbour = (id == this->SplineWorld->GetNumberOfPoints() - 1) // Last point
                        ? id -1
                        : id + 1;
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

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------7
bool vtkMRMLMarkupsShapeNode::GetControlPointPairPosition(vtkPolyData * spline,
                                                          int pointIndex, vtkPoints* result)
{
  if (!result || pointIndex < 0)
  {
    vtkErrorMacro("Index must be > 0 and the resuly array must not be NULL.");
    return false;
  }
  vtkDataArray * radiusArray = spline->GetPointData()->GetArray("TubeRadius");
  if (!radiusArray)
  {
    vtkErrorMacro("Tube does not have a radius array named 'TubeRadius'");
    return false;
  }
  // Get surface points orthogonal to the spline.
  double splineP1[3] = {0.0};
  double splineP2[3] = {0.0};
  double surfaceP1[3] = {0.0};
  double surfaceP2[3] = {0.0};

  spline->GetPoint(pointIndex, splineP1);
  if (pointIndex == (spline->GetNumberOfPoints() - 1)) // Last point.
  {
    spline->GetPoint(pointIndex - 1, splineP2);
  }
  else
  {
    spline->GetPoint(pointIndex + 1, splineP2);
  }
  double direction[3] = {0.0};
  double perp1[3] = {0.0};
  double perp2[3] = {0.0};
  vtkMath::Subtract(splineP2, splineP1, direction);
  vtkMath::Perpendiculars(direction, perp1, perp2, 0.0);
  const double perp1Length = vtkMath::Norm(perp1); // 1.0
  vtkMath::Add(splineP1, perp1, perp1);
  const double radius = vtkDoubleArray::SafeDownCast(radiusArray)->GetValue(pointIndex);
  vtkMath::GetPointAlongLine(surfaceP1, splineP1, perp1, radius - perp1Length);
  vtkMath::GetPointAlongLine(surfaceP2, perp1, splineP1, radius);
  result->Initialize();
  result->InsertNextPoint(surfaceP1);
  result->InsertNextPoint(surfaceP2);

  return true;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeNode::UpdateNumberOfControlPoints(int numberOfControlPoints, bool bypassLockedState)
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
  if ((numberOfControlPoints < 4
    || numberOfControlPoints % 2) != 0)
  {
    vtkErrorMacro("An odd number of control points"
    "or fewer than 4 control points is requested.");
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

  if (!this->GetScene())
  {
    vtkErrorMacro("Scene is NULL, aborting.");
    return false;
  }

  this->GetScene()->StartState(vtkMRMLScene::BatchProcessState);
  vtkNew<vtkPolyData> splineCopy;
  splineCopy->DeepCopy(this->SplineWorld);
  const int numberOfInitialSplinePoints = this->SplineWorld->GetNumberOfPoints();
  const int numberOfFinalControlPointPairs = numberOfControlPoints / 2;
  const int numberOfSplinePointsPerInterval = numberOfInitialSplinePoints / numberOfFinalControlPointPairs;
  this->RemoveAllControlPoints();
  for (int i = 0; i < numberOfFinalControlPointPairs; i++)
  {
    int pointIndex = i * numberOfSplinePointsPerInterval;
    vtkNew<vtkPoints> result;
    if (!this->GetControlPointPairPosition(splineCopy, pointIndex, result))
    {
      vtkWarningMacro("Error determining control points positions.");
      continue;
    }
    this->AddControlPoint(result->GetPoint(0));
    this->AddControlPoint(result->GetPoint(1));
  }

  const int numberOfControlPointsCreated = this->GetNumberOfControlPoints();
  vtkNew<vtkPoints> result;
  if (!this->GetControlPointPairPosition(splineCopy, splineCopy->GetNumberOfPoints() - 1, result)) // Last spline point
  {
    vtkWarningMacro("Error determining last control points positions.");
    this->GetScene()->EndState(vtkMRMLScene::BatchProcessState);
    return false;
  }
  if (numberOfControlPoints == numberOfControlPointsCreated)
  {
    this->SetNthControlPointPositionWorld(numberOfControlPointsCreated - 1 - 1, result->GetPoint(0));
    this->SetNthControlPointPositionWorld(numberOfControlPointsCreated - 1, result->GetPoint(1));
  }
  else
  {
    this->AddControlPoint(result->GetPoint(0));
    this->AddControlPoint(result->GetPoint(1));
  }
  this->GetScene()->EndState(vtkMRMLScene::BatchProcessState);
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
  vtkNew<vtkPoints> result;
  this->GetNthControlPointSplineIntersection(pointIndex, result);
  double * splineMiddlePoint = result->GetPoint(0);
  double splineMiddlePointNeighbour[3] = { 0.0 };
  vtkIdType id = this->SplineWorld->FindPoint(splineMiddlePoint);
  vtkIdType idNeighbour = (id == this->SplineWorld->GetNumberOfPoints() - 1) // Last point
                        ? id -1
                        : id + 1;
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
  if ((this->GetNumberOfUndefinedControlPoints() || this->GetNumberOfUndefinedControlPoints(true)))
  {
    // Avoid this message when CopyContent is unexpectedly called: markups creation, point placement, moving points.
    if (!this->GetDisableModifiedEvent())
    {
      vtkErrorMacro("Aborting because Shape node has undefined points.");
    }
    return false;
  }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  double p4[3] = { 0.0 };
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
    case Roman:
    case PluckerConoid:
    case Mobius:
    case Kuen:
    case ConicSpiral:
    case CrossCap:
    case Boy:
    case Bour:
    case BohemianDome:
    case Toroid:
    case Ellipsoid:
      if (this->GetNumberOfDefinedControlPoints() != 4)
      {
        // Avoid this message when CopyContent is unexpectedly called: markups creation, point placement, moving points.
        if (!this->GetDisableModifiedEvent())
        {
          vtkErrorMacro("Parametric Shape node does not have 4 defined control points.");
        }
        return false;
      }
      this->GetNthControlPointPositionWorld(0, p1);
      this->GetNthControlPointPositionWorld(3, p4);
      if (this->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
      {
        vtkMath::Assign(p1, center);
      }
      else
      {
        center[0] = (p1[0] + p4[0]) / 2.0;
        center[1] = (p1[1] + p4[1]) / 2.0;
        center[2] = (p1[2] + p4[2]) / 2.0;
      }
      break;
    default:
      break;
  }
  return true;
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::AddCustomMeasurement(const char* name, bool enabled,
                                                   const char* format, const char* units)
{
  vtkNew<vtkMRMLMeasurementShape> measurement;
  measurement->SetName(name);
  if (units)
  {
    measurement->SetUnits(units);
  }
  if (format)
  {
    measurement->SetPrintFormat(format);
  }
  measurement->SetInputMRMLNode(this);
  measurement->SetEnabled(enabled);
  this->AddMeasurement(measurement);
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::AddLengthMeasurement(const char* name, bool enabled,
                                                   bool withUnit, const char * suffix)
{
  vtkNew<vtkMRMLMeasurementShape> measurement;
  measurement->SetName(name);
  vtkMRMLUnitNode * lengthUnitNode = this->GetUnitNode("length");
  if (lengthUnitNode)
  {
    measurement->SetPrintFormat(lengthUnitNode->GetDisplayStringFormat());
    measurement->SetDisplayCoefficient(lengthUnitNode->GetDisplayCoefficient());
    if (withUnit)
    {
      measurement->SetUnits(lengthUnitNode->GetSuffix());
    }
    else
    {
      if (suffix)
      {
        measurement->SetUnits(suffix);
      }
    }
  }
  measurement->SetInputMRMLNode(this);
  measurement->SetEnabled(enabled);
  this->AddMeasurement(measurement);
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::AddAreaMeasurement(const char* name, bool enabled)
{
  if (!this->GetScene())
  {
    return;
  }
  vtkNew<vtkMRMLMeasurementShape> measurement;
  measurement->SetName(name);
  vtkMRMLUnitNode * areaUnitNode = this->GetUnitNode("length");
  if (areaUnitNode)
  {
    measurement->SetPrintFormat(areaUnitNode->GetDisplayStringFormat());
    measurement->SetDisplayCoefficient(areaUnitNode->GetDisplayCoefficient());
    measurement->SetUnits(areaUnitNode->GetSuffix());
  }
  measurement->SetInputMRMLNode(this);
  measurement->SetEnabled(enabled);
  this->AddMeasurement(measurement);
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::AddVolumeMeasurement(const char* name, bool enabled)
{
  if (!this->GetScene())
  {
    return;
  }
  vtkNew<vtkMRMLMeasurementShape> measurement;
  measurement->SetName(name);
  vtkMRMLUnitNode * volumeUnitNode = this->GetUnitNode("length");
  if (volumeUnitNode)
  {
    measurement->SetPrintFormat(volumeUnitNode->GetDisplayStringFormat());
    measurement->SetDisplayCoefficient(volumeUnitNode->GetDisplayCoefficient());
    measurement->SetUnits(volumeUnitNode->GetSuffix());
  }
  measurement->SetInputMRMLNode(this);
  measurement->SetEnabled(enabled);
  this->AddMeasurement(measurement);
}

//----------------------------API only----------------------------------------
bool vtkMRMLMarkupsShapeNode::SetParametricAxisValue(const char axis, double distance, bool moveControlPoint)
{
  if (!this->ShapeIsParametric)
  {
    vtkErrorMacro("Shape is not in the 'Parametric' group.");
    return false;
  }
  int pointIndex = 0;
  switch (axis)
  {
    case 'x':
      pointIndex = 1;
      break;
    case 'y':
      pointIndex = 2;
      break;
    case 'z':
      pointIndex = 3;
      break;
    default:
      vtkErrorMacro("Axis must be x, y or z.");
      return false;
  }
  
  if (!moveControlPoint) // Called from vtkSlicerShapeRepresentation3D
  {
    switch (pointIndex)
    {
      case 1:
        this->ParametricX = distance;
        break;
      case 2:
        this->ParametricY = distance;
        break;
      case 3:
        this->ParametricZ = distance;
        break;
      default:
        vtkErrorMacro("Point index must be 1, 2 or 3.");
        return false;
    }
    return true;
  }
  
  if (this->GetNumberOfDefinedControlPoints(false) < 4)
  {
    // Avoid this message when CopyContent is unexpectedly called: markups creation, point placement, moving points.
    if (!this->GetDisableModifiedEvent())
    {
      vtkErrorMacro("4 defined control points are required.");
    }
    return false;
  }
  
  double center[3] = { 0.0 };
  double px[3] = { 0.0 };
  double pNew[3] = { 0.0 };
  if (!this->GetCenterWorld(center))
  {
    return false;
  }
  this->GetNthControlPointPositionWorld(pointIndex, px);
  double currentDistance = std::sqrt(vtkMath::Distance2BetweenPoints(center, px));
  vtkMath::GetPointAlongLine(pNew, center, px, distance - currentDistance);
  this->SetNthControlPointPositionWorld(pointIndex, pNew);

  return true;
}

//----------------------------------------------------------------------------
int vtkMRMLMarkupsShapeNode::SetParametricXYZ(double value) // Isotropic
{
  if (this->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Circumferential)
  {
    vtkErrorMacro("Aborting: isotropic radii cannot be set in circumferential mode.");
    return -1;
  }
  int axes[3] = {'x', 'y', 'z'};
  for (int i = 0; i < 3; i++)
  {
    bool result = SetParametricAxisValue(axes[i], value, true);
    if (!result)
    {
      return axes[i];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
int vtkMRMLMarkupsShapeNode::SetParametricXYZ(double xvalue, double yvalue, double zvalue)
{
  if (this->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Circumferential)
  {
    vtkWarningMacro("The requested radii may not be exactly set in circumferential mode on a single call.");
  }
  int axes[3] = {'x', 'y', 'z'};
  double values[3] = {xvalue, yvalue, zvalue};
  for (int i = 0; i < 3; i++)
  {
    bool result = SetParametricAxisValue(axes[i], values[i], true);
    if (!result)
    {
      return axes[i];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeNode::SetParametricXYZToActiveControlPoint()
{
  if (!this->ShapeIsParametric)
  {
    vtkErrorMacro("Shape is not in the 'Parametric' group.");
    return false;
  }
  if (this->GetNumberOfDefinedControlPoints(false) != this->GetRequiredNumberOfControlPoints())
  {
    vtkErrorMacro("Shape does not have required number of control points.");
    return false;
  }
  // ActiveControlPoint is set by OnJumpToPoint(); it must have been clicked.
  if (this->ActiveControlPoint == 0)
  {
    return false;
  }
  double center[3] = { 0.0 };
  double px[3] = { 0.0 };
  if (!this->GetCenterWorld(center))
  {
    return false;
  }
  this->GetNthControlPointPositionWorld(this->ActiveControlPoint , px);
  double distance = std::sqrt(vtkMath::Distance2BetweenPoints(center, px));
  return (this->SetParametricXYZ(distance) == 0);
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeNode::SetParametricN(double value)
{
  if (value == 0.0) // Cancels all drawing in a persistent way.
  {
    vtkWarningMacro("0.0 is not valid, ignoring.");
    return false;
  }
  this->ParametricN = value;
  this->Modified();
  return true;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeNode::SetParametricN1(double value)
{
  if (value == 0.0)
  {
    vtkWarningMacro("0.0 is not valid, ignoring.");
    return false;
  }
  this->ParametricN1 = value;
  this->Modified();
  return true;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeNode::SetParametricN2(double value)
{
  if (value == 0.0)
  {
    vtkWarningMacro("0.0 is not valid, ignoring.");
    return false;
  }
  this->ParametricN2 = value;
  this->Modified();
  return true;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeNode::SetParametricRadius(double value)
{
  if (value == 0.0)
  {
    vtkWarningMacro("0.0 is not valid, ignoring.");
    return false;
  }
  this->ParametricRadius = value;
  this->Modified();
  return true;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeNode::SetParametricRingRadius(double value)
{
  if (value == 0.0)
  {
    vtkWarningMacro("0.0 is not valid, ignoring.");
    return false;
  }
  this->ParametricRingRadius = value;
  this->Modified();
  return true;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsShapeNode::SetParametricCrossSectionRadius(double value)
{
  if (value == 0.0)
  {
    vtkWarningMacro("0.0 is not valid, ignoring.");
    return false;
  }
  this->ParametricCrossSectionRadius = value;
  this->Modified();
  return true;
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::ApplyDefaultParametrics()
{
  if (!this->ShapeIsParametric)
  {
    return;
  }
  // Set the default UVW values for the current shape.
  ParametricsTuple tuple = this->Parametrics[this->ShapeName];
  this->ParametricRangeU.first = std::get<MinimumU>(tuple);
  this->ParametricRangeU.second = std::get<MaximumU>(tuple);
  this->ParametricRangeV.first = std::get<MinimumV>(tuple);
  this->ParametricRangeV.second = std::get<MaximumV>(tuple);
  this->ParametricRangeW.first = std::get<MinimumW>(tuple);
  this->ParametricRangeW.second = std::get<MaximumW>(tuple);
  
  this->ParametricMinimumU = std::get<MinimumU>(tuple);
  this->ParametricMaximumU = std::get<MaximumU>(tuple);
  this->ParametricMinimumV = std::get<MinimumV>(tuple);
  this->ParametricMaximumV = std::get<MaximumV>(tuple);
  this->ParametricMinimumW = std::get<MinimumW>(tuple);
  this->ParametricMaximumW = std::get<MaximumW>(tuple);
  this->ParametricJoinU = std::get<JoinU>(tuple);
  this->ParametricJoinV = std::get<JoinV>(tuple);
  this->ParametricJoinW = std::get<JoinW>(tuple);
  this->ParametricTwistU = std::get<TwistU>(tuple);
  this->ParametricTwistV = std::get<TwistV>(tuple);
  this->ParametricTwistW = std::get<TwistW>(tuple);
  this->ParametricClockwiseOrdering = std::get<ClockwiseOrdering>(tuple);
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::SetUseAlternateColors(const std::string& nodeID)
{
  // Avoid this message when CopyContent is unexpectedly called: markups creation, point placement, moving points.
  if (!this->GetDisableModifiedEvent())
  {
    vtkInfoMacro("The UseAlternateColors property is read from a default scene node only.");
  }
  this->UseAlternateColors = nodeID;
}


//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  vtkMRMLPrintBeginMacro(os, indent);
  vtkMRMLPrintEnumMacro(ShapeName);
  vtkMRMLPrintEnumMacro(RadiusMode);
  vtkMRMLPrintEnumMacro(DrawMode2D);
  vtkMRMLPrintFloatMacro(Resolution);
  vtkMRMLPrintBooleanMacro(ScalarVisibility);
  vtkMRMLPrintObjectMacro(ResliceNode);
  vtkMRMLPrintBooleanMacro(DisplayCappedTube);
  vtkMRMLPrintStdStringMacro(UseAlternateColors);
  vtkMRMLPrintFloatMacro(ParametricN1);
  vtkMRMLPrintFloatMacro(ParametricN2);
  vtkMRMLPrintFloatMacro(ParametricN);
  vtkMRMLPrintFloatMacro(ParametricRingRadius);
  vtkMRMLPrintFloatMacro(ParametricRingRadius);
  vtkMRMLPrintFloatMacro(ParametricRadius);
  vtkMRMLPrintFloatMacro(ParametricX);
  vtkMRMLPrintFloatMacro(ParametricY);
  vtkMRMLPrintFloatMacro(ParametricZ);
  vtkMRMLPrintFloatMacro(ParametricMinimumU);
  vtkMRMLPrintFloatMacro(ParametricMaximumU);
  vtkMRMLPrintFloatMacro(ParametricMinimumV);
  vtkMRMLPrintFloatMacro(ParametricMaximumV);
  vtkMRMLPrintFloatMacro(ParametricMinimumW);
  vtkMRMLPrintFloatMacro(ParametricMaximumW);
  vtkMRMLPrintBooleanMacro(ParametricJoinU);
  vtkMRMLPrintBooleanMacro(ParametricJoinV);
  vtkMRMLPrintBooleanMacro(ParametricJoinW);
  vtkMRMLPrintBooleanMacro(ParametricTwistU);
  vtkMRMLPrintBooleanMacro(ParametricTwistV);
  vtkMRMLPrintBooleanMacro(ParametricTwistW);
  vtkMRMLPrintBooleanMacro(ParametricClockwiseOrdering);
  vtkMRMLPrintIntMacro(ParametricScalarMode);
  vtkMRMLPrintEndMacro();
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsShapeNode::CopyContent(vtkMRMLNode* anode, bool deepCopy/*=true*/)
{
  MRMLNodeModifyBlocker blocker(this);
  Superclass::CopyContent(anode, deepCopy);
  
  vtkMRMLCopyBeginMacro(anode);
  vtkMRMLCopyEnumMacro(ShapeName);
  vtkMRMLCopyEnumMacro(RadiusMode);
  vtkMRMLCopyEnumMacro(DrawMode2D);
  vtkMRMLCopyFloatMacro(Resolution);
  vtkMRMLCopyBooleanMacro(ScalarVisibility);
  vtkMRMLCopyBooleanMacro(DisplayCappedTube);
  vtkMRMLCopyStdStringMacro(UseAlternateColors);
  if (this->ShapeIsParametric)
  {
    vtkMRMLCopyFloatMacro(ParametricN1);
    vtkMRMLCopyFloatMacro(ParametricN2);
    vtkMRMLCopyFloatMacro(ParametricN);
    vtkMRMLCopyFloatMacro(ParametricRingRadius);
    vtkMRMLCopyFloatMacro(ParametricRingRadius);
    vtkMRMLCopyFloatMacro(ParametricRadius);
    vtkMRMLCopyFloatMacro(ParametricX);
    vtkMRMLCopyFloatMacro(ParametricY);
    vtkMRMLCopyFloatMacro(ParametricZ);
    vtkMRMLCopyFloatMacro(ParametricMinimumU);
    vtkMRMLCopyFloatMacro(ParametricMaximumU);
    vtkMRMLCopyFloatMacro(ParametricMinimumV);
    vtkMRMLCopyFloatMacro(ParametricMaximumV);
    vtkMRMLCopyFloatMacro(ParametricMinimumW);
    vtkMRMLCopyFloatMacro(ParametricMaximumW);
    vtkMRMLCopyBooleanMacro(ParametricJoinU);
    vtkMRMLCopyBooleanMacro(ParametricJoinV);
    vtkMRMLCopyBooleanMacro(ParametricJoinW);
    vtkMRMLCopyBooleanMacro(ParametricTwistU);
    vtkMRMLCopyBooleanMacro(ParametricTwistV);
    vtkMRMLCopyBooleanMacro(ParametricTwistW);
    vtkMRMLCopyBooleanMacro(ParametricClockwiseOrdering);
    vtkMRMLCopyIntMacro(ParametricScalarMode);
  }
  vtkMRMLCopyEndMacro();
}
