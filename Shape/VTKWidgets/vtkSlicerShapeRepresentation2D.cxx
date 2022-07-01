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

#include "vtkSlicerShapeRepresentation2D.h"
#include "vtkMRMLMarkupsShapeNode.h"

#include <vtkActor2D.h>
#include <vtkGlyphSource2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkSampleImplicitFunctionFilter.h>
#include <vtkPlane.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerShapeRepresentation2D);

//------------------------------------------------------------------------------
vtkSlicerShapeRepresentation2D::vtkSlicerShapeRepresentation2D()
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
  
  this->DiskSource = vtkSmartPointer<vtkDiskSource>::New();
  this->DiskSource->SetInputConnection(this->WorldToSliceTransformer->GetOutputPort());
  this->RingSource = vtkSmartPointer<vtkDiskSource>::New();
  this->RingSource->SetInputConnection(this->WorldToSliceTransformer->GetOutputPort());
  this->SphereSource = vtkSmartPointer<vtkSphereSource>::New();
  this->SphereSource->SetInputConnection(this->WorldToSliceTransformer->GetOutputPort());
  
  this->RadiusSource = vtkSmartPointer<vtkLineSource>::New();
  this->RadiusSource->SetInputConnection(this->WorldToSliceTransformer->GetOutputPort());
  this->RadiusMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->RadiusMapper->SetInputConnection(this->RadiusSource->GetOutputPort());
  this->RadiusMapper->SetScalarVisibility(true);
  this->RadiusActor = vtkSmartPointer<vtkActor2D>::New();
  this->RadiusActor->SetMapper(this->RadiusMapper);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
  
  this->ShapeMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  //this->DiskMapper->SetInputConnection(this->DiskSource->GetOutputPort());
  //this->DiskMapper->SetLookupTable(this->LineColorMap);
  this->ShapeMapper->SetScalarVisibility(true);
  this->ShapeProperty = vtkSmartPointer<vtkProperty2D>::New();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(Unselected)->Property);
  this->ShapeActor = vtkSmartPointer<vtkActor2D>::New();
  this->ShapeActor->SetMapper(this->ShapeMapper);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  
  this->SliceViewPlane = vtkSmartPointer<vtkPlane>::New();
  this->SliceViewPlane->SetNormal(0.0, 0.0, 1.0);
  double * slicePlaneOrigin = this->SlicePlane->GetOrigin();
  this->SliceViewPlane->SetOrigin(slicePlaneOrigin[0], slicePlaneOrigin[1], slicePlaneOrigin[2]);
  this->SliceViewCutter = vtkSmartPointer<vtkCutter>::New();
  //this->SliceViewCutter->SetInputConnection(this->DiskSource->GetOutputPort());
  this->SliceViewCutter->SetCutFunction(this->SliceViewPlane);
  this->SliceViewCutMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->SliceViewCutMapper->SetInputConnection(this->SliceViewCutter->GetOutputPort());
  this->SliceViewCutActor = vtkSmartPointer<vtkActor2D>::New();
  this->SliceViewCutActor->SetMapper(this->SliceViewCutMapper);
}

//------------------------------------------------------------------------------
vtkSlicerShapeRepresentation2D::~vtkSlicerShapeRepresentation2D() = default;

