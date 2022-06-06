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

#include "vtkSlicerSphereRepresentation3D.h"

#include "vtkMRMLMarkupsSphereNode.h"

// VTK includes
#include <vtkActor.h>
#include <vtkCutter.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerSphereRepresentation3D);

//------------------------------------------------------------------------------
vtkSlicerSphereRepresentation3D::vtkSlicerSphereRepresentation3D()
{

  this->SlicingPlane = vtkSmartPointer<vtkPlane>::New();

  this->MiddlePointSource = vtkSmartPointer<vtkSphereSource>::New();

  this->MiddlePointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->MiddlePointMapper->SetInputConnection(this->MiddlePointSource->GetOutputPort());

  this->MiddlePointActor = vtkSmartPointer<vtkActor>::New();
  this->MiddlePointActor->SetMapper(this->MiddlePointMapper);
  
  this->SphereSource = vtkSmartPointer<vtkSphereSource>::New();
  
  this->SphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->SphereMapper->SetInputConnection(this->SphereSource->GetOutputPort());
  this->SphereMapper->SetScalarVisibility(true);
  
  this->SphereActor = vtkSmartPointer<vtkActor>::New();
  this->SphereActor->SetMapper(this->SphereMapper);
  this->SphereActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
  
  this->RadiusSource = vtkSmartPointer<vtkLineSource>::New();
  
  this->RadiusMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->RadiusMapper->SetInputConnection(this->RadiusSource->GetOutputPort());
  this->RadiusMapper->SetScalarVisibility(true);
  
  this->RadiusActor = vtkSmartPointer<vtkActor>::New();
  this->RadiusActor->SetMapper(this->RadiusMapper);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
  
  this->SphereProperty = vtkSmartPointer<vtkProperty>::New();
  this->SphereProperty->DeepCopy(this->GetControlPointsPipeline(Selected)->Property);
}

//------------------------------------------------------------------------------
vtkSlicerSphereRepresentation3D::~vtkSlicerSphereRepresentation3D() = default;

//------------------------------------------------------------------------------
void vtkSlicerSphereRepresentation3D::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
  if (this->SphereActor)
  {
    os << indent << "Sphere Visibility: " << this->SphereActor->GetVisibility() << "\n";
  }
  else
  {
    os << indent << "Sphere Visibility: (none)\n";
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
void vtkSlicerSphereRepresentation3D::GetActors(vtkPropCollection* pc)
{
  this->Superclass::GetActors(pc);
  this->MiddlePointActor->GetActors(pc);
  this->SphereActor->GetActors(pc);
  this->RadiusActor->GetActors(pc);
  this->TextActor->GetActors(pc);
}

//------------------------------------------------------------------------------
void vtkSlicerSphereRepresentation3D::ReleaseGraphicsResources(vtkWindow* win)
{
  this->Superclass::ReleaseGraphicsResources(win);
  this->MiddlePointActor->ReleaseGraphicsResources(win);
  this->SphereActor->ReleaseGraphicsResources(win);
  this->RadiusActor->ReleaseGraphicsResources(win);
  this->TextActor->ReleaseGraphicsResources(win);
}

//------------------------------------------------------------------------------
int vtkSlicerSphereRepresentation3D::RenderOverlay(vtkViewport* viewport)
{
  int count = this->Superclass::RenderOverlay(viewport);
  
  if (this->MiddlePointActor->GetVisibility())
    {
    count += this->MiddlePointActor->RenderOverlay(viewport);
    }
  if (this->SphereActor->GetVisibility())
    {
      count +=  this->SphereActor->RenderOverlay(viewport);
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
int vtkSlicerSphereRepresentation3D::RenderOpaqueGeometry(vtkViewport* viewport)
{
  int count = this->Superclass::RenderOpaqueGeometry(viewport);

  if (this->MiddlePointActor->GetVisibility())
    {
    count += this->MiddlePointActor->RenderOpaqueGeometry(viewport);
    }
  if (this->SphereActor->GetVisibility())
    {
      count += this->SphereActor->RenderOpaqueGeometry(viewport);
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
int vtkSlicerSphereRepresentation3D::RenderTranslucentPolygonalGeometry(vtkViewport* viewport)
{
  int count = this->Superclass::RenderTranslucentPolygonalGeometry(viewport);

  if (this->MiddlePointActor->GetVisibility())
    {
    this->MiddlePointActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->MiddlePointActor->RenderTranslucentPolygonalGeometry(viewport);
    }
  if (this->SphereActor->GetVisibility())
    {
      this->SphereActor->SetPropertyKeys(this->GetPropertyKeys());
      count += this->SphereActor->RenderTranslucentPolygonalGeometry(viewport);
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
vtkTypeBool vtkSlicerSphereRepresentation3D::HasTranslucentPolygonalGeometry()
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
  if (this->SphereActor->GetVisibility() &&
      this->SphereActor->HasTranslucentPolygonalGeometry())
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
void vtkSlicerSphereRepresentation3D::UpdateFromMRML(vtkMRMLNode* caller,
                                                           unsigned long event,
                                                           void *callData /*=nullptr*/)
{
  this->Superclass::UpdateFromMRML(caller, event, callData);

  this->NeedToRenderOn();

  vtkMRMLMarkupsSphereNode* sphereNode=
    vtkMRMLMarkupsSphereNode::SafeDownCast(this->GetMarkupsNode());

  if (!sphereNode || sphereNode->GetNumberOfDefinedControlPoints() != 2)
    {
    return;
    }

  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double center[3] = {0.0};
  sphereNode->GetNthControlPointPositionWorld(0, p1);
  sphereNode->GetNthControlPointPositionWorld(1, p2);
  center[0] = (p1[0] + p2[0]) / 2.0;
  center[1] = (p1[1] + p2[1]) / 2.0;
  center[2] = (p1[2] + p2[2]) / 2.0;
  
  this->MiddlePointSource->SetCenter(center);
  this->MiddlePointSource->SetRadius(this->ControlPointSize);
  
  double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  
  // Centered mode : p1 is center, line length is radius.
  if (sphereNode->GetRadiusMode() == vtkMRMLMarkupsSphereNode::Centered)
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
  this->SphereSource->SetPhiResolution(sphereNode->GetResolution());
  this->SphereSource->SetThetaResolution(sphereNode->GetResolution());
  this->SphereSource->Update();
  sphereNode->SetSphereWorld(this->SphereSource->GetOutput());
  
  this->RadiusSource->SetPoint2(p2);
  this->RadiusSource->Update();
  
  this->SphereActor->SetVisibility(this->GetAllControlPointsVisible() && sphereNode->GetNumberOfDefinedControlPoints(true) == 2);
  this->RadiusActor->SetVisibility(this->GetAllControlPointsVisible() && sphereNode->GetNumberOfDefinedControlPoints(true) == 2);
  this->TextActor->SetVisibility(this->GetAllControlPointsVisible() && sphereNode->GetNumberOfDefinedControlPoints(true) == 2);
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->SphereActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->SphereProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->SphereProperty->SetOpacity(fillOpacity);
  this->SphereActor->SetProperty(this->SphereProperty);
  
  this->TextActorPositionWorld[0] = p2[0];
  this->TextActorPositionWorld[1] = p2[1];
  this->TextActorPositionWorld[2] = p2[2];
}
