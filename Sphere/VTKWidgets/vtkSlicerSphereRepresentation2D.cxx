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
#include <vtkProperty2D.h>
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

  this->MiddlePointActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 2);
  this->SliceViewCutActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 2);
  this->RadiusActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 2);
  this->TextActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 2);

  // Hide the line actor if it doesn't intersect the current slice
  this->SliceDistance->Update();
  if (!this->IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->MiddlePointActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
  }

  if (markupsNode->GetNumberOfDefinedControlPoints(true) == 2)
  {
    double p1[3] = { 0.0 };
    double p2[3] = { 0.0 };
    this->GetNthControlPointDisplayPosition(0, p1);
    this->GetNthControlPointDisplayPosition(1, p2);
    double middlePointPos[2] = { (p1[0] + p2[0]) / 2.0, (p1[1] + p2[1]) / 2.0 };
    this->MiddlePointSource->SetCenter(middlePointPos[0], middlePointPos[1], 0.0);
    this->MiddlePointSource->Update();
    
    double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));  
    // Centered mode : p1 is center, line length is radius.
    if (sphereNode->GetMode() == vtkMRMLMarkupsSphereNode::Centered)
    { 
      this->SphereSource->SetCenter(p1);
      this->SphereSource->SetRadius(lineLength);
      this->MiddlePointActor->SetVisibility(false);
      this->RadiusSource->SetPoint1(p1);
    }
    // Circumferential mode : center is half way between p1 and p2, radius is half of line length.
    else
    {      
      this->SphereSource->SetCenter(middlePointPos[0], middlePointPos[1], 0.0);
      this->SphereSource->SetRadius(lineLength / 2.0);
      this->MiddlePointActor->SetVisibility(true);
      this->RadiusSource->SetPoint1(middlePointPos[0], middlePointPos[1], 0.0);
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
    this->MiddlePointActor->SetVisibility(false);
    this->SliceViewCutActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
  }

  this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->SliceViewCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
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
      this->SliceDistance->SetInputData(this->SliceViewCutter->GetOutput());
    }
  }
  this->Superclass::SetMarkupsNode(markupsNode);
}

//----------------------------------------------------------------------
void vtkSlicerSphereRepresentation2D::GetActors(vtkPropCollection *pc)
{
  this->MiddlePointActor->GetActors(pc);
  this->RadiusActor->GetActors(pc);
  this->SliceViewCutActor->GetActors(pc);
  this->TextActor->GetActors(pc);
  this->Superclass::GetActors(pc);
}

//----------------------------------------------------------------------
void vtkSlicerSphereRepresentation2D::ReleaseGraphicsResources(vtkWindow *win)
{
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
