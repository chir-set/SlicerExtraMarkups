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

#include "vtkSlicerRingRepresentation2D.h"

#include <vtkActor2D.h>
#include <vtkGlyphSource2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkSampleImplicitFunctionFilter.h>
#include <vtkMRMLMarkupsRingNode.h>
#include <vtkPlane.h>
//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerRingRepresentation2D);

//------------------------------------------------------------------------------
vtkSlicerRingRepresentation2D::vtkSlicerRingRepresentation2D()
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
  
  this->RingSource = vtkSmartPointer<vtkDiskSource>::New();
  this->RingSource->SetInputConnection(this->WorldToSliceTransformer->GetOutputPort());
  
  this->RingMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->RingMapper->SetInputConnection(this->RingSource->GetOutputPort());
  //this->RingMapper->SetLookupTable(this->LineColorMap);
  this->RingMapper->SetScalarVisibility(true);
  
  this->RingActor = vtkSmartPointer<vtkActor2D>::New();
  this->RingActor->SetMapper(this->RingMapper);
  this->RingActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
  
  this->RadiusSource = vtkSmartPointer<vtkLineSource>::New();
  this->RadiusSource->SetInputConnection(this->WorldToSliceTransformer->GetOutputPort());
  
  this->RadiusMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->RadiusMapper->SetInputConnection(this->RadiusSource->GetOutputPort());
  this->RadiusMapper->SetScalarVisibility(true);
  
  this->RadiusActor = vtkSmartPointer<vtkActor2D>::New();
  this->RadiusActor->SetMapper(this->RadiusMapper);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
  
  this->SliceViewPlane = vtkSmartPointer<vtkPlane>::New();
  this->SliceViewPlane->SetNormal(0.0, 0.0, 1.0);
  double * slicePlaneOrigin = this->SlicePlane->GetOrigin();
  this->SliceViewPlane->SetOrigin(slicePlaneOrigin[0], slicePlaneOrigin[1], slicePlaneOrigin[2]);
  
  this->SliceViewCutter = vtkSmartPointer<vtkCutter>::New();
  this->SliceViewCutter->SetInputConnection(this->RingSource->GetOutputPort());
  this->SliceViewCutter->SetCutFunction(this->SliceViewPlane);
  
  this->SliceViewCutMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->SliceViewCutMapper->SetInputConnection(this->SliceViewCutter->GetOutputPort());
  
  this->SliceViewCutActor = vtkSmartPointer<vtkActor2D>::New();
  this->SliceViewCutActor->SetMapper(this->SliceViewCutMapper);
}

//------------------------------------------------------------------------------
vtkSlicerRingRepresentation2D::~vtkSlicerRingRepresentation2D() = default;

