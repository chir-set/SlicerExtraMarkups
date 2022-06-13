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

#include "vtkSlicerSphereRepresentation2D.h"

#include <vtkActor2D.h>
#include <vtkGlyphSource2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPlane.h>
#include <vtkMRMLMarkupsSphereNode.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerSphereRepresentation2D);

//------------------------------------------------------------------------------
vtkSlicerSphereRepresentation2D::vtkSlicerSphereRepresentation2D()
{
  this->SliceDistance = vtkSmartPointer<vtkSampleImplicitFunctionFilter>::New();
  this->SliceDistance->SetImplicitFunction(this->SlicePlane);
  
  this->WorldToSliceTransformer = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->WorldToSliceTransformer->SetTransform(this->WorldToSliceTransform);
  this->WorldToSliceTransformer->SetInputConnection(this->SliceDistance->GetOutputPort());
  
  this->MiddlePointSource = vtkSmartPointer<vtkGlyphSource2D>::New();
  this->MiddlePointSource->SetCenter(0.0, 0.0, 0.0);
  this->MiddlePointSource->SetScale(5);

  this->MiddlePointDataMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->MiddlePointDataMapper->SetInputConnection(this->MiddlePointSource->GetOutputPort());

  this->MiddlePointActor = vtkSmartPointer<vtkActor2D>::New();
  this->MiddlePointActor->SetMapper(this->MiddlePointDataMapper);
  
  this->SphereSource = vtkSmartPointer<vtkSphereSource>::New();
  this->SphereSource->SetInputConnection(this->WorldToSliceTransformer->GetOutputPort());
  
  this->SphereMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->SphereMapper->SetInputConnection(this->SphereSource->GetOutputPort());
  this->SphereMapper->SetScalarVisibility(true);
  
  this->SphereActor = vtkSmartPointer<vtkActor2D>::New();
  this->SphereActor->SetMapper(this->SphereMapper);
  this->SphereActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
  
  this->SliceViewPlane = vtkSmartPointer<vtkPlane>::New();
  this->SliceViewPlane->SetNormal(0.0, 0.0, 1.0);
  double * slicePlaneOrigin = this->SlicePlane->GetOrigin();
  this->SliceViewPlane->SetOrigin(slicePlaneOrigin[0], slicePlaneOrigin[1], slicePlaneOrigin[2]);
  
  this->SliceViewCutter = vtkSmartPointer<vtkCutter>::New();
  this->SliceViewCutter->SetInputConnection(this->SphereSource->GetOutputPort());
  this->SliceViewCutter->SetCutFunction(this->SliceViewPlane);
  
  this->SliceViewCutMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->SliceViewCutMapper->SetInputConnection(this->SliceViewCutter->GetOutputPort());
  
  this->SliceViewCutActor = vtkSmartPointer<vtkActor2D>::New();
  this->SliceViewCutActor->SetMapper(this->SliceViewCutMapper);
  
  this->RadiusSource = vtkSmartPointer<vtkLineSource>::New();
  this->RadiusSource->SetInputConnection(this->WorldToSliceTransformer->GetOutputPort());
  
  this->RadiusDataMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->RadiusDataMapper->SetInputConnection(this->RadiusSource->GetOutputPort());
  this->RadiusDataMapper->SetScalarVisibility(true);
  
  this->RadiusActor = vtkSmartPointer<vtkActor2D>::New();
  this->RadiusActor->SetMapper(this->RadiusDataMapper);
  
  this->SphereProperty = vtkSmartPointer<vtkProperty2D>::New();
  this->SphereProperty->DeepCopy(this->GetControlPointsPipeline(Selected)->Property);
}

//------------------------------------------------------------------------------
vtkSlicerSphereRepresentation2D::~vtkSlicerSphereRepresentation2D() = default;

