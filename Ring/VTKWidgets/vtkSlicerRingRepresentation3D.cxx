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

#include "vtkSlicerRingRepresentation3D.h"

#include "vtkMRMLMarkupsRingNode.h"

// VTK includes
#include <vtkActor.h>
#include <vtkCutter.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerRingRepresentation3D);

//------------------------------------------------------------------------------
vtkSlicerRingRepresentation3D::vtkSlicerRingRepresentation3D()
{
  this->SlicingPlane = vtkSmartPointer<vtkPlane>::New();

  this->MiddlePointSource = vtkSmartPointer<vtkSphereSource>::New();

  this->MiddlePointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->MiddlePointMapper->SetInputConnection(this->MiddlePointSource->GetOutputPort());

  this->MiddlePointActor = vtkSmartPointer<vtkActor>::New();
  this->MiddlePointActor->SetMapper(this->MiddlePointMapper);
  
  this->RingSource = vtkSmartPointer<vtkDiskSource>::New();
  
  this->RingMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->RingMapper->SetInputConnection(this->RingSource->GetOutputPort());
  this->RingMapper->SetScalarVisibility(true);
  
  this->RingActor = vtkSmartPointer<vtkActor>::New();
  this->RingActor->SetMapper(this->RingMapper);
  this->RingActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
  
  this->RadiusSource = vtkSmartPointer<vtkLineSource>::New();
  
  this->RadiusMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->RadiusMapper->SetInputConnection(this->RadiusSource->GetOutputPort());
  this->RadiusMapper->SetScalarVisibility(true);
  
  this->RadiusActor = vtkSmartPointer<vtkActor>::New();
  this->RadiusActor->SetMapper(this->RadiusMapper);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
}

//------------------------------------------------------------------------------
vtkSlicerRingRepresentation3D::~vtkSlicerRingRepresentation3D() = default;

//------------------------------------------------------------------------------
void vtkSlicerRingRepresentation3D::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
  if (this->RingActor)
    {
      os << indent << "Ring Visibility: " << this->RingActor->GetVisibility() << "\n";
    }
  else
    {
      os << indent << "Ring Visibility: (none)\n";
    }
  if (this->RadiusActor)
    {
      os << indent << "Radius Visibility: " << this->RadiusActor->GetVisibility() << "\n";
    }
    else
    {
      os << indent << "Radius Visibility: (none)\n";
    }
}

//------------------------------------------------------------------------------
void vtkSlicerRingRepresentation3D::GetActors(vtkPropCollection* pc)
{
  this->Superclass::GetActors(pc);

  this->MiddlePointActor->GetActors(pc);
  this->RingActor->GetActors(pc);
  this->RadiusActor->GetActors(pc);
  this->TextActor->GetActors(pc);
}

//------------------------------------------------------------------------------
void vtkSlicerRingRepresentation3D::ReleaseGraphicsResources(vtkWindow* win)
{
  this->Superclass::ReleaseGraphicsResources(win);

  this->MiddlePointActor->ReleaseGraphicsResources(win);
  this->RingActor->ReleaseGraphicsResources(win);
  this->RadiusActor->ReleaseGraphicsResources(win);
  this->TextActor->ReleaseGraphicsResources(win);
}

//------------------------------------------------------------------------------
int vtkSlicerRingRepresentation3D::RenderOverlay(vtkViewport* viewport)
{
  int count = this->Superclass::RenderOverlay(viewport);
  if (this->MiddlePointActor->GetVisibility())
    {
    count += this->MiddlePointActor->RenderOverlay(viewport);
    }

  if (this->RingActor->GetVisibility())
    {
      count +=  this->RingActor->RenderOverlay(viewport);
    }
  
  if (this->RadiusActor->GetVisibility())
  {
    count +=  this->RadiusActor->RenderOverlay(viewport);
  }
  if (this->TextActor->GetVisibility())
  {
    count +=  this->TextActor->RenderOverlay(viewport);
  }
  
  return count;
}

//------------------------------------------------------------------------------
int vtkSlicerRingRepresentation3D::RenderOpaqueGeometry(vtkViewport* viewport)
{
  int count = this->Superclass::RenderOpaqueGeometry(viewport);
  if (this->MiddlePointActor->GetVisibility())
    {
    count += this->MiddlePointActor->RenderOpaqueGeometry(viewport);
    }

  if (this->RingActor->GetVisibility())
    {
      count += this->RingActor->RenderOpaqueGeometry(viewport);
    }
  
  if (this->RadiusActor->GetVisibility())
  {
    count += this->RadiusActor->RenderOpaqueGeometry(viewport);
  }
  
  if (this->TextActor->GetVisibility())
  {
    count += this->TextActor->RenderOpaqueGeometry(viewport);
  }
  
  return count;
}

