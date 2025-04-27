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

#include "vtkSlicerShapeRepresentation3D.h"

#include "vtkMRMLMarkupsShapeNode.h"

// VTK includes
#include <vtkActor.h>
#include <vtkCutter.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkTupleInterpolator.h>
#include <vtkPointData.h>
#include <vtkCollection.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkTextActor.h>
#include <vtkMatrix4x4.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerShapeRepresentation3D);

//------------------------------------------------------------------------------
vtkSlicerShapeRepresentation3D::vtkSlicerShapeRepresentation3D()
{
  this->DiskSource = vtkSmartPointer<vtkDiskSource>::New();
  this->RingSource = vtkSmartPointer<vtkDiskSource>::New();
  this->RadiusSource = vtkSmartPointer<vtkLineSource>::New();
  this->SphereSource = vtkSmartPointer<vtkSphereSource>::New();
  this->ConeSource = vtkSmartPointer<vtkConeSource>::New();
  this->ConeSource->CappingOn();
  this->ArcSource = vtkSmartPointer<vtkArcSource>::New();
  this->ArcSource->UseNormalAndAngleOn();
  
  this->ShapeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->ShapeProperty = vtkSmartPointer<vtkProperty>::New();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(Selected)->Property);
  this->ShapeActor = vtkSmartPointer<vtkActor>::New();
  this->ShapeActor->SetMapper(this->ShapeMapper);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  
  this->MiddlePointSource = vtkSmartPointer<vtkSphereSource>::New();
  this->MiddlePointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->MiddlePointMapper->SetInputConnection(this->MiddlePointSource->GetOutputPort());
  this->MiddlePointActor = vtkSmartPointer<vtkActor>::New();
  this->MiddlePointActor->SetMapper(this->MiddlePointMapper);
  
  this->RadiusMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->RadiusMapper->SetInputConnection(this->RadiusSource->GetOutputPort());
  this->RadiusActor = vtkSmartPointer<vtkActor>::New();
  this->RadiusActor->SetMapper(this->RadiusMapper);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
  
  this->Spline = vtkSmartPointer<vtkParametricSpline>::New();
  vtkNew<vtkPoints> points;
  const double point[3] = { 0.0 };
  points->InsertNextPoint(point);
  this->Spline->SetPoints(points);
  this->SplineFunctionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
  this->SplineFunctionSource->SetParametricFunction(this->Spline);
  this->SplineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->SplineMapper->SetInputConnection(this->SplineFunctionSource->GetOutputPort());
  this->SplineActor = vtkSmartPointer<vtkActor>::New();
  this->SplineActor->SetMapper(this->SplineMapper);
  this->SplineActor->SetProperty(this->ShapeProperty);
  // This is for display. Viewing a closed tube is not natural while dealing with arteries.
  this->Tube = vtkSmartPointer<vtkTubeFilter>::New();
  this->Tube->SetNumberOfSides(20);
  this->Tube->SetVaryRadiusToVaryRadiusByAbsoluteScalar();
  this->Tube->SetInputConnection(this->SplineFunctionSource->GetOutputPort());
  // This is to calculate volume with vtkMassProperties, it needs a closed polydata.
  this->CappedTube = vtkSmartPointer<vtkTubeFilter>::New();
  this->CappedTube->SetNumberOfSides(20);
  this->CappedTube->SetVaryRadiusToVaryRadiusByAbsoluteScalar();
  this->CappedTube->SetInputConnection(this->SplineFunctionSource->GetOutputPort());
  this->CappedTube->SetCapping(true);
  
  this->CylinderAxis = vtkSmartPointer<vtkLineSource>::New();
  this->CylinderSource = vtkSmartPointer<vtkTubeFilter>::New();
  this->CylinderSource->SetNumberOfSides(20);
  this->CylinderSource->SetInputConnection(this->CylinderAxis->GetOutputPort());
  this->CylinderSource->SetCapping(true);

  this->Ellipsoid = vtkSmartPointer<vtkParametricSuperEllipsoid>::New();
  this->Toroid = vtkSmartPointer<vtkParametricSuperToroid>::New();
  this->BohemianDome = vtkSmartPointer<vtkParametricBohemianDome>::New();
  this->Bour = vtkSmartPointer<vtkParametricBour>::New();
  this->Boy = vtkSmartPointer<vtkParametricBoy>::New();
  this->CrossCap = vtkSmartPointer<vtkParametricCrossCap>::New();
  this->ConicSpiral = vtkSmartPointer<vtkParametricConicSpiral>::New();
  this->Kuen = vtkSmartPointer<vtkParametricKuen>::New();
  this->Mobius = vtkSmartPointer<vtkParametricMobius>::New();
  this->PluckerConoid = vtkSmartPointer<vtkParametricPluckerConoid>::New();
  this->Roman = vtkSmartPointer<vtkParametricRoman>::New();
  this->ParametricFunctionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
  
  this->ParametricMiddlePointSource = vtkSmartPointer<vtkSphereSource>::New();
  this->ParametricMiddlePointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->ParametricMiddlePointMapper->SetInputConnection(this->ParametricMiddlePointSource->GetOutputPort());
  this->ParametricMiddlePointActor = vtkSmartPointer<vtkActor>::New();
  this->ParametricMiddlePointActor->SetMapper(this->ParametricMiddlePointMapper);
  this->ParametricTransform = vtkSmartPointer<vtkTransform>::New();
  this->ParametricTransformer = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->ParametricTransformer->SetTransform(this->ParametricTransform);
}

//------------------------------------------------------------------------------
vtkSlicerShapeRepresentation3D::~vtkSlicerShapeRepresentation3D()
{
}

//------------------------------------------------------------------------------
void vtkSlicerShapeRepresentation3D::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
void vtkSlicerShapeRepresentation3D::GetActors(vtkPropCollection* pc)
{
  this->Superclass::GetActors(pc);
  this->ShapeActor->GetActors(pc);
  this->MiddlePointActor->GetActors(pc);
  this->ParametricMiddlePointActor->GetActors(pc);
  this->RadiusActor->GetActors(pc);
  this->SplineActor->GetActors(pc);
}

