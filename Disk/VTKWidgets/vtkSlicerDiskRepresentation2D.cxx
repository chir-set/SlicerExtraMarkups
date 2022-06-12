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

#include "vtkSlicerDiskRepresentation2D.h"

#include <vtkActor2D.h>
#include <vtkGlyphSource2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkSampleImplicitFunctionFilter.h>
#include <vtkPlane.h>

#include "vtkMRMLMarkupsDiskNode.h"

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerDiskRepresentation2D);

//------------------------------------------------------------------------------
vtkSlicerDiskRepresentation2D::vtkSlicerDiskRepresentation2D()
{
  this->SliceDistance = vtkSmartPointer<vtkSampleImplicitFunctionFilter>::New();
  this->SliceDistance->SetImplicitFunction(this->SlicePlane);
  
  this->WorldToSliceTransformer = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->WorldToSliceTransformer->SetTransform(this->WorldToSliceTransform);
  this->WorldToSliceTransformer->SetInputConnection(this->SliceDistance->GetOutputPort());
  
  this->DiskSource = vtkSmartPointer<vtkDiskSource>::New();
  this->DiskSource->SetInputConnection(this->WorldToSliceTransformer->GetOutputPort());
  
  this->DiskMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->DiskMapper->SetInputConnection(this->DiskSource->GetOutputPort());
  //this->DiskMapper->SetLookupTable(this->LineColorMap);
  this->DiskMapper->SetScalarVisibility(true);
  
  this->DiskProperty = vtkSmartPointer<vtkProperty2D>::New();
  this->DiskProperty->DeepCopy(this->GetControlPointsPipeline(Unselected)->Property);
  
  this->DiskActor = vtkSmartPointer<vtkActor2D>::New();
  this->DiskActor->SetMapper(this->DiskMapper);
  this->DiskActor->SetProperty(this->DiskProperty);
  
  this->SliceViewPlane = vtkSmartPointer<vtkPlane>::New();
  this->SliceViewPlane->SetNormal(0.0, 0.0, 1.0);
  double * slicePlaneOrigin = this->SlicePlane->GetOrigin();
  this->SliceViewPlane->SetOrigin(slicePlaneOrigin[0], slicePlaneOrigin[1], slicePlaneOrigin[2]);
  
  this->SliceViewCutter = vtkSmartPointer<vtkCutter>::New();
  this->SliceViewCutter->SetInputConnection(this->DiskSource->GetOutputPort());
  this->SliceViewCutter->SetCutFunction(this->SliceViewPlane);
  
  this->SliceViewCutMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->SliceViewCutMapper->SetInputConnection(this->SliceViewCutter->GetOutputPort());
  
  this->SliceViewCutActor = vtkSmartPointer<vtkActor2D>::New();
  this->SliceViewCutActor->SetMapper(this->SliceViewCutMapper);
  
  //this->TextActor->SetTextProperty(this->GetControlPointsPipeline(Unselected)->TextProperty);
}

//------------------------------------------------------------------------------
vtkSlicerDiskRepresentation2D::~vtkSlicerDiskRepresentation2D() = default;

