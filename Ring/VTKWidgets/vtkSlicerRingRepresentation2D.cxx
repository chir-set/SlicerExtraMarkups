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

  this->MiddlePointActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 3);
  this->RingActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 3);
  this->RadiusActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 3);
  this->TextActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 3);

  if (markupsNode->GetNumberOfDefinedControlPoints(true) == 3)
  {
    double p1[3] = { 0.0 };
    double p2[3] = { 0.0 };
    double p3[3] = { 0.0 };
    this->GetNthControlPointDisplayPosition(0, p1);
    this->GetNthControlPointDisplayPosition(1, p2);
    this->GetNthControlPointDisplayPosition(2, p3);
    double middlePointPos[2] = { (p1[0] + p2[0]) / 2.0, (p1[1] + p2[1]) / 2.0 };
    // this->MiddlePointActor->SetDisplayPosition(static_cast<int>(middlePointPos[0]),
    //                                            static_cast<int>(middlePointPos[1]));
    this->MiddlePointSource->SetCenter(middlePointPos[0], middlePointPos[1], 0.0);
    this->MiddlePointSource->Update();
    this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
  
    double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));    
    const double normal[3] = { 0.0, 0.0, 1.0 };
    
    // Centered mode : p1 is center, line length is radius.
    if (ringNode->GetMode() == vtkMRMLMarkupsRingNode::Centered)
    { 
      this->RingSource->SetCenter(p1);
      this->RingSource->SetNormal(normal);
      this->RingSource->SetOuterRadius(lineLength);
      this->RingSource->SetInnerRadius(lineLength - 1.0);

      this->MiddlePointActor->SetVisibility(false);
      this->RadiusSource->SetPoint1(p1);
    }
    // Circumferentiale mode : center is half way between p1 and p2, radius is half of line length.
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
    
    this->RingSource->SetCircumferentialResolution((int) ringNode->GetResolution());
    this->RingSource->Update();
    
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
  }
  
  // Hide the line actor if it doesn't intersect the current slice
  this->SliceDistance->Update();
  if (!Superclass::IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->MiddlePointActor->SetVisibility(false);
    this->RingActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
  }
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->RingActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
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
  this->Superclass::GetActors(pc);
}

//----------------------------------------------------------------------
void vtkSlicerRingRepresentation2D::ReleaseGraphicsResources(vtkWindow *win)
{
  this->MiddlePointActor->ReleaseGraphicsResources(win);
  this->RingActor->ReleaseGraphicsResources(win);
  this->RadiusActor->ReleaseGraphicsResources(win);
  this->TextActor->ReleaseGraphicsResources(win);
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
  return false;
}