//------------------------------------------------------------------------------
void vtkSlicerShapeRepresentation3D::ReleaseGraphicsResources(vtkWindow* win)
{
  this->Superclass::ReleaseGraphicsResources(win);
  this->ShapeActor->ReleaseGraphicsResources(win);
  this->MiddlePointActor->ReleaseGraphicsResources(win);
  this->ParametricMiddlePointActor->ReleaseGraphicsResources(win);
  this->RadiusActor->ReleaseGraphicsResources(win);
  this->SplineActor->ReleaseGraphicsResources(win);
}

//------------------------------------------------------------------------------
int vtkSlicerShapeRepresentation3D::RenderOverlay(vtkViewport* viewport)
{
  int count = this->Superclass::RenderOverlay(viewport);
  if (this->ShapeActor->GetVisibility())
  {
    count += this->ShapeActor->RenderOverlay(viewport);
  }
  if (this->MiddlePointActor->GetVisibility())
  {
    count += this->MiddlePointActor->RenderOverlay(viewport);
  }
  if (this->ParametricMiddlePointActor->GetVisibility())
  {
    count += this->ParametricMiddlePointActor->RenderOverlay(viewport);
  }
  if (this->RadiusActor->GetVisibility())
  {
    count += this->RadiusActor->RenderOverlay(viewport);
  }
  if (this->SplineActor->GetVisibility())
  {
    count += this->SplineActor->RenderOverlay(viewport);
  }
  return count;
}

//------------------------------------------------------------------------------
int vtkSlicerShapeRepresentation3D::RenderOpaqueGeometry(vtkViewport* viewport)
{
  int count = this->Superclass::RenderOpaqueGeometry(viewport);
  if (this->ShapeActor->GetVisibility())
  {
    count += this->ShapeActor->RenderOpaqueGeometry(viewport);
  }
  if (this->MiddlePointActor->GetVisibility())
  {
    count += this->MiddlePointActor->RenderOpaqueGeometry(viewport);
  }
  if (this->ParametricMiddlePointActor->GetVisibility())
  {
    count += this->ParametricMiddlePointActor->RenderOpaqueGeometry(viewport);
  }
  if (this->RadiusActor->GetVisibility())
  {
    count += this->RadiusActor->RenderOpaqueGeometry(viewport);
  }
  if (this->SplineActor->GetVisibility())
  {
    count += this->SplineActor->RenderOpaqueGeometry(viewport);
  }

  return count;
}

//------------------------------------------------------------------------------
int vtkSlicerShapeRepresentation3D::RenderTranslucentPolygonalGeometry(vtkViewport* viewport)
{
  int count = this->Superclass::RenderTranslucentPolygonalGeometry(viewport);
  if (this->ShapeActor->GetVisibility())
  {
    this->ShapeActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->ShapeActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->MiddlePointActor->GetVisibility())
  {
    this->MiddlePointActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->MiddlePointActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->ParametricMiddlePointActor->GetVisibility())
  {
    this->ParametricMiddlePointActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->ParametricMiddlePointActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->RadiusActor->GetVisibility())
  {
    this->RadiusActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->RadiusActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->SplineActor->GetVisibility())
  {
    this->SplineActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->SplineActor->RenderTranslucentPolygonalGeometry(viewport);
  }

  return count;
}