//------------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}
// -----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData /*=nullptr*/)
{
  Superclass::UpdateFromMRML(caller, event, callData);

  this->NeedToRenderOn();

  vtkMRMLMarkupsShapeNode* shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  if (!shapeNode || !this->IsDisplayable())
  {
    this->VisibilityOff();
    return;
  }

  this->VisibilityOn();

  this->MiddlePointActor->SetVisibility(shapeNode->GetNumberOfDefinedControlPoints(true) >= 2);
  // Hide the middle point actor if it doesn't intersect the current slice
  this->SliceDistance->Update();
  if (!this->IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->MiddlePointActor->SetVisibility(false);
  }
  if (shapeNode->GetNumberOfDefinedControlPoints(true) >= 2)
  {
    double p1[3] = { 0.0 };
    double p2[3] = { 0.0 };
    this->GetNthControlPointDisplayPosition(0, p1);
    this->GetNthControlPointDisplayPosition(1, p2);
    double middlePointPos[2] = { (p1[0] + p2[0]) / 2.0, (p1[1] + p2[1]) / 2.0 };
    this->MiddlePointSource->SetCenter(middlePointPos[0], middlePointPos[1], 0.0);
    this->MiddlePointSource->Update();
  }
  else
  {
    this->MiddlePointActor->SetVisibility(false);
  }
  this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
  
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

//-----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::SetMarkupsNode(vtkMRMLMarkupsNode *markupsNode)
{
  if (this->MarkupsNode != markupsNode)
  {
    if (markupsNode)
    {
      this->SliceDistance->SetInputConnection(markupsNode->GetCurveWorldConnection());
    }
    else
    {
      //this->SliceDistance->SetInputData(this->RingSource->GetOutput());
    }
  }
  this->Superclass::SetMarkupsNode(markupsNode);
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::UpdateInteractionPipeline()
{
  if (!this->MiddlePointActor->GetVisibility() || !this->ShapeActor->GetVisibility()
    || !this->TextActor->GetVisibility() || !this->SliceViewCutActor->GetVisibility()
    || !this->RadiusActor->GetVisibility()
  )
  {
    this->InteractionPipeline->Actor->SetVisibility(false);
    return;
  }
  // Final visibility handled by superclass in vtkSlicerMarkupsWidgetRepresentation
  Superclass::UpdateInteractionPipeline();
}

//----------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::GetActors(vtkPropCollection *pc)
{
  this->MiddlePointActor->GetActors(pc);
  this->ShapeActor->GetActors(pc);
  this->RadiusActor->GetActors(pc);
  this->TextActor->GetActors(pc);
  this->SliceViewCutActor->GetActors(pc);
  this->Superclass::GetActors(pc);
}

//----------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::ReleaseGraphicsResources(vtkWindow *win)
{
  this->MiddlePointActor->ReleaseGraphicsResources(win);
  this->ShapeActor->ReleaseGraphicsResources(win);
  this->RadiusActor->ReleaseGraphicsResources(win);
  this->TextActor->ReleaseGraphicsResources(win);
  this->SliceViewCutActor->ReleaseGraphicsResources(win);
  this->Superclass::ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------
int vtkSlicerShapeRepresentation2D::RenderOverlay(vtkViewport *viewport)
{
  int count=0;
  if (this->MiddlePointActor->GetVisibility())
  {
    count +=  this->MiddlePointActor->RenderOverlay(viewport);
  }
  if (this->ShapeActor->GetVisibility())
  {
    count +=  this->ShapeActor->RenderOverlay(viewport);
  }
  if (this->RadiusActor->GetVisibility())
  {
    count +=  this->RadiusActor->RenderOverlay(viewport);
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
int vtkSlicerShapeRepresentation2D::RenderOpaqueGeometry(vtkViewport *viewport)
{
  int count=0;
  if (this->MiddlePointActor->GetVisibility())
  {
    count += this->MiddlePointActor->RenderOpaqueGeometry(viewport);
  }
  if (this->ShapeActor->GetVisibility())
  {
    count +=  this->ShapeActor->RenderOverlay(viewport);
  }
  if (this->RadiusActor->GetVisibility())
  {
    count +=  this->RadiusActor->RenderOverlay(viewport);
  }
  if (this->TextActor->GetVisibility())
  {
    count +=  this->TextActor->RenderOverlay(viewport);
  }
  if (this->SliceViewCutActor->GetVisibility())
  {
    count +=  this->SliceViewCutActor->RenderOverlay(viewport);
  }
  count = this->Superclass::RenderOpaqueGeometry(viewport);
  return count;
}

//-----------------------------------------------------------------------------
int vtkSlicerShapeRepresentation2D::RenderTranslucentPolygonalGeometry(vtkViewport *viewport)
{
  int count=0;
  if (this->MiddlePointActor->GetVisibility())
  {
    count += this->MiddlePointActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->ShapeActor->GetVisibility())
  {
    count += this->ShapeActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->RadiusActor->GetVisibility())
  {
    count += this->RadiusActor->RenderTranslucentPolygonalGeometry(viewport);
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
vtkTypeBool vtkSlicerShapeRepresentation2D::HasTranslucentPolygonalGeometry()
{
  if (this->Superclass::HasTranslucentPolygonalGeometry())
  {
    return true;
  }
  if (this->MiddlePointActor->GetVisibility() && this->MiddlePointActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }
  if (this->ShapeActor->GetVisibility() && this->ShapeActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }
  if (this->RadiusActor->GetVisibility() && this->RadiusActor->HasTranslucentPolygonalGeometry())
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
void vtkSlicerShapeRepresentation2D::UpdateDiskFromMRML(vtkMRMLNode* caller, unsigned long event,
                                                        void* callData)
{
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  
  this->VisibilityOn();
  bool visibility = shapeNode->GetNumberOfDefinedControlPoints(true) == 3;
  this->ShapeActor->SetVisibility(visibility);
  this->TextActor->SetVisibility(visibility);
  this->SliceViewCutActor->SetVisibility(visibility);
  
  double closestPoint[3] = { 0.0 }; // Unused here
  double farthestPoint[3] = { 0.0 };
  double innerDisplayRadius = 0.0, outerDisplayRadius = 0.0;
  // shapeNode->DescribeDiskPointsProximity() uses world coordinates.
  if (!this->DescribeDisplayPointSpacing(closestPoint, farthestPoint,
                                            innerDisplayRadius, outerDisplayRadius))
  {
    return;
  }
  this->MiddlePointActor->SetVisibility(false);
  this->ShapeMapper->SetInputConnection(this->DiskSource->GetOutputPort());
  this->SliceViewCutter->SetInputConnection(this->DiskSource->GetOutputPort());
  
  if (shapeNode->GetNumberOfDefinedControlPoints(true) == 3)
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
    shapeNode->GetNthControlPointPositionWorld(0, p1World);
    shapeNode->GetNthControlPointPositionWorld(1, p2World);
    shapeNode->GetNthControlPointPositionWorld(2, p3World);
    
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
          
    // Calculate inner and outer radii. Account for point proximities to center.
    double innerRadiusScaled = 0.0, outerRadiusScaled = 0.0;
    if (closestPoint[0] == p2[0] && closestPoint[1] == p2[1] && closestPoint[2] == p2[2])
    {
      innerRadiusScaled = std::sqrt(vtkMath::Distance2BetweenPoints(p1WorldToDisplayScaled, 
                                                                    p2WorldToDisplayScaled));
      outerRadiusScaled = std::sqrt(vtkMath::Distance2BetweenPoints(p1WorldToDisplayScaled, 
                                                                    p3WorldToDisplayScaled));
    }
    else
    {
      innerRadiusScaled = std::sqrt(vtkMath::Distance2BetweenPoints(p1WorldToDisplayScaled, 
                                                                    p3WorldToDisplayScaled));
      outerRadiusScaled = std::sqrt(vtkMath::Distance2BetweenPoints(p1WorldToDisplayScaled, 
                                                                    p2WorldToDisplayScaled));
    }
            
    this->DiskSource->SetCenter(p1WorldToDisplayScaled);
    this->DiskSource->SetNormal(normalWorldToDisplayScaled);
    this->DiskSource->SetOuterRadius(outerRadiusScaled);
    this->DiskSource->SetInnerRadius(innerRadiusScaled );
    // Show projections on demand.
    this->SliceViewCutActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Intersection);
    this->ShapeActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
  }
  else
  {
    this->ShapeActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
  }
  
  this->DiskSource->SetCircumferentialResolution((int) shapeNode->GetResolution());
  this->DiskSource->Update();
  this->SliceViewCutter->Update();
  
  // Hide the disk actor if it doesn't intersect the current slice
  this->SliceDistance->Update();
  if (!Superclass::IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->ShapeActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
  }
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->SliceViewCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  this->TextActor->SetDisplayPosition(farthestPoint[0], farthestPoint[1]);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->ShapeProperty->SetOpacity(fillOpacity);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  this->SliceViewCutActor->SetProperty(this->ShapeProperty);
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::UpdateRingFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  
  this->VisibilityOn();
  bool visibility = shapeNode->GetNumberOfDefinedControlPoints(true) == 3;
  this->MiddlePointActor->SetVisibility(visibility);
  this->ShapeActor->SetVisibility(visibility);
  this->RadiusActor->SetVisibility(visibility);
  this->TextActor->SetVisibility(visibility);
  this->SliceViewCutActor->SetVisibility(visibility);
  
  this->ShapeMapper->SetInputConnection(this->RingSource->GetOutputPort());
  this->SliceViewCutter->SetInputConnection(this->RingSource->GetOutputPort());
  
  if (shapeNode->GetNumberOfDefinedControlPoints(true) == 3)
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
    shapeNode->GetNthControlPointPositionWorld(0, p1World);
    shapeNode->GetNthControlPointPositionWorld(1, p2World);
    shapeNode->GetNthControlPointPositionWorld(2, p3World);
    
    // Display coordinates with Z.
    double p1WorldToDisplay[3] = { 0.0 };
    double p2WorldToDisplay[3] = { 0.0 };
    double p3WorldToDisplay[3] = { 0.0 };
    this->GetWorldToDisplayCoordinates(p1World, p1WorldToDisplay);
    this->GetWorldToDisplayCoordinates(p2World, p2WorldToDisplay);
    this->GetWorldToDisplayCoordinates(p3World, p3WorldToDisplay);
    
    // Account for slice view zooming. Why doesn't GetWorldToDisplayCoordinates do that ?
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
    
    // Scaled distance. This is not world radius.
    double lineLengthScaled = std::sqrt(vtkMath::Distance2BetweenPoints(p1WorldToDisplayScaled, p2WorldToDisplayScaled));    
          
    // Centered mode.
    if (shapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
    { 
      this->RingSource->SetCenter(p1WorldToDisplayScaled);
      this->RingSource->SetNormal(normalWorldToDisplayScaled);
      this->RingSource->SetOuterRadius(lineLengthScaled);
      this->RingSource->SetInnerRadius((lineLengthScaled - 1.0) );
      
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
        
      this->RingSource->SetCenter(center);
      this->RingSource->SetNormal(normalWorldToDisplayScaled);
      this->RingSource->SetOuterRadius(radiusScaled);
      this->RingSource->SetInnerRadius(radiusScaled - 1.0);
      
      double middlePointPos[2] = { (p1[0] + p2[0]) / 2.0, (p1[1] + p2[1]) / 2.0 };
      this->MiddlePointSource->SetCenter(middlePointPos[0], middlePointPos[1], 0.0);
      this->MiddlePointSource->Update();
      this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
      
      this->RadiusSource->SetPoint1(center);
    }
    // Showing the radius here is confusing in UI.
    this->RadiusActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
    // Show the projection. SliceViewCutActor is also visible, but will blend with the projection. 
    this->ShapeActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
    
    this->RingSource->SetCircumferentialResolution((int) shapeNode->GetResolution());
    this->RingSource->Update();
    this->SliceViewCutter->Update();
    this->RadiusSource->SetPoint2(p2);
    this->RadiusSource->Update();
    this->TextActor->SetPosition(p3[0], p3[1]);
  }
  else
  {
    this->MiddlePointActor->SetVisibility(false);
    this->ShapeActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
  }
  
  // Hide actors if they don't intersect the current slice
  this->SliceDistance->Update();
  if (!Superclass::IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->MiddlePointActor->SetVisibility(false);
    this->ShapeActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
  }
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  this->SliceViewCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::UpdateSphereFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  
  bool visibility = (shapeNode->GetNumberOfDefinedControlPoints(true) == 2);
  this->ShapeActor->SetVisibility(visibility);
  this->MiddlePointActor->SetVisibility(visibility);
  this->SliceViewCutActor->SetVisibility(visibility);
  this->RadiusActor->SetVisibility(visibility);
  this->TextActor->SetVisibility(visibility);
  
  this->ShapeMapper->SetInputConnection(this->SphereSource->GetOutputPort());
  this->SliceViewCutter->SetInputConnection(this->SphereSource->GetOutputPort());
  
  if (shapeNode->GetNumberOfDefinedControlPoints(true) == 2)
  {
    // Display coordinates.
    double p1[3] = { 0.0 };
    double p2[3] = { 0.0 };
    this->GetNthControlPointDisplayPosition(0, p1);
    this->GetNthControlPointDisplayPosition(1, p2);
    
    // World coordinates.
    double p1World[3] = { 0.0 };
    double p2World[3] = { 0.0 };
    shapeNode->GetNthControlPointPositionWorld(0, p1World);
    shapeNode->GetNthControlPointPositionWorld(1, p2World);
    
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
    
    // Centered mode.
    if (shapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
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
    this->ShapeActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
    this->RadiusActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Intersection);
    this->SliceViewCutActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Intersection);
    
    this->SphereSource->SetPhiResolution(shapeNode->GetResolution());
    this->SphereSource->SetThetaResolution(shapeNode->GetResolution());
    this->SphereSource->Update();
    this->SliceViewCutter->Update();
    this->RadiusSource->SetPoint2(p2);
    this->RadiusSource->Update();
    this->TextActor->SetPosition(p2);
  }
  else
  {
    this->ShapeActor->SetVisibility(false);
    this->MiddlePointActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
  }
  
  // Hide actors if they don't intersect the current slice
  this->SliceDistance->Update();
  if (!this->IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->ShapeActor->SetVisibility(false);
    this->MiddlePointActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
  }
  
  this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->SliceViewCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->ShapeProperty->SetOpacity(fillOpacity);
  this->ShapeActor->SetProperty(this->ShapeProperty);
}

//-----------------------------------------------------------------------------
bool vtkSlicerShapeRepresentation2D::DescribeDisplayPointSpacing(double* closestPoint, double* farthestPoint, 
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