//------------------------------------------------------------------------------
void vtkSlicerSphereRepresentation2D::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}
// -----------------------------------------------------------------------------
void vtkSlicerSphereRepresentation2D::UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData /*=nullptr*/)
{
  Superclass::UpdateFromMRML(caller, event, callData);

  this->NeedToRenderOn();

  vtkMRMLMarkupsNode* markupsNode = this->GetMarkupsNode();
  if (!markupsNode || !this->IsDisplayable())
  {
    this->VisibilityOff();
    return;
  }

  this->VisibilityOn();
  
  vtkMRMLMarkupsSphereNode * sphereNode = vtkMRMLMarkupsSphereNode::SafeDownCast(markupsNode);

  bool visibility = (markupsNode->GetNumberOfDefinedControlPoints(true) == 2);
  this->SphereActor->SetVisibility(visibility);
  this->MiddlePointActor->SetVisibility(visibility);
  this->SliceViewCutActor->SetVisibility(visibility);
  this->RadiusActor->SetVisibility(visibility);
  this->TextActor->SetVisibility(visibility);

  if (markupsNode->GetNumberOfDefinedControlPoints(true) == 2)
  {
    // Display coordinates.
    double p1[3] = { 0.0 };
    double p2[3] = { 0.0 };
    this->GetNthControlPointDisplayPosition(0, p1);
    this->GetNthControlPointDisplayPosition(1, p2);
    
    // World coordinates.
    double p1World[3] = { 0.0 };
    double p2World[3] = { 0.0 };
    sphereNode->GetNthControlPointPositionWorld(0, p1World);
    sphereNode->GetNthControlPointPositionWorld(1, p2World);
    
    // Display coordinates with Z.
    double p1WorldToDisplay[3] = { 0.0 };
    double p2WorldToDisplay[3] = { 0.0 };
    this->GetWorldToDisplayCoordinates(p1World, p1WorldToDisplay);
    this->GetWorldToDisplayCoordinates(p2World, p2WorldToDisplay);
    
    // Account for slice view zooming. Why doesn't GetWorldToDisplayCoordinates do that ?
    double p1WorldToDisplayScaled[3] = { p1WorldToDisplay[0],
                                        p1WorldToDisplay[1],
                                        p1WorldToDisplay[2] / this->ViewScaleFactorMmPerPixel };
    double p2WorldToDisplayScaled[3] = { p2WorldToDisplay[0],
                                        p2WorldToDisplay[1],
                                        p2WorldToDisplay[2] / this->ViewScaleFactorMmPerPixel };
    
    // Scaled distance. This is not world radius.
    double lineLengthScaled = std::sqrt(vtkMath::Distance2BetweenPoints(p1WorldToDisplayScaled, p2WorldToDisplayScaled));
    // Draw mode : WorldProjection or WorldIntersection
    if (sphereNode->GetDrawMode2D() != vtkMRMLMarkupsSphereNode::SliceProjection)
    {
      // Centered mode.
      if (sphereNode->GetRadiusMode() == vtkMRMLMarkupsSphereNode::Centered)
      { 
        this->SphereSource->SetCenter(p1WorldToDisplayScaled);
        this->SphereSource->SetRadius(lineLengthScaled);
        
        this->MiddlePointSource->SetCenter(p1[0], p1[1], 0.0);
        this->MiddlePointSource->Update();
        // The middle point's properties are distinct.
        this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
        
        this->RadiusSource->SetPoint1(p1);
      }
      // Circumferential mode : center is half way between p1 and p2 WorldToDisplayScaled.
      else
      {
        double radiusScaled = lineLengthScaled / 2.0;
        double center[3] = { (p1WorldToDisplayScaled[0] + p2WorldToDisplayScaled[0]) / 2.0,
                            (p1WorldToDisplayScaled[1] + p2WorldToDisplayScaled[1]) / 2.0,
                            (p1WorldToDisplayScaled[2] + p2WorldToDisplayScaled[2]) / 2.0 };
          
          this->SphereSource->SetCenter(center);
          this->SphereSource->SetRadius(radiusScaled);
          
          double middlePointPos[2] = { (p1[0] + p2[0]) / 2.0, (p1[1] + p2[1]) / 2.0 };
          this->MiddlePointSource->SetCenter(middlePointPos[0], middlePointPos[1], 0.0);
          this->MiddlePointSource->Update();
          this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
          
          this->RadiusSource->SetPoint1(center);
      }
      this->SphereActor->SetVisibility(sphereNode->GetDrawMode2D() == vtkMRMLMarkupsSphereNode::WorldProjection);
      this->RadiusActor->SetVisibility(sphereNode->GetDrawMode2D() == vtkMRMLMarkupsSphereNode::WorldIntersection);
      this->SliceViewCutActor->SetVisibility(sphereNode->GetDrawMode2D() == vtkMRMLMarkupsSphereNode::WorldIntersection);
    }
    else
      // Draw mode : SliceProjection.
    {
      double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));    
      const double normal[3] = { 0.0, 0.0, 1.0 };
      
      // Centered mode : p1 is center, line length is radius.
      if (sphereNode->GetRadiusMode() == vtkMRMLMarkupsSphereNode::Centered)
      { 
        this->SphereSource->SetCenter(p1);
        this->SphereSource->SetRadius(lineLength);
        
        this->MiddlePointSource->SetCenter(p1[0], p1[1], 0.0);
        this->MiddlePointSource->Update();
        
        this->RadiusSource->SetPoint1(p1);
      }
      // Circumferential mode : center is half way between p1 and p2, radius is half of line length.
      else
      {
        double radius = lineLength / 2.0;
        double center[3] = { (p1[0] + p2[0]) / 2.0, (p1[1] + p2[1]) / 2.0, (p1[2] + p2[2]) / 2.0 };
        
        this->SphereSource->SetCenter(center);
        this->SphereSource->SetRadius(radius);
        
        this->MiddlePointSource->SetCenter(center);
        this->MiddlePointSource->Update();
        
        this->RadiusSource->SetPoint1(center);
      }
      this->SphereActor->SetVisibility(true);
      this->RadiusActor->SetVisibility(true);
      this->SliceViewCutActor->SetVisibility(false);
    }
    
    this->SphereSource->SetPhiResolution(sphereNode->GetResolution());
    this->SphereSource->SetThetaResolution(sphereNode->GetResolution());
    this->SphereSource->Update();
    this->SliceViewCutter->Update();
    this->RadiusSource->SetPoint2(p2);
    this->RadiusSource->Update();
    this->TextActor->SetPosition(p2);
  }
  else
  {
    this->SphereActor->SetVisibility(false);
    this->MiddlePointActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
  }

  // Hide the line actor if it doesn't intersect the current slice
  this->SliceDistance->Update();
  if (!this->IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->SphereActor->SetVisibility(false);
    this->MiddlePointActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
  }
  
  this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->SphereActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->SliceViewCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->SphereProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->SphereProperty->SetOpacity(fillOpacity);
  this->SphereActor->SetProperty(this->SphereProperty);
}