//------------------------------------------------------------------------------
vtkTypeBool vtkSlicerShapeRepresentation3D::HasTranslucentPolygonalGeometry()
{
  if (this->Superclass::HasTranslucentPolygonalGeometry())
    {
    return true;
    }
  if (this->ShapeActor->GetVisibility() &&
      this->ShapeActor->HasTranslucentPolygonalGeometry())
    {
      return true;
    }
  if (this->MiddlePointActor->GetVisibility() &&
      this->MiddlePointActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }
  if (this->ParametricMiddlePointActor->GetVisibility() &&
    this->ParametricMiddlePointActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }
  if (this->RadiusActor->GetVisibility() &&
    this->RadiusActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }
  if (this->SplineActor->GetVisibility() &&
    this->SplineActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateFromMRML(vtkMRMLNode* caller,
                                                           unsigned long event,
                                                           void *callData /*=nullptr*/)
{
  this->Superclass::UpdateFromMRML(caller, event, callData);

  this->NeedToRenderOn();

  this->BuildMiddlePoint();

  vtkMRMLMarkupsShapeNode* shapeNode=
    vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  if (!shapeNode)
  {
    return;
  }

  this->ShapeMapper->SetScalarVisibility(shapeNode->GetScalarVisibility());
  this->RadiusMapper->SetScalarVisibility(shapeNode->GetScalarVisibility());
  this->SplineMapper->SetScalarVisibility(shapeNode->GetScalarVisibility());

  this->ShapeActor->SetVisibility(false);
  this->MiddlePointActor->SetVisibility(false);
  this->ParametricMiddlePointActor->SetVisibility(false);
  this->RadiusActor->SetVisibility(false);
  this->TextActor->SetVisibility(false);
  this->SplineActor->SetVisibility(false);

  if (!shapeNode->IsParametric())
  {
    // A ShapeActor's transform is applied in UpdateParametricFromMRML; restore it.
    this->ShapeActor->SetUserTransform(nullptr);
    switch (shapeNode->GetShapeName())
    {
      case vtkMRMLMarkupsShapeNode::Sphere :
        this->UpdateSphereFromMRML(caller, event, callData);
        break;
      case vtkMRMLMarkupsShapeNode::Ring :
        this->UpdateRingFromMRML(caller, event, callData);
        break;
      case vtkMRMLMarkupsShapeNode::Disk :
        this->UpdateDiskFromMRML(caller, event, callData);
        break;
      case vtkMRMLMarkupsShapeNode::Tube :
        this->UpdateTubeFromMRML(caller, event, callData);
        break;
      case vtkMRMLMarkupsShapeNode::Cone :
        this->UpdateConeFromMRML(caller, event, callData);
        break;
      case vtkMRMLMarkupsShapeNode::Cylinder :
        this->UpdateCylinderFromMRML(caller, event, callData);
        break;
      case vtkMRMLMarkupsShapeNode::Arc :
        this->UpdateArcFromMRML(caller, event, callData);
        break;
      case vtkMRMLMarkupsShapeNode::ShapeName_Last :
        break;
      default:
        vtkErrorMacro("Unknown shape.");
        return;
    }
  }
  else
  {
    this->UpdateParametricFromMRML(caller, event, callData);
  }
}

//------------------------------------------------------------------------------
void vtkSlicerShapeRepresentation3D::BuildMiddlePoint()
{
  vtkMRMLMarkupsNode* markupsNode = this->GetMarkupsNode();
  if (!markupsNode)
  {
    return;
  }

  if (markupsNode->GetNumberOfControlPoints() < 2)
  {
    return;
  }

  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double center[3] = {0.0};
  markupsNode->GetNthControlPointPositionWorld(0, p1);
  markupsNode->GetNthControlPointPositionWorld(1, p2);
  center[0] = (p1[0] + p2[0]) / 2.0;
  center[1] = (p1[1] + p2[1]) / 2.0;
  center[2] = (p1[2] + p2[2]) / 2.0;

  this->MiddlePointSource->SetCenter(center);
  this->MiddlePointSource->SetRadius(this->ControlPointSize / 2.0);
}

//----------------------------------------------------------------------------
vtkObject * vtkSlicerShapeRepresentation3D::GetFirstViewNode(vtkMRMLScene* scene) const
{
  if (!scene)
  {
    return nullptr;
  }
  vtkCollection * allNodes = scene->GetNodesByClass(this->ViewNode->GetClassName());
  if (!allNodes)
  {
    return nullptr;
  }
  return allNodes->GetItemAsObject(0);
}

//---------------------------- Disk ------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateDiskFromMRML(vtkMRMLNode* caller,
                                                   unsigned long event,
                                                   void *callData /*=nullptr*/)
{
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  
  this->RadiusActor->SetVisibility(false);
  this->MiddlePointActor->SetVisibility(false);
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  
  double closestPoint[3] = { 0.0 }; // Unused here
  double farthestPoint[3] = { 0.0 };
  double innerRadius = 0.0, outerRadius = 0.0;
  if (!shapeNode->DescribeDiskPointSpacing(closestPoint, farthestPoint, innerRadius, outerRadius))
  {
    vtkDebugMacro("Point proximity description failure.");
    return;
  }
  this->ShapeMapper->SetInputConnection(this->DiskSource->GetOutputPort());
  
  double p1[3] = { 0.0 }; // center
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  shapeNode->GetNthControlPointPositionWorld(0, p1);
  shapeNode->GetNthControlPointPositionWorld(1, p2);
  shapeNode->GetNthControlPointPositionWorld(2, p3);
  
  // Relative to center
  double rp2[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
  double rp3[3] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
  
  double normal[3] = { 0.0 };
  vtkMath::Cross(rp2, rp3, normal);
  if (normal[0] == 0.0 && normal[1] == 0.0 && normal[2] == 0.0)
  {
    vtkDebugMacro("Got zero normal.");
    return;
  }
  
  this->DiskSource->SetCenter(p1);
  this->DiskSource->SetNormal(normal);
  this->DiskSource->SetOuterRadius(innerRadius);
  this->DiskSource->SetInnerRadius(outerRadius);
  
  this->DiskSource->SetCircumferentialResolution((int) shapeNode->GetResolution());
  this->DiskSource->Update();
  
  this->ShapeActor->SetVisibility(shapeNode->GetNumberOfDefinedControlPoints(true) == shapeNode->GetRequiredNumberOfControlPoints());
  this->TextActor->SetVisibility(shapeNode->GetNumberOfDefinedControlPoints(true) == shapeNode->GetRequiredNumberOfControlPoints());
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->ShapeProperty->SetOpacity(fillOpacity);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  
  this->TextActorPositionWorld[0] = farthestPoint[0];
  this->TextActorPositionWorld[1] = farthestPoint[1];
  this->TextActorPositionWorld[2] = farthestPoint[2];
  
  if (this->GetViewNode() == this->GetFirstViewNode(shapeNode->GetScene()))
  {
    shapeNode->SetShapeWorld(this->DiskSource->GetOutput());
  }
  
  this->ShapeActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
}

//---------------------------- Ring ------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateRingFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  if (!this->DoUpdateFromMRML)
  {
    return;
  }
  
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  double center[3] = {0.0};
  shapeNode->GetNthControlPointPositionWorld(0, p1);
  shapeNode->GetNthControlPointPositionWorld(1, p2);
  shapeNode->GetNthControlPointPositionWorld(2, p3);
  center[0] = (p1[0] + p2[0]) / 2.0;
  center[1] = (p1[1] + p2[1]) / 2.0;
  center[2] = (p1[2] + p2[2]) / 2.0;
  
  this->ShapeMapper->SetInputConnection(this->RingSource->GetOutputPort());
  
  double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  double normal[3] = { 0.0 };
  
  // Centered mode : p1 is center, line length is radius.
  if (shapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
  {
    // Relative to p1 (center)
    double rp2[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
    double rp3[3] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
    
    vtkMath::Cross(rp2, rp3, normal);
    if (normal[0] == 0.0 && normal[1] == 0.0 && normal[2] == 0.0)
    {
      vtkDebugMacro("Got zero normal.");
      return;
    }
    this->RingSource->SetCenter(p1);
    this->RingSource->SetNormal(normal);
    this->RingSource->SetOuterRadius(lineLength);
    this->RingSource->SetInnerRadius(lineLength - this->ViewScaleFactorMmPerPixel);
    this->RadiusSource->SetPoint1(p1);
    
    this->MiddlePointActor->SetVisibility(false);
  }
  // Circumferentiale mode : center is half way between p1 and p2, radius is half of line length.
  else
  {
    double radius = lineLength / 2.0;
    
    // relative to center
    double rp2[3] = { p2[0] - center[0], p2[1] - center[1], p2[2] - center[2] };
    double rp3[3] = { p3[0] - center[0], p3[1] - center[1], p3[2] - center[2] };
    
    vtkMath::Cross(rp2, rp3, normal);
    this->RingSource->SetCenter(center);
    this->RingSource->SetNormal(normal);
    this->RingSource->SetOuterRadius(radius);
    this->RingSource->SetInnerRadius(radius - this->ViewScaleFactorMmPerPixel);
    this->RadiusSource->SetPoint1(center);
    
    this->MiddlePointActor->SetVisibility(true);
  }
  
  this->RingSource->SetCircumferentialResolution((int) shapeNode->GetResolution());
  this->RingSource->Update();
  if (this->GetViewNode() == this->GetFirstViewNode(shapeNode->GetScene()))
  {
    shapeNode->SetShapeWorld(this->RingSource->GetOutput());
  }
  
  this->RadiusSource->SetPoint2(p2);
  this->RadiusSource->Update();
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  // Text is badly colored
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  // Stick p3 on ring.
  this->DoUpdateFromMRML = false;
  vtkIdType closestIdOnRing = this->RingSource->GetOutput()->FindPoint(p3);
  if (closestIdOnRing >= 0)
  {
    double * closestPointOnRing = this->RingSource->GetOutput()->GetPoint(closestIdOnRing);
    if (p3[0] != closestPointOnRing[0] || p3[1] != closestPointOnRing[1] || p3[2] != closestPointOnRing[2])
    {
      if (shapeNode->GetNumberOfDefinedControlPoints() == shapeNode->GetRequiredNumberOfControlPoints() && shapeNode->GetModifiedSinceRead())
      {
        shapeNode->SetNthControlPointPositionWorld(2, closestPointOnRing);
      }
    }
  }
  this->DoUpdateFromMRML = true;
  
  this->TextActorPositionWorld[0] = p3[0];
  this->TextActorPositionWorld[1] = p3[1];
  this->TextActorPositionWorld[2] = p3[2];
  
  this->ShapeActor->SetVisibility(true);
  this->RadiusActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
}

//---------------------------- Sphere ------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateSphereFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double center[3] = {0.0};
  shapeNode->GetNthControlPointPositionWorld(0, p1);
  shapeNode->GetNthControlPointPositionWorld(1, p2);
  center[0] = (p1[0] + p2[0]) / 2.0;
  center[1] = (p1[1] + p2[1]) / 2.0;
  center[2] = (p1[2] + p2[2]) / 2.0;
  
  this->ShapeMapper->SetInputConnection(this->SphereSource->GetOutputPort());
  double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  
  // Centered mode : p1 is center, line length is radius.
  if (shapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
  {
    this->SphereSource->SetCenter(p1);
    this->SphereSource->SetRadius(lineLength);
    this->RadiusSource->SetPoint1(p1);
    this->MiddlePointActor->SetVisibility(false);
  }
  // Circumferential mode : center is half way between p1 and p2, radius is half of line length.
  else
  {
    double radius = lineLength / 2.0;
    
    this->SphereSource->SetCenter(center);
    this->SphereSource->SetRadius(radius);
    this->RadiusSource->SetPoint1(center);
    this->MiddlePointActor->SetVisibility(true);
  }
  this->SphereSource->SetPhiResolution(shapeNode->GetResolution());
  this->SphereSource->SetThetaResolution(shapeNode->GetResolution());
  this->SphereSource->Update();
  if (this->GetViewNode() == this->GetFirstViewNode(shapeNode->GetScene()))
  {
    shapeNode->SetShapeWorld(this->SphereSource->GetOutput());
  }
  
  this->RadiusSource->SetPoint2(p2);
  this->RadiusSource->Update();
    
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->ShapeProperty->SetOpacity(fillOpacity);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  
  this->TextActorPositionWorld[0] = p2[0];
  this->TextActorPositionWorld[1] = p2[1];
  this->TextActorPositionWorld[2] = p2[2];

  this->ShapeActor->SetVisibility(true);
  this->RadiusActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
}

//---------------------------- Tube ------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateTubeFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  this->Superclass::UpdateFromMRML(caller, event, callData);
  this->NeedToRenderOn();
  
  this->MiddlePointActor->SetVisibility(false);
  this->RadiusActor->SetVisibility(false);
  
  vtkMRMLMarkupsShapeNode* shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  
  if (!shapeNode || shapeNode->GetNumberOfControlPoints() < 4
    || shapeNode->GetNumberOfUndefinedControlPoints() > 0)
  {
    return;
  }
  
  if (!shapeNode->GetDisplayCappedTube())
  {
    this->ShapeMapper->SetInputConnection(this->Tube->GetOutputPort());
  }
  else
  {
    this->ShapeMapper->SetInputConnection(this->CappedTube->GetOutputPort());
  }
  this->SplineActor->SetVisibility(shapeNode->GetSplineVisibility());

  int numberOfPairedControlPoints = (shapeNode->GetNumberOfControlPoints() % 2)
                            ? shapeNode->GetNumberOfControlPoints() - 1
                            : shapeNode->GetNumberOfControlPoints();

  vtkNew<vtkPoints> splinePoints;
  vtkNew<vtkTupleInterpolator> interpolatedRadius;
  interpolatedRadius->SetInterpolationTypeToLinear();
  interpolatedRadius->SetNumberOfComponents(1);
  int interpolatorIndex = 0;
  for (int i = 0; i < numberOfPairedControlPoints; i = i + 2)
  {
    double middlePoint[3] = { 0.0 };
    double p1[3] = { 0.0 };
    double p2[3] = { 0.0 };
    shapeNode->GetNthControlPointPositionWorld(i, p1);
    shapeNode->GetNthControlPointPositionWorld(i + 1, p2);
    middlePoint[0] = (p1[0] + p2[0]) / 2.0;
    middlePoint[1] = (p1[1] + p2[1]) / 2.0;
    middlePoint[2] = (p1[2] + p2[2]) / 2.0;
    splinePoints->InsertNextPoint(middlePoint);
    
    double radius = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2)) / 2.0;
    interpolatedRadius->AddTuple(interpolatorIndex, &radius);
    //interpolatedRadius->AddTuple(i + 1, &radius);
    interpolatorIndex++;
  }
  int numberOfPoints = splinePoints->GetNumberOfPoints();
  
  this->Spline->SetPoints(splinePoints);
  this->SplineFunctionSource->SetUResolution(100 * numberOfPoints);
  this->SplineFunctionSource->SetVResolution(100 * numberOfPoints);
  this->SplineFunctionSource->SetWResolution(100 * numberOfPoints);
  this->SplineFunctionSource->Update();
  vtkPolyData * splinePolyData = this->SplineFunctionSource->GetOutput();
  numberOfPoints = splinePolyData->GetNumberOfPoints();
  
  // https://kitware.github.io/vtk-examples/site/Cxx/VisualizationAlgorithms/TubesFromSplines/
  vtkSmartPointer<vtkDoubleArray> tubeRadius = vtkSmartPointer<vtkDoubleArray>::New();
  tubeRadius->SetNumberOfTuples(numberOfPoints);
  tubeRadius->SetName("TubeRadius");
  double tMin = interpolatedRadius->GetMinimumT();
  double tMax = interpolatedRadius->GetMaximumT();
  double r;
  for (unsigned int i = 0; i < numberOfPoints; ++i)
  {
    double t = (tMax - tMin) / (numberOfPoints - 1) * i + tMin;
    interpolatedRadius->InterpolateTuple(t, &r);
    tubeRadius->SetTuple1(i, r);
  }
  
  splinePolyData->GetPointData()->AddArray(tubeRadius);
  splinePolyData->GetPointData()->SetActiveScalars("TubeRadius");
  
  this->Tube->SetNumberOfSides(shapeNode->GetResolution());
  this->Tube->Update();
  this->CappedTube->SetNumberOfSides(shapeNode->GetResolution());
  this->CappedTube->Update();
  
  if (this->GetViewNode() == this->GetFirstViewNode(shapeNode->GetScene()))
  {
    shapeNode->SetShapeWorld(this->Tube->GetOutput());
    shapeNode->SetCappedTubeWorld(this->CappedTube->GetOutput());
    shapeNode->SetSplineWorld(this->SplineFunctionSource->GetOutput());
  }
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->ShapeProperty->SetOpacity(fillOpacity);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  
  double p1[3] = { 0.0 };
  shapeNode->GetNthControlPointPositionWorld(0, p1);
  this->TextActorPositionWorld[0] = p1[0];
  this->TextActorPositionWorld[1] = p1[1];
  this->TextActorPositionWorld[2] = p1[2];
  this->TextActor->SetVisibility(true);

  this->ShapeActor->SetVisibility(true);
}