//------------------------------------------------------------------------------
void vtkSlicerRingRepresentation2D::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}
// -----------------------------------------------------------------------------
void vtkSlicerRingRepresentation2D::UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData /*=nullptr*/)
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
  vtkMRMLMarkupsRingNode * ringNode = vtkMRMLMarkupsRingNode::SafeDownCast(markupsNode);

  bool visibility = markupsNode->GetNumberOfDefinedControlPoints(true) == 3;
  this->MiddlePointActor->SetVisibility(visibility);
  this->RingActor->SetVisibility(visibility);
  this->RadiusActor->SetVisibility(visibility);
  this->TextActor->SetVisibility(visibility);
  this->SliceViewCutActor->SetVisibility(visibility);
  
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
    ringNode->GetNthControlPointPositionWorld(0, p1World);
    ringNode->GetNthControlPointPositionWorld(1, p2World);
    ringNode->GetNthControlPointPositionWorld(2, p3World);
    
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
    
    // Draw mode : WorldProjection or WorldIntersection
    if (ringNode->GetDrawMode2D() != vtkMRMLMarkupsRingNode::SliceProjection)
    {
      // Centered mode.
      if (ringNode->GetRadiusMode() == vtkMRMLMarkupsRingNode::Centered)
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
      this->RadiusActor->SetVisibility(ringNode->GetDrawMode2D() == vtkMRMLMarkupsRingNode::WorldProjection);
      // Show the projection. SliceViewCutActor is also visible, but will blend with the projection. 
      this->RingActor->SetVisibility(ringNode->GetDrawMode2D() == vtkMRMLMarkupsRingNode::WorldProjection);
    }
    else
      // Draw mode : SliceProjection.
    {
      double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));    
      const double normal[3] = { 0.0, 0.0, 1.0 };
      
      // Centered mode : p1 is center, line length is radius.
      if (ringNode->GetRadiusMode() == vtkMRMLMarkupsRingNode::Centered)
      { 
        this->RingSource->SetCenter(p1);
        this->RingSource->SetNormal(normal);
        this->RingSource->SetOuterRadius(lineLength);
        this->RingSource->SetInnerRadius(lineLength - 1.0);
        
        this->MiddlePointActor->SetVisibility(false);
        this->RadiusSource->SetPoint1(p1);
      }
      // Circumferential mode : center is half way between p1 and p2, radius is half of line length.
      else
      {
        double radius = lineLength / 2.0;
        double center[3] = { (p1[0] + p2[0]) / 2.0, (p1[1] + p2[1]) / 2.0, (p1[2] + p2[2]) / 2.0 };
        
        this->RingSource->SetCenter(center);
        this->RingSource->SetNormal(normal);
        this->RingSource->SetOuterRadius(radius);
        this->RingSource->SetInnerRadius(radius - 1.0);
        
        this->MiddlePointActor->SetVisibility(true);
        this->RadiusSource->SetPoint1(center);
      }
      this->RadiusActor->SetVisibility(true);
      this->RingActor->SetVisibility(true);
    }
    
    this->RingSource->SetCircumferentialResolution((int) ringNode->GetResolution());
    this->RingSource->Update();
    this->SliceViewCutter->Update();
    
    this->RadiusSource->SetPoint2(p2);
    this->RadiusSource->Update();
    
    this->TextActor->SetPosition(p3[0], p3[1]);
  }
  else
  {
    this->MiddlePointActor->SetVisibility(false);
    this->RingActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
  }
  
  // Hide the line actor if it doesn't intersect the current slice
  this->SliceDistance->Update();
  if (!Superclass::IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->MiddlePointActor->SetVisibility(false);
    this->RingActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
  }
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->RingActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  this->SliceViewCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  
  /*if (this->MarkupsDisplayNode->GetLineColorNode() && this->MarkupsDisplayNode->GetLineColorNode()->GetColorTransferFunction())
  {
    this->RingMapper->SetLookupTable(this->MarkupsDisplayNode->GetLineColorNode()->GetColorTransferFunction());
  }
  else
  {
    this->RingMapper->SetLookupTable(this->LineColorMap);
  }*/
}

//-----------------------------------------------------------------------------
void vtkSlicerRingRepresentation2D::SetMarkupsNode(vtkMRMLMarkupsNode *markupsNode)
{
  if (this->MarkupsNode != markupsNode)
  {
    if (markupsNode)
    {
      this->SliceDistance->SetInputConnection(markupsNode->GetCurveWorldConnection());
    }
    else
    {
      this->SliceDistance->SetInputData(this->RingSource->GetOutput());
    }
  }
  this->Superclass::SetMarkupsNode(markupsNode);
}

//-----------------------------------------------------------------------------
void vtkSlicerRingRepresentation2D::UpdateInteractionPipeline()
{
  if (!this->MiddlePointActor->GetVisibility() || !this->RingActor->GetVisibility()
    || !this->RadiusActor->GetVisibility() || !this->TextActor->GetVisibility()
    || !this->SliceViewCutActor->GetVisibility()
  )
  {
    this->InteractionPipeline->Actor->SetVisibility(false);
    return;
  }
  // Final visibility handled by superclass in vtkSlicerMarkupsWidgetRepresentation
  Superclass::UpdateInteractionPipeline();
}