//------------------------------------------------------------------------------
void vtkSlicerDiskRepresentation2D::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}
// -----------------------------------------------------------------------------
void vtkSlicerDiskRepresentation2D::UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData /*=nullptr*/)
{
  Superclass::UpdateFromMRML(caller, event, callData);

  this->NeedToRenderOn();

  vtkMRMLMarkupsNode* markupsNode = this->GetMarkupsNode();
  if (!markupsNode || !this->IsDisplayable())
    {
    this->VisibilityOff();
    return;
    }  
  vtkMRMLMarkupsDiskNode * diskNode = vtkMRMLMarkupsDiskNode::SafeDownCast(markupsNode);

  this->VisibilityOn();
  bool visibility = markupsNode->GetNumberOfDefinedControlPoints(true) == 3;
  this->DiskActor->SetVisibility(visibility);
  this->TextActor->SetVisibility(visibility);
  this->SliceViewCutActor->SetVisibility(visibility);
  
  double closestPoint[3] = { 0.0 }; // Unused here
  double farthestPoint[3] = { 0.0 };
  double innerDisplayRadius = 0.0, outerDisplayRadius = 0.0;
  if (!this->IdentifyPointProximity(closestPoint, farthestPoint, innerDisplayRadius, outerDisplayRadius))
    {
      return;
    }

  if (markupsNode->GetNumberOfDefinedControlPoints(true) == 3)
  {
    // Display coordinates.
    double p1[3] = { 0.0 };
    double p2[3] = { 0.0 };
    double p3[3] = { 0.0 };
    this->GetNthControlPointDisplayPosition(0, p1);
    this->GetNthControlPointDisplayPosition(1, p2);
    this->GetNthControlPointDisplayPosition(2, p3);
    
    // World coordinates.
    double p1World[3] = { 0.0 };
    double p2World[3] = { 0.0 };
    double p3World[3] = { 0.0 };
    diskNode->GetNthControlPointPositionWorld(0, p1World);
    diskNode->GetNthControlPointPositionWorld(1, p2World);
    diskNode->GetNthControlPointPositionWorld(2, p3World);
    
    // Display coordinates with Z.
    double p1WorldToDisplay[3] = { 0.0 };
    double p2WorldToDisplay[3] = { 0.0 };
    double p3WorldToDisplay[3] = { 0.0 };
    this->GetWorldToDisplayCoordinates(p1World, p1WorldToDisplay);
    this->GetWorldToDisplayCoordinates(p2World, p2WorldToDisplay);
    this->GetWorldToDisplayCoordinates(p3World, p3WorldToDisplay);
    
    // Account for slice view zooming.
    double p1WorldToDisplayScaled[3] = { p1WorldToDisplay[0],
                                        p1WorldToDisplay[1],
                                        p1WorldToDisplay[2] / this->ViewScaleFactorMmPerPixel };
    double p2WorldToDisplayScaled[3] = { p2WorldToDisplay[0],
                                        p2WorldToDisplay[1],
                                        p2WorldToDisplay[2] / this->ViewScaleFactorMmPerPixel };
    double p3WorldToDisplayScaled[3] = { p3WorldToDisplay[0],
                                        p3WorldToDisplay[1],
                                        p3WorldToDisplay[2] / this->ViewScaleFactorMmPerPixel };
          
    // Calculate normal relative to p1WorldToDisplayScaled
    double normalWorldToDisplayScaled[3] = { 0.0 };
    double rp2WorldToDisplayScaled[3] = { 0.0 };
    double rp3WorldToDisplayScaled[3] = { 0.0 };
    vtkMath::Subtract(p2WorldToDisplayScaled, p1WorldToDisplayScaled, rp2WorldToDisplayScaled);
    vtkMath::Subtract(p3WorldToDisplayScaled, p1WorldToDisplayScaled, rp3WorldToDisplayScaled);
    vtkMath::Cross(rp2WorldToDisplayScaled, rp3WorldToDisplayScaled, normalWorldToDisplayScaled);
    
    // Draw mode : WorldProjection or WorldIntersection.
    if (diskNode->GetDrawMode2D() != vtkMRMLMarkupsDiskNode::SliceProjection)
    {
      // Calculate inner and outer radii. Account for point proximities to center.
      double innerRadiusScaled = 0.0, outerRadiusScaled = 0.0;
      if (closestPoint[0] == p2[0] && closestPoint[1] == p2[1] && closestPoint[2] == p2[2])
      {
        innerRadiusScaled = std::sqrt(vtkMath::Distance2BetweenPoints(p1WorldToDisplayScaled, p2WorldToDisplayScaled));
        outerRadiusScaled = std::sqrt(vtkMath::Distance2BetweenPoints(p1WorldToDisplayScaled, p3WorldToDisplayScaled));
      }
      else
      {
        innerRadiusScaled = std::sqrt(vtkMath::Distance2BetweenPoints(p1WorldToDisplayScaled, p3WorldToDisplayScaled));
        outerRadiusScaled = std::sqrt(vtkMath::Distance2BetweenPoints(p1WorldToDisplayScaled, p2WorldToDisplayScaled));
      }
      
      this->DiskSource->SetCenter(p1WorldToDisplayScaled);
      this->DiskSource->SetNormal(normalWorldToDisplayScaled);
      this->DiskSource->SetOuterRadius(outerRadiusScaled);
      this->DiskSource->SetInnerRadius(innerRadiusScaled );
      // Show projection on demand.
      this->DiskActor->SetVisibility(diskNode->GetDrawMode2D() == vtkMRMLMarkupsDiskNode::WorldProjection);
    }
    else
    // Draw mode : SliceProjection.
    {
      const double normal[3] = { 0.0, 0.0, 1.0 };

      this->DiskSource->SetCenter(p1);
      this->DiskSource->SetNormal(normal);
      this->DiskSource->SetOuterRadius(innerDisplayRadius);
      this->DiskSource->SetInnerRadius(outerDisplayRadius);
    }
  }
  else
  {
    this->DiskActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
  }

  this->DiskSource->SetCircumferentialResolution((int) diskNode->GetResolution());
  this->DiskSource->Update();
  this->SliceViewCutter->Update();
  
  // Hide the disk actor if it doesn't intersect the current slice
  this->SliceDistance->Update();
  if (!Superclass::IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->DiskActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
  }
    
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->DiskActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->SliceViewCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  this->TextActor->SetDisplayPosition(farthestPoint[0], farthestPoint[1]);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->DiskProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->DiskProperty->SetOpacity(fillOpacity);
  this->DiskActor->SetProperty(this->DiskProperty);
  this->SliceViewCutActor->SetProperty(this->DiskProperty);
}