//---------------------------- Cone -------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateConeFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  if (!this->DoUpdateFromMRML)
  {
    return;
  }
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  this->RadiusActor->SetVisibility(false);
  this->MiddlePointActor->SetVisibility(false);
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  shapeNode->GetNthControlPointPositionWorld(0, p1);
  shapeNode->GetNthControlPointPositionWorld(1, p2);
  shapeNode->GetNthControlPointPositionWorld(2, p3);
  
  this->ShapeMapper->SetInputConnection(this->ConeSource->GetOutputPort());
  
  double height = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p3));
  double direction[3] = { 0.0 };
  // Points towards p3
  vtkMath::Subtract(p3, p1, direction);

  double center[3] = {0.0};
  center[0] = (p1[0] + p3[0]) / 2.0;
  center[1] = (p1[1] + p3[1]) / 2.0;
  center[2] = (p1[2] + p3[2]) / 2.0;
  
  double radius = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  
  this->ConeSource->SetCenter(center);
  this->ConeSource->SetRadius(radius);
  this->ConeSource->SetHeight(height);
  this->ConeSource->SetDirection(direction);
  this->ConeSource->SetResolution(shapeNode->GetResolution());
  this->ConeSource->Update();
  
  if (this->GetViewNode() == this->GetFirstViewNode(shapeNode->GetScene()))
  {
    shapeNode->SetShapeWorld(this->ConeSource->GetOutput());
  }
  
  bool visibility = shapeNode->GetNumberOfDefinedControlPoints(true) == shapeNode->GetRequiredNumberOfControlPoints();
  this->ShapeActor->SetVisibility(visibility);
  this->TextActor->SetVisibility(visibility);
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->ShapeProperty->SetOpacity(fillOpacity);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  
  // Stick p2 on rim : place a sphere at the base, cut it and find point.
  this->DoUpdateFromMRML = false;
  vtkNew<vtkSphereSource> baseSphere;
  baseSphere->SetCenter(p1);
  baseSphere->SetRadius(radius);
  baseSphere->SetPhiResolution(360);
  baseSphere->SetThetaResolution(360);
  baseSphere->Update();
  vtkNew<vtkPlane> basePlane;
  basePlane->SetOrigin(p1);
  basePlane->SetNormal(direction);
  vtkNew<vtkCutter> baseCutter;
  baseCutter->SetInputConnection(baseSphere->GetOutputPort());
  baseCutter->SetCutFunction(basePlane);
  baseCutter->Update();
  vtkIdType closestIdOnRim = baseCutter->GetOutput()->FindPoint(p2);
  if (closestIdOnRim >= 0)
  {
    double * closestPointOnRim = baseCutter->GetOutput()->GetPoint(closestIdOnRim);
    if (p2[0] != closestPointOnRim[0] || p2[1] != closestPointOnRim[1] || p2[2] != closestPointOnRim[2])
    {
      if (shapeNode->GetNumberOfDefinedControlPoints() == shapeNode->GetRequiredNumberOfControlPoints() && shapeNode->GetModifiedSinceRead())
      {
        shapeNode->SetNthControlPointPositionWorld(1, closestPointOnRim);
      }
    }
  }
  this->DoUpdateFromMRML = true;
  
  this->TextActorPositionWorld[0] = p3[0];
  this->TextActorPositionWorld[1] = p3[1];
  this->TextActorPositionWorld[2] = p3[2];

  this->ShapeActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
}

