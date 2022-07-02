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

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerShapeRepresentation3D);

//------------------------------------------------------------------------------
vtkSlicerShapeRepresentation3D::vtkSlicerShapeRepresentation3D()
{
  this->DiskSource = vtkSmartPointer<vtkDiskSource>::New();
  this->RingSource = vtkSmartPointer<vtkDiskSource>::New();
  this->RadiusSource = vtkSmartPointer<vtkLineSource>::New();
  this->SphereSource = vtkSmartPointer<vtkSphereSource>::New();
  
  this->ShapeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  //this->ShapeMapper->SetInputConnection(this->DiskSource->GetOutputPort());
  this->ShapeMapper->SetScalarVisibility(true);
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
  this->RadiusMapper->SetScalarVisibility(true);
  this->RadiusActor = vtkSmartPointer<vtkActor>::New();
  this->RadiusActor->SetMapper(this->RadiusMapper);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
}

//------------------------------------------------------------------------------
vtkSlicerShapeRepresentation3D::~vtkSlicerShapeRepresentation3D() = default;

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
  this->RadiusActor->GetActors(pc);
}

//------------------------------------------------------------------------------
void vtkSlicerShapeRepresentation3D::ReleaseGraphicsResources(vtkWindow* win)
{
  this->Superclass::ReleaseGraphicsResources(win);
  this->ShapeActor->ReleaseGraphicsResources(win);
  this->MiddlePointActor->ReleaseGraphicsResources(win);
  this->RadiusActor->ReleaseGraphicsResources(win);
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
  if (this->RadiusActor->GetVisibility())
  {
    count += this->RadiusActor->RenderOverlay(viewport);
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
  if (this->RadiusActor->GetVisibility())
  {
    count += this->RadiusActor->RenderOpaqueGeometry(viewport);
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
  if (this->RadiusActor->GetVisibility())
  {
    this->RadiusActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->RadiusActor->RenderTranslucentPolygonalGeometry(viewport);
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
  if (this->RadiusActor->GetVisibility() &&
    this->RadiusActor->HasTranslucentPolygonalGeometry())
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
    default:
      vtkErrorMacro("Unknown shape.");
      return;
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
  this->MiddlePointSource->SetRadius(this->ControlPointSize);
}

//---------------------------- Disk ------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateDiskFromMRML(vtkMRMLNode* caller,
                                                   unsigned long event,
                                                   void *callData /*=nullptr*/)
{
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  
  this->ShapeActor->SetVisibility(shapeNode->GetNumberOfDefinedControlPoints(true) == 3);
  this->TextActor->SetVisibility(shapeNode->GetNumberOfDefinedControlPoints(true) == 3);
  this->RadiusActor->SetVisibility(false);
  this->MiddlePointActor->SetVisibility(false);
  
  double closestPoint[3] = { 0.0 }; // Unused here
  double farthestPoint[3] = { 0.0 };
  double innerRadius = 0.0, outerRadius = 0.0;
  if (!shapeNode->DescribeDiskPointSpacing(closestPoint, farthestPoint, innerRadius, outerRadius))
  {
    vtkDebugMacro("Point proximity description failure.");
    return;
  }
  this->ShapeMapper->SetInputConnection(this->DiskSource->GetOutputPort());
  
  if (shapeNode->GetNumberOfDefinedControlPoints(true) == 3)
  {
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
  }
  else
  {
    this->ShapeActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
  }
  
  this->DiskSource->SetCircumferentialResolution((int) shapeNode->GetResolution());
  this->DiskSource->Update();
  
  this->ShapeActor->SetVisibility(this->GetAllControlPointsVisible() && shapeNode->GetNumberOfDefinedControlPoints(true) == 3);
  this->TextActor->SetVisibility(this->GetAllControlPointsVisible() && shapeNode->GetNumberOfDefinedControlPoints(true) == 3);
  
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
  
  shapeNode->SetShapeWorld(this->DiskSource->GetOutput());
}

//---------------------------- Ring ------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateRingFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  if (!this->DoUpdateFromMRML)
  {
    return;
  }
  
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  
  bool visibility = shapeNode->GetNumberOfDefinedControlPoints(true) == 3;
  this->ShapeActor->SetVisibility(visibility);
  this->TextActor->SetVisibility(visibility);
  this->RadiusActor->SetVisibility(visibility);
  this->MiddlePointActor->SetVisibility(visibility);
  
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
  
  this->MiddlePointSource->SetCenter(center);
  this->MiddlePointSource->SetRadius(this->ControlPointSize);
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
    this->RingSource->SetInnerRadius(lineLength - 1.0);
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
    this->RingSource->SetInnerRadius(radius - 1.0);
    this->RadiusSource->SetPoint1(center);
    
    this->MiddlePointActor->SetVisibility(true);
  }
  
  this->RingSource->SetCircumferentialResolution((int) shapeNode->GetResolution());
  this->RingSource->Update();
  shapeNode->SetShapeWorld(this->RingSource->GetOutput());
  
  this->RadiusSource->SetPoint2(p2);
  this->RadiusSource->Update();
  
  visibility = (this->GetAllControlPointsVisible() && shapeNode->GetNumberOfDefinedControlPoints(true) == 3);
  this->ShapeActor->SetVisibility(visibility);
  this->RadiusActor->SetVisibility(visibility);
  this->TextActor->SetVisibility(visibility);
  
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
      if (shapeNode->GetNumberOfDefinedControlPoints() == 3)
      {
        shapeNode->SetNthControlPointPositionWorld(2, closestPointOnRing);
      }
    }
  }
  this->DoUpdateFromMRML = true;
  
  this->TextActorPositionWorld[0] = p3[0];
  this->TextActorPositionWorld[1] = p3[1];
  this->TextActorPositionWorld[2] = p3[2];
}

//---------------------------- Sphere ------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateSphereFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  
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
  shapeNode->SetShapeWorld(this->SphereSource->GetOutput());
  
  this->RadiusSource->SetPoint2(p2);
  this->RadiusSource->Update();
  
  this->ShapeActor->SetVisibility(this->GetAllControlPointsVisible() && shapeNode->GetNumberOfDefinedControlPoints(true) == 2);
  this->RadiusActor->SetVisibility(this->GetAllControlPointsVisible() && shapeNode->GetNumberOfDefinedControlPoints(true) == 2);
  this->TextActor->SetVisibility(this->GetAllControlPointsVisible() && shapeNode->GetNumberOfDefinedControlPoints(true) == 2);
  
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
}