void vtkSlicerDiskRepresentation2D::SetMarkupsNode(vtkMRMLMarkupsNode* markupsNode)
{
  if (this->MarkupsNode != markupsNode)
  {
    if (markupsNode)
    {
      this->SliceDistance->SetInputConnection(markupsNode->GetCurveWorldConnection());
    }
    else
    {
      this->SliceDistance->SetInputData(this->DiskSource->GetOutput());
    }
  }
  this->Superclass::SetMarkupsNode(markupsNode);
}

//----------------------------------------------------------------------
void vtkSlicerDiskRepresentation2D::GetActors(vtkPropCollection *pc)
{
  this->DiskActor->GetActors(pc);
  this->TextActor->GetActors(pc);
  this->SliceViewCutActor->GetActors(pc);
  this->Superclass::GetActors(pc);
}

//----------------------------------------------------------------------
void vtkSlicerDiskRepresentation2D::ReleaseGraphicsResources(vtkWindow *win)
{
  this->DiskActor->ReleaseGraphicsResources(win);
  this->TextActor->ReleaseGraphicsResources(win);
  this->SliceViewCutActor->ReleaseGraphicsResources(win);
  this->Superclass::ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------
int vtkSlicerDiskRepresentation2D::RenderOverlay(vtkViewport *viewport)
{
  int count=0;
  if (this->DiskActor->GetVisibility())
  {
    count +=  this->DiskActor->RenderOverlay(viewport);
  }
  if (this->TextActor->GetVisibility())
  {
    count +=  this->TextActor->RenderOverlay(viewport);
  }
  if (this->SliceViewCutActor->GetVisibility())
  {
    count +=  this->SliceViewCutActor->RenderOverlay(viewport);
  }
  count += this->Superclass::RenderOverlay(viewport);
  return count;
}

//-----------------------------------------------------------------------------
int vtkSlicerDiskRepresentation2D::RenderOpaqueGeometry(vtkViewport *viewport)
{
  int count=0;
  if (this->DiskActor->GetVisibility())
  {
    count += this->DiskActor->RenderOpaqueGeometry(viewport);
  }
  if (this->TextActor->GetVisibility())
  {
    count += this->TextActor->RenderOpaqueGeometry(viewport);
  }
  if (this->SliceViewCutActor->GetVisibility())
  {
    count += this->SliceViewCutActor->RenderOpaqueGeometry(viewport);
  }
  count = this->Superclass::RenderOpaqueGeometry(viewport);
  return count;
}

//-----------------------------------------------------------------------------
int vtkSlicerDiskRepresentation2D::RenderTranslucentPolygonalGeometry(vtkViewport *viewport)
{
  int count=0;
  if (this->DiskActor->GetVisibility())
  {
    count += this->DiskActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->TextActor->GetVisibility())
  {
    count += this->TextActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->SliceViewCutActor->GetVisibility())
  {
    count += this->SliceViewCutActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  count = this->Superclass::RenderTranslucentPolygonalGeometry(viewport);
  return count;
}

//-----------------------------------------------------------------------------
vtkTypeBool vtkSlicerDiskRepresentation2D::HasTranslucentPolygonalGeometry()
{
  if (this->Superclass::HasTranslucentPolygonalGeometry())
    {
    return true;
    }
  if (this->DiskActor->GetVisibility() && this->DiskActor->HasTranslucentPolygonalGeometry())
    {
      return true;
    }
  if (this->TextActor->GetVisibility() && this->TextActor->HasTranslucentPolygonalGeometry())
    {
      return true;
    }
  if (this->SliceViewCutActor->GetVisibility() && this->SliceViewCutActor->HasTranslucentPolygonalGeometry())
    {
      return true;
    }
  return false;
}

//-----------------------------------------------------------------------------
bool vtkSlicerDiskRepresentation2D::IdentifyPointProximity(double * closestPoint, double * farthestPoint,
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
  this->GetNthControlPointDisplayPosition(0, p1);
  this->GetNthControlPointDisplayPosition(1, p2);
  this->GetNthControlPointDisplayPosition(2, p3);
  
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