//---------------------------- Cylinder -------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateCylinderFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  if (!this->DoUpdateFromMRML)
  {
    return;
  }
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  this->RadiusActor->SetVisibility(false);
  this->MiddlePointActor->SetVisibility(false);
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  shapeNode->GetNthControlPointPositionWorld(0, p1);
  shapeNode->GetNthControlPointPositionWorld(1, p2);
  shapeNode->GetNthControlPointPositionWorld(2, p3);
  
  this->CylinderAxis->SetPoint1(p1);
  this->CylinderAxis->SetPoint2(p3);
  this->CylinderAxis->Update();
  
  this->ShapeMapper->SetInputConnection(this->CylinderSource->GetOutputPort());
  
  double radius = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  this->CylinderSource->SetRadius(radius);
  this->CylinderSource->SetNumberOfSides(shapeNode->GetResolution());
  this->CylinderSource->Update();
  
  if (this->GetViewNode() == this->GetFirstViewNode(shapeNode->GetScene()))
  {
    shapeNode->SetShapeWorld(this->CylinderSource->GetOutput());
  }
  
  bool visibility = shapeNode->GetNumberOfDefinedControlPoints(true) == shapeNode->GetRequiredNumberOfControlPoints();
  this->ShapeActor->SetVisibility(visibility);
  this->TextActor->SetVisibility(visibility);
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->ShapeProperty->SetOpacity(fillOpacity);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  
  // Stick p2 on rim : place a sphere at the base, cut it and find point.
  this->DoUpdateFromMRML = false;
  double direction[3] = { 0.0 };
  vtkMath::Subtract(p3, p1, direction); // Points towards p3
  vtkNew<vtkSphereSource> baseSphere;
  baseSphere->SetCenter(p1);
  baseSphere->SetRadius(radius);
  baseSphere->SetPhiResolution(360);
  baseSphere->SetThetaResolution(360);
  baseSphere->Update();
  vtkNew<vtkPlane> basePlane;
  basePlane->SetOrigin(p1);
  basePlane->SetNormal(direction);
  vtkNew<vtkCutter> baseCutter;
  baseCutter->SetInputConnection(baseSphere->GetOutputPort());
  baseCutter->SetCutFunction(basePlane);
  baseCutter->Update();
  vtkIdType closestIdOnRim = baseCutter->GetOutput()->FindPoint(p2);
  if (closestIdOnRim >= 0)
  {
    double * closestPointOnRim = baseCutter->GetOutput()->GetPoint(closestIdOnRim);
    if (p2[0] != closestPointOnRim[0] || p2[1] != closestPointOnRim[1] || p2[2] != closestPointOnRim[2])
    {
      if (shapeNode->GetNumberOfDefinedControlPoints() == shapeNode->GetRequiredNumberOfControlPoints() && shapeNode->GetModifiedSinceRead())
      {
        shapeNode->SetNthControlPointPositionWorld(1, closestPointOnRim);
      }
    }
  }
  this->DoUpdateFromMRML = true;
  
  this->TextActorPositionWorld[0] = p3[0];
  this->TextActorPositionWorld[1] = p3[1];
  this->TextActorPositionWorld[2] = p3[2];

  this->ShapeActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
}