//----------------------------------------------------------------------
void vtkSlicerRingRepresentation2D::GetActors(vtkPropCollection *pc)
{
  this->MiddlePointActor->GetActors(pc);
  this->RingActor->GetActors(pc);
  this->RadiusActor->GetActors(pc);
  this->TextActor->GetActors(pc);
  this->SliceViewCutActor->GetActors(pc);
  this->Superclass::GetActors(pc);
}

//----------------------------------------------------------------------
void vtkSlicerRingRepresentation2D::ReleaseGraphicsResources(vtkWindow *win)
{
  this->MiddlePointActor->ReleaseGraphicsResources(win);
  this->RingActor->ReleaseGraphicsResources(win);
  this->RadiusActor->ReleaseGraphicsResources(win);
  this->TextActor->ReleaseGraphicsResources(win);
  this->SliceViewCutActor->ReleaseGraphicsResources(win);
  this->Superclass::ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------
int vtkSlicerRingRepresentation2D::RenderOverlay(vtkViewport *viewport)
{
  int count=0;
  if (this->MiddlePointActor->GetVisibility())
    {
    count +=  this->MiddlePointActor->RenderOverlay(viewport);
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
  if (this->SliceViewCutActor->GetVisibility())
    {
      count +=  this->SliceViewCutActor->RenderOverlay(viewport);
    }
  count += this->Superclass::RenderOverlay(viewport);
  return count;
}

//-----------------------------------------------------------------------------
int vtkSlicerRingRepresentation2D::RenderOpaqueGeometry(vtkViewport *viewport)
{
  int count=0;
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
  if (this->SliceViewCutActor->GetVisibility())
    {
      count += this->SliceViewCutActor->RenderOpaqueGeometry(viewport);
    }
  count = this->Superclass::RenderOpaqueGeometry(viewport);
  return count;
}

//-----------------------------------------------------------------------------
int vtkSlicerRingRepresentation2D::RenderTranslucentPolygonalGeometry(vtkViewport *viewport)
{
  int count=0;
  if (this->MiddlePointActor->GetVisibility())
    {
    count += this->MiddlePointActor->RenderTranslucentPolygonalGeometry(viewport);
    }
  if (this->RingActor->GetVisibility())
    {
      count += this->RingActor->RenderTranslucentPolygonalGeometry(viewport);
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
vtkTypeBool vtkSlicerRingRepresentation2D::HasTranslucentPolygonalGeometry()
{
  if (this->Superclass::HasTranslucentPolygonalGeometry())
    {
    return true;
    }
  if (this->MiddlePointActor->GetVisibility() && this->MiddlePointActor->HasTranslucentPolygonalGeometry())
    {
    return true;
    }
  if (this->RingActor->GetVisibility() && this->RingActor->HasTranslucentPolygonalGeometry())
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

//****************************************************************************
/*
 * ////// Notes \\\\\\
 * GetSliceToWorldCoordinates :
 *  Result is a world coordinate, that is always on the slice; is a projection
 *  of the source world coordinate, in the current slice view orientation.
 * GetWorldToSliceCoordinates :
 *  Same result as GetNthControlPointDisplayPosition() - (p[0], p[1], 0)
 *  Not a world coordinate.
 * GetWorldToDisplayCoordinates :
 *  Same result as GetWorldToSliceCoordinates, plus an offset value - (p[0], p[1], offset)
 *  Not a world coordinate. Kind of 3D coordinate system unique for each slice view, where
 *  each axis value is influenced by translation, rotation and zooming.
 *  The latter does not alter the Z-axis however. It must be updated with ViewScaleFactorMmPerPixel.
*/

/*
 * ////// Comparative validation \\\\\\
 * In slice views, if we compare to a model that can be created from the ring's polydata, radius is very slightly 
 * greater in WorldProjection mode, as we get further away from control points.
 * Is it the result of different drawing techniques between vtkRingSource and vtkMRMLModelNode ?
 * 
 * r = slicer.util.getNode("RI")
 * mod = slicer.modules.models.logic().AddModel(r.GetRingWorld())
 */