//------------------------------------------------------------------------------
int vtkSlicerRingRepresentation3D::RenderTranslucentPolygonalGeometry(vtkViewport* viewport)
{
  int count = this->Superclass::RenderTranslucentPolygonalGeometry(viewport);
  if (this->MiddlePointActor->GetVisibility())
    {
    this->MiddlePointActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->MiddlePointActor->RenderTranslucentPolygonalGeometry(viewport);
    }

  if (this->RingActor->GetVisibility())
    {
      this->RingActor->SetPropertyKeys(this->GetPropertyKeys());
      count += this->RingActor->RenderTranslucentPolygonalGeometry(viewport);
    }
  
  if (this->RadiusActor->GetVisibility())
  {
    this->RadiusActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->RadiusActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  
  if (this->TextActor->GetVisibility())
  {
    this->TextActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->TextActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  
  return count;
}

//------------------------------------------------------------------------------
vtkTypeBool vtkSlicerRingRepresentation3D::HasTranslucentPolygonalGeometry()
{
  if (this->Superclass::HasTranslucentPolygonalGeometry())
    {
    return true;
    }

  if (this->MiddlePointActor->GetVisibility() &&
      this->MiddlePointActor->HasTranslucentPolygonalGeometry())
    {
    return true;
    }
  
  if (this->RingActor->GetVisibility() &&
      this->RingActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }

  if (this->RadiusActor->GetVisibility() &&
    this->RadiusActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }
  
  if (this->TextActor->GetVisibility() &&
    this->TextActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }
  
  return false;
}

//----------------------------------------------------------------------
void vtkSlicerRingRepresentation3D::UpdateFromMRML(vtkMRMLNode* caller,
                                                           unsigned long event,
                                                           void *callData /*=nullptr*/)
{
  this->Superclass::UpdateFromMRML(caller, event, callData);

  this->NeedToRenderOn();
  
  if (!this->DoUpdateFromMRML)
  {
    return;
  }

  vtkMRMLMarkupsNode* markupsNode = this->GetMarkupsNode();
  if (!markupsNode || markupsNode->GetNumberOfDefinedControlPoints() != 3)
  {
    return;
  }

  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  double center[3] = {0.0};
  markupsNode->GetNthControlPointPositionWorld(0, p1);
  markupsNode->GetNthControlPointPositionWorld(1, p2);
  markupsNode->GetNthControlPointPositionWorld(2, p3);
  center[0] = (p1[0] + p2[0]) / 2.0;
  center[1] = (p1[1] + p2[1]) / 2.0;
  center[2] = (p1[2] + p2[2]) / 2.0;

  this->MiddlePointSource->SetCenter(center);
  this->MiddlePointSource->SetRadius(this->ControlPointSize);

  double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  vtkMRMLMarkupsRingNode * ringNode = vtkMRMLMarkupsRingNode::SafeDownCast(markupsNode);
  double normal[3] = { 0.0 };

  // Centered mode : p1 is center, line length is radius.
  if (ringNode->GetMode() == vtkMRMLMarkupsRingNode::Centered)
  {
    // Relative to p1 (center)
    double rp2[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
    double rp3[3] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
    
    vtkMath::Cross(rp2, rp3, normal);
    if (normal[0] == 0.0 && normal[1] == 0.0 && normal[2] == 0.0)
    {
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

  this->RingSource->SetCircumferentialResolution((int) ringNode->GetResolution());
  this->RingSource->Update();
  ringNode->SetRingWorld(this->RingSource->GetOutput());
  
  this->RadiusSource->SetPoint2(p2);
  this->RadiusSource->Update();
  
  this->RingActor->SetVisibility(this->GetAllControlPointsVisible() && markupsNode->GetNumberOfDefinedControlPoints(true) == 3);
  this->RadiusActor->SetVisibility(this->GetAllControlPointsVisible() && markupsNode->GetNumberOfDefinedControlPoints(true) == 3);
  this->TextActor->SetVisibility(this->GetAllControlPointsVisible() && markupsNode->GetNumberOfDefinedControlPoints(true) == 3);

  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->RingActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  // Text is badly colored
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  // Stick p3 on ring.
  this->DoUpdateFromMRML = false;
  vtkIdType closestIdOnRing = this->RingSource->GetOutput()->FindPoint(p3);
  double * closestPointOnRing = this->RingSource->GetOutput()->GetPoint(closestIdOnRing);
  ringNode->SetNthControlPointPositionWorld(2, closestPointOnRing);
  this->DoUpdateFromMRML = true;
  
  this->TextActorPositionWorld[0] = p3[0];
  this->TextActorPositionWorld[1] = p3[1];
  this->TextActorPositionWorld[2] = p3[2];
}