//---------------------------- Arc -------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateArcFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  if (!this->DoUpdateFromMRML)
  {
    return;
  }
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  this->RadiusActor->SetVisibility(false);
  this->MiddlePointActor->SetVisibility(false);
  /*
   * Don't include preview for ::Centered mode.
   * Else, for unknown reasons, p3 is badly placed when the node is just created,
   * p3 being very close to p2. When p3 is moved subsequently, no unexpected
   * behaviour is seen.
   */
  if (!(shapeNode->GetNumberOfDefinedControlPoints(shapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Circumferential) == shapeNode->GetRequiredNumberOfControlPoints()))
  {
    return;
  }
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  shapeNode->GetNthControlPointPositionWorld(0, p1);
  shapeNode->GetNthControlPointPositionWorld(1, p2);
  shapeNode->GetNthControlPointPositionWorld(2, p3);
  
  this->ShapeMapper->SetInputConnection(this->ArcSource->GetOutputPort());
  
  double polarVector1[3] = { 0.0 };
  double polarVector2[3] = { 0.0 }; // Nor really, but will be when repositioned.
  double normal[3] = { 0.0 }; // Normal to the plane.
  vtkMath::Subtract(p2, p1, polarVector1);
  vtkMath::Subtract(p3, p1, polarVector2);
  vtkMath::Cross(polarVector1, polarVector2, normal);
  const double angle = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(polarVector1, polarVector2));
  
  this->ArcSource->SetCenter(p1);
  this->ArcSource->SetPolarVector(polarVector1);
  this->ArcSource->SetNormal(normal);
  this->ArcSource->SetAngle(angle);
  this->ArcSource->SetResolution(shapeNode->GetResolution());
  this->ArcSource->Update();
  
  if (this->GetViewNode() == this->GetFirstViewNode(shapeNode->GetScene()))
  {
    shapeNode->SetShapeWorld(this->ArcSource->GetOutput());
  }
  
  bool visibility = shapeNode->GetNumberOfDefinedControlPoints(true) == shapeNode->GetRequiredNumberOfControlPoints();
  this->ShapeActor->SetVisibility(visibility);
  this->TextActor->SetVisibility(visibility);
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->ShapeProperty->SetOpacity(fillOpacity);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  
  // Not really 'Radius mode'.
  if (shapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
  {
    // Centre (p1) position is not calculated. p3 is calculated.
    // Stick p3 on the other end of the arc.
    double arcEndPoint[3] = { 0.0 };
    const vtkIdType arcEndPointId = this->ArcSource->GetOutput()->FindPoint(p3);
    this->ArcSource->GetOutput()->GetPoint(arcEndPointId, arcEndPoint);
    if (arcEndPoint[0] != p3[0] || arcEndPoint[1] != p3[1] || arcEndPoint[2] != p3[2])
    {
      this->DoUpdateFromMRML = false;
      shapeNode->SetNthControlPointPositionWorld(2, arcEndPoint);
      shapeNode->GetNthControlPointPositionWorld(2, p3);
      this->DoUpdateFromMRML = true;
    }
  }
  else
  {
    // Centre (p1) position is calculated. p3 is not calculated.
    // Let p1 remain the centre of the arc.
    // In-plane direction vectors.
    double binormal[3] = { 0.0 };
    double centreDirectionVector[3] = { 0.0 };
    double midPoint[3] = {(p2[0] + p3[0]) / 2.0, (p2[1] + p3[1]) / 2.0, (p2[2] + p3[2]) / 2.0};
    double tangent[3] = { 0.0 }; // From middle point between p2 and p3 to p2.
    
    vtkMath::Subtract(p2, midPoint, tangent);
    vtkMath::Subtract(p1, midPoint, centreDirectionVector);
    vtkMath::Cross(normal, tangent, binormal);
    
    double centreProjection[3] = { 0.0 }; // Project centre on binormal.
    double inPlaneCentreProjection[3] = { 0.0 };
    vtkMath::ProjectVector(centreDirectionVector, binormal, centreProjection);
    vtkMath::Add(midPoint, centreProjection, inPlaneCentreProjection);
    if (inPlaneCentreProjection[0] != p1[0] || inPlaneCentreProjection[1] != p1[1] || inPlaneCentreProjection[2] != p1[2])
    {
      this->DoUpdateFromMRML = false;
      shapeNode->SetNthControlPointPositionWorld(0, inPlaneCentreProjection);
      shapeNode->GetNthControlPointPositionWorld(0, p1);
      this->DoUpdateFromMRML = true;
    }
  }
  
  this->TextActorPositionWorld[0] = p1[0];
  this->TextActorPositionWorld[1] = p1[1];
  this->TextActorPositionWorld[2] = p1[2];

  this->ShapeActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
}

