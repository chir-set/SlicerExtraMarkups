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

#include "vtkSlicerDiskRepresentation3D.h"

#include "vtkMRMLMarkupsDiskNode.h"

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkProperty.h>
#include <vtkSetGet.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerDiskRepresentation3D);

//------------------------------------------------------------------------------
vtkSlicerDiskRepresentation3D::vtkSlicerDiskRepresentation3D()
{
  this->DiskSource = vtkSmartPointer<vtkDiskSource>::New();
  
  this->DiskMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->DiskMapper->SetInputConnection(this->DiskSource->GetOutputPort());
  this->DiskMapper->SetScalarVisibility(true);
  
  this->DiskProperty = vtkSmartPointer<vtkProperty>::New();
  this->DiskProperty->DeepCopy(this->GetControlPointsPipeline(Selected)->Property);
  
  this->DiskActor = vtkSmartPointer<vtkActor>::New();
  this->DiskActor->SetMapper(this->DiskMapper);
  this->DiskActor->SetProperty(this->DiskProperty);
  
  //this->TextActor->SetTextProperty(this->GetControlPointsPipeline(Unselected)->TextProperty);
}

//------------------------------------------------------------------------------
vtkSlicerDiskRepresentation3D::~vtkSlicerDiskRepresentation3D() = default;

//------------------------------------------------------------------------------
void vtkSlicerDiskRepresentation3D::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
  if (this->DiskActor)
  {
    os << indent << "Disk Visibility: " << this->DiskActor->GetVisibility() << "\n";
  }
  else
  {
    os << indent << "Disk Visibility: (none)\n";
  }
}

//------------------------------------------------------------------------------
void vtkSlicerDiskRepresentation3D::GetActors(vtkPropCollection* pc)
{
  this->Superclass::GetActors(pc);

  this->DiskActor->GetActors(pc);
  this->TextActor->GetActors(pc);
}

//------------------------------------------------------------------------------
void vtkSlicerDiskRepresentation3D::ReleaseGraphicsResources(vtkWindow* win)
{
  this->Superclass::ReleaseGraphicsResources(win);

  this->DiskActor->ReleaseGraphicsResources(win);
  this->TextActor->ReleaseGraphicsResources(win);
}

//------------------------------------------------------------------------------
int vtkSlicerDiskRepresentation3D::RenderOverlay(vtkViewport* viewport)
{
  int count = this->Superclass::RenderOverlay(viewport);
  if (this->DiskActor->GetVisibility())
  {
    count +=  this->DiskActor->RenderOverlay(viewport);
  }
  if (this->TextActor->GetVisibility())
  {
    count +=  this->TextActor->RenderOverlay(viewport);
  }
  
  return count;
}

//------------------------------------------------------------------------------
int vtkSlicerDiskRepresentation3D::RenderOpaqueGeometry(vtkViewport* viewport)
{
  int count = this->Superclass::RenderOpaqueGeometry(viewport);

  if (this->DiskActor->GetVisibility())
  {
    count += this->DiskActor->RenderOpaqueGeometry(viewport);
  }
  if (this->TextActor->GetVisibility())
  {
    count += this->TextActor->RenderOpaqueGeometry(viewport);
  }

  return count;
}

//------------------------------------------------------------------------------
int vtkSlicerDiskRepresentation3D::RenderTranslucentPolygonalGeometry(vtkViewport* viewport)
{
  int count = this->Superclass::RenderTranslucentPolygonalGeometry(viewport);

  if (this->DiskActor->GetVisibility())
  {
    this->DiskActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->DiskActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->TextActor->GetVisibility())
  {
    this->TextActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->TextActor->RenderTranslucentPolygonalGeometry(viewport);
  }

  return count;
}

//------------------------------------------------------------------------------
vtkTypeBool vtkSlicerDiskRepresentation3D::HasTranslucentPolygonalGeometry()
{
  if (this->Superclass::HasTranslucentPolygonalGeometry())
    {
    return true;
    }

  if (this->DiskActor->GetVisibility() &&
      this->DiskActor->HasTranslucentPolygonalGeometry())
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
void vtkSlicerDiskRepresentation3D::UpdateFromMRML(vtkMRMLNode* caller,
                                                           unsigned long event,
                                                           void *callData /*=nullptr*/)
{
  this->Superclass::UpdateFromMRML(caller, event, callData);

  this->NeedToRenderOn();

  vtkMRMLMarkupsDiskNode* markupsNode=
    vtkMRMLMarkupsDiskNode::SafeDownCast(this->GetMarkupsNode());

  if (!markupsNode)
    {
    return;
    }

  vtkMRMLMarkupsDiskNode * diskNode = vtkMRMLMarkupsDiskNode::SafeDownCast(markupsNode);
 
  this->DiskActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 3);
  this->TextActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 3);
  
  double closestPoint[3] = { 0.0 }; // Unused here
  double farthestPoint[3] = { 0.0 };
  double innerRadius = 0.0, outerRadius = 0.0;
  if (!this->DescribePointsProximity(closestPoint, farthestPoint, innerRadius, outerRadius))
  {
    vtkDebugMacro("Point proximity description failure.");
    return;
  }
  
  if (markupsNode->GetNumberOfDefinedControlPoints(true) == 3)
  {
    double p1[3] = { 0.0 }; // center
    double p2[3] = { 0.0 };
    double p3[3] = { 0.0 };
    diskNode->GetNthControlPointPositionWorld(0, p1);
    diskNode->GetNthControlPointPositionWorld(1, p2);
    diskNode->GetNthControlPointPositionWorld(2, p3);
    
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
    this->DiskActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
  }
  
  this->DiskSource->SetCircumferentialResolution((int) diskNode->GetResolution());
  this->DiskSource->Update();
  
  this->DiskActor->SetVisibility(this->GetAllControlPointsVisible() && markupsNode->GetNumberOfDefinedControlPoints(true) == 3);
  this->TextActor->SetVisibility(this->GetAllControlPointsVisible() && markupsNode->GetNumberOfDefinedControlPoints(true) == 3);
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->DiskActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->DiskProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->DiskProperty->SetOpacity(fillOpacity);
  this->DiskActor->SetProperty(this->DiskProperty);
  
  this->TextActorPositionWorld[0] = farthestPoint[0];
  this->TextActorPositionWorld[1] = farthestPoint[1];
  this->TextActorPositionWorld[2] = farthestPoint[2];
  
  diskNode->SetDiskWorld(this->DiskSource->GetOutput());
}

//----------------------------------------------------------------------
bool vtkSlicerDiskRepresentation3D::DescribePointsProximity(double * closestPoint, double * farthestPoint,
                                                           double& innerRadius, double& outerRadius)
{
  vtkMRMLMarkupsNode* markupsNode = this->GetMarkupsNode();
  if (!markupsNode || !this->IsDisplayable())
  {
    return false;
  }
  if (markupsNode->GetNumberOfDefinedControlPoints(true) != 3)
  {
    return false;
  }
  double p1[3] = { 0.0 }; // center
  double p2[3] = { 0.0 };
  double p3[3] = { 0.0 };
  markupsNode->GetNthControlPointPositionWorld(0, p1);
  markupsNode->GetNthControlPointPositionWorld(1, p2);
  markupsNode->GetNthControlPointPositionWorld(2, p3);
  
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