//----------------------------------------------------------------------
void vtkSlicerSphereRepresentation2D::SetMarkupsNode(vtkMRMLMarkupsNode* markupsNode)
{
  if (this->MarkupsNode != markupsNode)
  {
    if (markupsNode)
    {
      this->SliceDistance->SetInputConnection(markupsNode->GetCurveWorldConnection());
    }
    else
    {
      this->SliceDistance->SetInputData(this->SliceViewCutter->GetOutput());;
    }
  }
  this->Superclass::SetMarkupsNode(markupsNode);
}

//----------------------------------------------------------------------
void vtkSlicerSphereRepresentation2D::GetActors(vtkPropCollection *pc)
{
  this->SphereActor->GetActors(pc);
  this->MiddlePointActor->GetActors(pc);
  this->RadiusActor->GetActors(pc);
  this->SliceViewCutActor->GetActors(pc);
  this->TextActor->GetActors(pc);
  this->Superclass::GetActors(pc);
}

//----------------------------------------------------------------------
void vtkSlicerSphereRepresentation2D::ReleaseGraphicsResources(vtkWindow *win)
{
  this->SphereActor->ReleaseGraphicsResources(win);
  this->MiddlePointActor->ReleaseGraphicsResources(win);
  this->RadiusActor->ReleaseGraphicsResources(win);
  this->SliceViewCutActor->ReleaseGraphicsResources(win);
  this->TextActor->ReleaseGraphicsResources(win);
  this->Superclass::ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------
int vtkSlicerSphereRepresentation2D::RenderOverlay(vtkViewport *viewport)
{
  int count=0;
  if (this->SphereActor->GetVisibility())
    {
      count +=  this->SphereActor->RenderOverlay(viewport);
    }
  if (this->MiddlePointActor->GetVisibility())
    {
    count +=  this->MiddlePointActor->RenderOverlay(viewport);
    }
  if (this->RadiusActor->GetVisibility())
    {
      count +=  this->RadiusActor->RenderOverlay(viewport);
    }
  if (this->SliceViewCutActor->GetVisibility())
    {
      count +=  this->SliceViewCutActor->RenderOverlay(viewport);
    }
  if (this->TextActor->GetVisibility())
    {
      count +=  this->TextActor->RenderOverlay(viewport);
    }
  count += this->Superclass::RenderOverlay(viewport);
  return count;
}

//-----------------------------------------------------------------------------
int vtkSlicerSphereRepresentation2D::RenderOpaqueGeometry(vtkViewport *viewport)
{
  int count=0;
  if (this->SphereActor->GetVisibility())
    {
      count += this->SphereActor->RenderOpaqueGeometry(viewport);
    }
  if (this->MiddlePointActor->GetVisibility())
    {
    count += this->MiddlePointActor->RenderOpaqueGeometry(viewport);
    }
  if (this->RadiusActor->GetVisibility())
    {
      count += this->RadiusActor->RenderOpaqueGeometry(viewport);
    }
  if (this->SliceViewCutActor->GetVisibility())
    {
      count += this->SliceViewCutActor->RenderOpaqueGeometry(viewport);
    }
  if (this->TextActor->GetVisibility())
    {
      count += this->TextActor->RenderOpaqueGeometry(viewport);
    }
  count = this->Superclass::RenderOpaqueGeometry(viewport);
  return count;
}

//-----------------------------------------------------------------------------
int vtkSlicerSphereRepresentation2D::RenderTranslucentPolygonalGeometry(vtkViewport *viewport)
{
  int count=0;
  if (this->SphereActor->GetVisibility())
    {
      count += this->SphereActor->RenderTranslucentPolygonalGeometry(viewport);
    }
  if (this->MiddlePointActor->GetVisibility())
    {
    count += this->MiddlePointActor->RenderTranslucentPolygonalGeometry(viewport);
    }
  if (this->RadiusActor->GetVisibility())
    {
      count += this->RadiusActor->RenderTranslucentPolygonalGeometry(viewport);
    }
  if (this->SliceViewCutActor->GetVisibility())
    {
      count += this->SliceViewCutActor->RenderTranslucentPolygonalGeometry(viewport);
    }
  if (this->TextActor->GetVisibility())
    {
      count += this->TextActor->RenderTranslucentPolygonalGeometry(viewport);
    }
  count = this->Superclass::RenderTranslucentPolygonalGeometry(viewport);
  return count;
}

//-----------------------------------------------------------------------------
vtkTypeBool vtkSlicerSphereRepresentation2D::HasTranslucentPolygonalGeometry()
{
  if (this->Superclass::HasTranslucentPolygonalGeometry())
    {
    return true;
    }
  if (this->SphereActor->GetVisibility() && this->SphereActor->HasTranslucentPolygonalGeometry())
    {
      return true;
    }
  if (this->MiddlePointActor->GetVisibility() && this->MiddlePointActor->HasTranslucentPolygonalGeometry())
    {
    return true;
    }
   if (this->RadiusActor->GetVisibility() && this->RadiusActor->HasTranslucentPolygonalGeometry())
    {
      return true;
    }
  if (this->SliceViewCutActor->GetVisibility() && this->SliceViewCutActor->HasTranslucentPolygonalGeometry())
    {
      return true;
    }
  if (this->TextActor->GetVisibility() && this->TextActor->HasTranslucentPolygonalGeometry())
    {
      return true;
    }
  return false;
}
