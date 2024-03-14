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
  this->ShapeMapper->SetScalarVisibility(false);
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
  
  this->Spline = vtkSmartPointer<vtkParametricSpline>::New();
  vtkNew<vtkPoints> points;
  const double point[3] = { 0.0 };
  points->InsertNextPoint(point);
  this->Spline->SetPoints(points);
  this->SplineFunctionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
  this->SplineFunctionSource->SetParametricFunction(this->Spline);
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
  
  this->CameraModifiedCallbackCommand = vtkSmartPointer<vtkCallbackCommand>::New();
  this->CameraModifiedCallbackCommand->SetClientData( reinterpret_cast<void *>(this) );
  this->CameraModifiedCallbackCommand->SetCallback( vtkSlicerShapeRepresentation3D::OnCameraModified );
}

//------------------------------------------------------------------------------
vtkSlicerShapeRepresentation3D::~vtkSlicerShapeRepresentation3D()
{
  if (this->CameraIsBeingObserved)
  {
    this->GetRenderer()->GetActiveCamera()->RemoveObserver(this->CameraModifiedCallbackCommand);
  }
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
  if (!this->CameraIsBeingObserved)
  {
    this->GetRenderer()->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, this->CameraModifiedCallbackCommand);
    this->CameraIsBeingObserved = true;
  }
  
  if (shapeNode->GetNumberOfControlPoints() < shapeNode->GetRequiredNumberOfControlPoints())
  {
    this->ShapeActor->SetVisibility(false);
    this->MiddlePointActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
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

//---------------------------------------------------------------------------
void vtkSlicerShapeRepresentation3D::OnCameraModified(vtkObject *caller,
                                                      unsigned long event, void *clientData, void *callData)
{
  vtkSlicerShapeRepresentation3D * client = reinterpret_cast<vtkSlicerShapeRepresentation3D*>(clientData);
  if (!client)
  {
    return;
  }
  client->UpdateFromMRML(NULL, 0);
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
  
  this->ShapeActor->SetVisibility(shapeNode->GetNumberOfDefinedControlPoints(true) == 3);
  this->TextActor->SetVisibility(shapeNode->GetNumberOfDefinedControlPoints(true) == 3);
  
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
  
  visibility = shapeNode->GetNumberOfDefinedControlPoints(true) == 3;
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
      if (shapeNode->GetNumberOfDefinedControlPoints() == 3 && shapeNode->GetModifiedSinceRead())
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
  if (this->GetViewNode() == this->GetFirstViewNode(shapeNode->GetScene()))
  {
    shapeNode->SetShapeWorld(this->SphereSource->GetOutput());
  }
  
  this->RadiusSource->SetPoint2(p2);
  this->RadiusSource->Update();
  
  bool visibility = shapeNode->GetNumberOfDefinedControlPoints(true) == 2;
  this->ShapeActor->SetVisibility(visibility);
  this->RadiusActor->SetVisibility(visibility);
  this->TextActor->SetVisibility(visibility);
  
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

//---------------------------- Tube ------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateTubeFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  this->Superclass::UpdateFromMRML(caller, event, callData);
  this->NeedToRenderOn();
  
  this->MiddlePointActor->SetVisibility(false);
  this->RadiusActor->SetVisibility(false);
  
  vtkMRMLMarkupsShapeNode* shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  
  if (!shapeNode || shapeNode->GetNumberOfControlPoints() < 4
    || shapeNode->GetNumberOfUndefinedControlPoints() > 0
    || (shapeNode->GetNumberOfControlPoints() % 2) != 0) // Complete point pairs required.
  {
    return;
  }
  
  this->TextActor->SetVisibility(true);
  if (!shapeNode->GetDisplayCappedTube())
  {
    this->ShapeMapper->SetInputConnection(this->Tube->GetOutputPort());
  }
  else
  {
    this->ShapeMapper->SetInputConnection(this->CappedTube->GetOutputPort());
  }
  
  vtkNew<vtkPoints> splinePoints;
  vtkNew<vtkTupleInterpolator> interpolatedRadius;
  interpolatedRadius->SetInterpolationTypeToLinear();
  interpolatedRadius->SetNumberOfComponents(1);
  int interpolatorIndex = 0;
  for (int i = 0; i < shapeNode->GetNumberOfControlPoints(); i = i + 2)
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
  this->ShapeActor->SetVisibility(true);
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
  if (!(shapeNode->GetNumberOfDefinedControlPoints(true) == 3))
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
  
  bool visibility = shapeNode->GetNumberOfDefinedControlPoints(true) == 3;
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
      if (shapeNode->GetNumberOfDefinedControlPoints() == 3 && shapeNode->GetModifiedSinceRead())
      {
        shapeNode->SetNthControlPointPositionWorld(1, closestPointOnRim);
      }
    }
  }
  this->DoUpdateFromMRML = true;
  
  this->TextActorPositionWorld[0] = p3[0];
  this->TextActorPositionWorld[1] = p3[1];
  this->TextActorPositionWorld[2] = p3[2];
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
  if (!(shapeNode->GetNumberOfDefinedControlPoints(true) == 3))
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
  
  bool visibility = shapeNode->GetNumberOfDefinedControlPoints(true) == 3;
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
      if (shapeNode->GetNumberOfDefinedControlPoints() == 3 && shapeNode->GetModifiedSinceRead())
      {
        shapeNode->SetNthControlPointPositionWorld(1, closestPointOnRim);
      }
    }
  }
  this->DoUpdateFromMRML = true;
  
  this->TextActorPositionWorld[0] = p3[0];
  this->TextActorPositionWorld[1] = p3[1];
  this->TextActorPositionWorld[2] = p3[2];
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
  if (!(shapeNode->GetNumberOfDefinedControlPoints(shapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Circumferential) == 3))
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
  
  bool visibility = shapeNode->GetNumberOfDefinedControlPoints(true) == 3;
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
}