//--------------------- Ellipsoid, Toroid, and more ------------------------
void vtkSlicerShapeRepresentation3D::UpdateParametricFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  if (!this->DoUpdateFromMRML)
  {
    return;
  }
  /*
   * We use 4 markups points:
   *  - p1: centre in Centered mode; opposite to p4 and through the centre in Circumferential mode
   *  - p2: X axis
   *  - p3: Y axis
   *  - p4: Z axis; controls orientation also, like p1.
   * p2 and p3 are repositioned so that all points intersect at the centre at 90°.
   * The VTK parametric function is always computed at origin according to the orthogonal axes.
   * A transform is applied to the VTK actor to show the result within the control points.
   * The same approach is used in slice views, followed by mapping in each view.
   */
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  // Set the respective parametric function in the function source.
  switch (shapeNode->GetShapeName())
  {
    case vtkMRMLMarkupsShapeNode::Ellipsoid:
      this->ParametricFunctionSource->SetParametricFunction(this->Ellipsoid);
      break;
    case vtkMRMLMarkupsShapeNode::Toroid:
      this->ParametricFunctionSource->SetParametricFunction(this->Toroid);
      break;
    case vtkMRMLMarkupsShapeNode::BohemianDome:
      this->ParametricFunctionSource->SetParametricFunction(this->BohemianDome);
      break;
    case vtkMRMLMarkupsShapeNode::Bour:
      this->ParametricFunctionSource->SetParametricFunction(this->Bour);
      break;
    case vtkMRMLMarkupsShapeNode::Boy:
      this->ParametricFunctionSource->SetParametricFunction(this->Boy);
      break;
    case vtkMRMLMarkupsShapeNode::CrossCap:
      this->ParametricFunctionSource->SetParametricFunction(this->CrossCap);
      break;
    case vtkMRMLMarkupsShapeNode::ConicSpiral:
      this->ParametricFunctionSource->SetParametricFunction(this->ConicSpiral);
      break;
    case vtkMRMLMarkupsShapeNode::Kuen:
      this->ParametricFunctionSource->SetParametricFunction(this->Kuen);
      break;
    case vtkMRMLMarkupsShapeNode::Mobius:
      this->ParametricFunctionSource->SetParametricFunction(this->Mobius);
      break;
    case vtkMRMLMarkupsShapeNode::PluckerConoid:
      this->ParametricFunctionSource->SetParametricFunction(this->PluckerConoid);
      break;
    case vtkMRMLMarkupsShapeNode::Roman:
      this->ParametricFunctionSource->SetParametricFunction(this->Roman);
      break;
    default:
      vtkErrorMacro("Unfit shape.");
      return;
  }
    
  if (shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  this->ShapeMapper->SetInputConnection(this->ParametricFunctionSource->GetOutputPort());
  this->ParametricFunctionSource->SetScalarMode(shapeNode->GetParametricScalarMode());
  
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  double p4[3] = { 0.0 };
  double direction[3] = { 0.0 }; // p4, centre
  double center[3] = { 0.0 };
  shapeNode->GetNthControlPointPositionWorld(0, p1);
  shapeNode->GetNthControlPointPositionWorld(1, p2);
  shapeNode->GetNthControlPointPositionWorld(2, p3);
  shapeNode->GetNthControlPointPositionWorld(3, p4);
  
  if (shapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
  {
    // Centre is p1.
    vtkMath::Assign(p1, center);
    this->ParametricMiddlePointActor->SetVisibility(false);
  }
  else{
    // Centre is midway between p1 and p4.
    center[0] = (p1[0] + p4[0]) / 2.0;
    center[1] = (p1[1] + p4[1]) / 2.0;
    center[2] = (p1[2] + p4[2]) / 2.0;
    // The middle point.
    this->ParametricMiddlePointSource->SetCenter(center);
    this->ParametricMiddlePointSource->SetRadius(this->ControlPointSize / 2.0);
    this->ParametricMiddlePointSource->Update();
    this->ParametricMiddlePointActor->SetVisibility(true); // Is never shown!
  }
  vtkMath::Subtract(p4, center, direction);
  vtkMath::Normalize(direction);
  
  // Rotate {0, 0, 1} to direction[].
  double transformReferenceAxis[3] = {0.0, 0.0, 1.0}; // From GetOrientationWXYZ().
  double transformRotationAxis[3] = { 0.0 };
  vtkMath::Cross(transformReferenceAxis, direction, transformRotationAxis);
  double angleToTransformReferenceAxis = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(direction, transformReferenceAxis));
  this->ParametricTransform->Identity();
  this->ParametricTransform->RotateWXYZ(angleToTransformReferenceAxis, transformRotationAxis);
  // Place at the centre.
  this->ParametricTransform->PostMultiply();
  this->ParametricTransform->Translate(center);
  this->ParametricTransform->PreMultiply();
  
  // Calculate radii.
  double xRadius = std::sqrt(vtkMath::Distance2BetweenPoints(center, p2));
  double yRadius = std::sqrt(vtkMath::Distance2BetweenPoints(center, p3));
  double zRadius = std::sqrt(vtkMath::Distance2BetweenPoints(center, p4));
  
  // Get orthogonal axes at centre.
  double normal[3] = { 0.0 };
  double binormal[3] = { 0.0 };
  vtkMatrix4x4 * transformMatrix = this->ParametricTransform->GetMatrix();
  for (int i = 0; i < 3; i++)
  {
    normal[i] = transformMatrix->GetElement(i, 0);
    binormal[i] = transformMatrix->GetElement(i, 1);
  }
  
  // p2 and p3 will be moved; calculate their new coordinates.
  // At the centre.
  double newP2[3] = { 0.0 };
  double newP3[3] = { 0.0 };
  double modelNormal[3] = { 0.0 };
  double modelBinormal[3] = { 0.0 };
  vtkMath::Add(center, normal, modelNormal);
  vtkMath::Add(center, binormal, modelBinormal);
  // At origin.
  double origin[3] = { 0.0 };
  double unitDistance1 = std::sqrt(vtkMath::Distance2BetweenPoints(origin, normal)); // Must be 1.0 since it is a unit vector.
  double unitDistance2 = std::sqrt(vtkMath::Distance2BetweenPoints(origin, binormal));
  // At the centre.
  vtkMath::GetPointAlongLine(newP2, center, modelNormal, xRadius - unitDistance1);
  vtkMath::GetPointAlongLine(newP3, center, modelBinormal, yRadius - unitDistance2);
  
  // Create the shape at origin.
  switch (shapeNode->GetShapeName())
  {
    case vtkMRMLMarkupsShapeNode::Ellipsoid:
      this->Ellipsoid->SetZRadius(zRadius);
      this->Ellipsoid->SetYRadius(yRadius);
      this->Ellipsoid->SetXRadius(xRadius);
      this->Ellipsoid->SetN1(shapeNode->GetParametricN1());
      this->Ellipsoid->SetN2(shapeNode->GetParametricN2());
      break;
    case vtkMRMLMarkupsShapeNode::Toroid:
      // It is documented as a scaling factor.
      this->Toroid->SetZRadius(zRadius);
      this->Toroid->SetYRadius(yRadius);
      this->Toroid->SetXRadius(xRadius);
      this->Toroid->SetN1(shapeNode->GetParametricN1());
      this->Toroid->SetN2(shapeNode->GetParametricN2());
      this->Toroid->SetRingRadius(shapeNode->GetParametricRingRadius());
      this->Toroid->SetCrossSectionRadius(shapeNode->GetParametricCrossSectionRadius());
      break;
    case vtkMRMLMarkupsShapeNode::BohemianDome:
      this->BohemianDome->SetA(xRadius);
      this->BohemianDome->SetB(yRadius);
      this->BohemianDome->SetC(zRadius);
      break;
    case vtkMRMLMarkupsShapeNode::ConicSpiral:
      this->ConicSpiral->SetA(xRadius);
      this->ConicSpiral->SetB(zRadius); // Yes, to be interactively consistent.
      this->ConicSpiral->SetC(yRadius);
      this->ConicSpiral->SetN(shapeNode->GetParametricN());
      break;
    case vtkMRMLMarkupsShapeNode::PluckerConoid:
      this->PluckerConoid->SetN((int) shapeNode->GetParametricN());
      this->ParametricTransform->Scale(xRadius, yRadius, zRadius);
      break;
    case vtkMRMLMarkupsShapeNode::Roman:
      this->Roman->SetRadius(shapeNode->GetParametricRadius());
      this->ParametricTransform->Scale(xRadius, yRadius, zRadius);
      break;
    case vtkMRMLMarkupsShapeNode::Mobius:
      this->Mobius->SetRadius(shapeNode->GetParametricRadius());
      this->ParametricTransform->Scale(xRadius, yRadius, zRadius);
      break;
    case vtkMRMLMarkupsShapeNode::Kuen:
    case vtkMRMLMarkupsShapeNode::CrossCap:
    // vtkParametricBoy has a ZScale parameter with a default of 0.125.
    // We ignore it and rely on the transform's scale function for simplicity.
    // The ZScale parameter remains at 0.125.
    case vtkMRMLMarkupsShapeNode::Boy:
    case vtkMRMLMarkupsShapeNode::Bour:
      // This geometry and many others do not have their own resizing parameters.
      this->ParametricTransform->Scale(xRadius, yRadius, zRadius);
      break;
    default:
      vtkErrorMacro("Unfit shape.");
      return;
  }
  // UVW *resolution*.
  this->ParametricFunctionSource->SetUResolution(shapeNode->GetResolution());
  this->ParametricFunctionSource->SetVResolution(shapeNode->GetResolution());
  this->ParametricFunctionSource->SetWResolution(shapeNode->GetResolution());
  
  // UVW values.
  vtkParametricFunction * function = this->ParametricFunctionSource->GetParametricFunction();
  function->SetMinimumU(shapeNode->GetParametricMinimumU());
  function->SetMaximumU(shapeNode->GetParametricMaximumU());
  function->SetMinimumV(shapeNode->GetParametricMinimumV());
  function->SetMaximumV(shapeNode->GetParametricMaximumV());
  function->SetMinimumW(shapeNode->GetParametricMinimumW());
  function->SetMaximumW(shapeNode->GetParametricMaximumW());
  function->SetJoinU(shapeNode->GetParametricJoinU());
  function->SetJoinV(shapeNode->GetParametricJoinV());
  function->SetJoinW(shapeNode->GetParametricJoinW());
  function->SetTwistU(shapeNode->GetParametricTwistU());
  function->SetTwistV(shapeNode->GetParametricTwistV());
  function->SetTwistW(shapeNode->GetParametricTwistW());
  function->SetClockwiseOrdering(shapeNode->GetParametricClockwiseOrdering());
  this->ParametricFunctionSource->Update();
  
  // Block recursion while repositioning p2 and p3.
  this->DoUpdateFromMRML = false;
  shapeNode->SetNthControlPointPositionWorld(1, newP2);
  shapeNode->SetNthControlPointPositionWorld(2, newP3);
  this->DoUpdateFromMRML = true;
  
  // Move the shape from origin to the intersection (centre) of the markups points.
  this->ShapeActor->SetUserTransform(ParametricTransform);
  
  // From the first 3D view.
  if (this->GetViewNode() == this->GetFirstViewNode(shapeNode->GetScene()))
  {
    // Expose radii so that they need not be computed again.
    shapeNode->SetParametricX(xRadius, false);
    shapeNode->SetParametricY(yRadius, false);
    shapeNode->SetParametricZ(zRadius, false);
    
    // Make the shape available from outside already repositioned.
    this->ParametricTransformer->SetInputConnection(this->ParametricFunctionSource->GetOutputPort());
    this->ParametricTransformer->Update();
    shapeNode->SetShapeWorld(this->ParametricTransformer->GetOutput());
  }
  
  this->ShapeActor->SetVisibility(shapeNode->GetNumberOfDefinedControlPoints(true) == shapeNode->GetRequiredNumberOfControlPoints());
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->ShapeProperty->SetOpacity(fillOpacity);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  
  this->TextActor->SetVisibility(shapeNode->GetNumberOfDefinedControlPoints(true) == shapeNode->GetRequiredNumberOfControlPoints());
  this->TextActorPositionWorld[0] = center[0];
  this->TextActorPositionWorld[1] = center[1];
  this->TextActorPositionWorld[2] = center[2];

  this->ShapeActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
}
