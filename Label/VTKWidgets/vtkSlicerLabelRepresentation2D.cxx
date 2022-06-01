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

#include "vtkSlicerLabelRepresentation2D.h"

#include <vtkActor2D.h>
#include <vtkGlyphSource2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkPlane.h>

#include <cmath>

#include "vtkMRMLMarkupsLabelNode.h"

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerLabelRepresentation2D);

//------------------------------------------------------------------------------
vtkSlicerLabelRepresentation2D::vtkSlicerLabelRepresentation2D()
{
  this->SliceDistance = vtkSmartPointer<vtkSampleImplicitFunctionFilter>::New();
  this->SliceDistance->SetImplicitFunction(this->SlicePlane);
  
  this->WorldToSliceTransformer = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->WorldToSliceTransformer->SetTransform(this->WorldToSliceTransform);
  this->WorldToSliceTransformer->SetInputConnection(this->SliceDistance->GetOutputPort());
  
  this->LineSource = vtkSmartPointer<vtkLineSource>::New();
  this->LineSource->SetInputConnection(this->WorldToSliceTransformer->GetOutputPort());
  
  this->LineMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->LineMapper->SetInputConnection(this->LineSource->GetOutputPort());
  this->LineMapper->SetScalarVisibility(true);
  
  this->LineActor = vtkSmartPointer<vtkActor2D>::New();
  this->LineActor->SetMapper(this->LineMapper);
  this->LineActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
  
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(Unselected)->TextProperty);
}

//------------------------------------------------------------------------------
vtkSlicerLabelRepresentation2D::~vtkSlicerLabelRepresentation2D() = default;

//------------------------------------------------------------------------------
void vtkSlicerLabelRepresentation2D::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}
// -----------------------------------------------------------------------------
void vtkSlicerLabelRepresentation2D::UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData /*=nullptr*/)
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
  vtkMRMLMarkupsLabelNode * labelNode = vtkMRMLMarkupsLabelNode::SafeDownCast(markupsNode);

  this->LineActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 2);
  this->TextActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 2);

  // Hide the line actor if it doesn't intersect the current slice
  this->SliceDistance->Update();
  if (!this->IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
    {
    this->LineActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    }

  // The control point arrow glyph is used instead of the default sphere.
  double glyphRotationAngle = 0.0;
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  if (markupsNode->GetNumberOfDefinedControlPoints(true) == 2)
  {
    this->GetNthControlPointDisplayPosition(0, p1);
    this->GetNthControlPointDisplayPosition(1, p2);
  
    glyphRotationAngle = vtkMath::DegreesFromRadians(std::atan((p2[1] - p1[1]) / (p2[0] - p1[0])));
    // On negative x
    if ((p2[0] >= p1[0]))
    {
      glyphRotationAngle += 180.0;
    }
    this->LineSource->SetPoint1(p1);
    this->LineSource->SetPoint2(p2);
    this->LineSource->Update();
  }
  else
  {
    this->LineActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
  }
  
  // Hide the line actor if it doesn't intersect the current slice
  this->SliceDistance->Update();
  if (!Superclass::IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->LineActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
  }
  
  this->TextActor->SetInput(labelNode->GetLabel().toStdString().c_str());
  this->TextActor->SetPosition(p1[0], p1[1]);
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->LineActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  vtkMarkupsGlyphSource2D * glyphSource2D = this->GetControlPointsPipeline(controlPointType)->GlyphSource2D;
  /* Unfortunately, there is apparently no selective control of the glyph to use on each control point.
   * ::None would have looked better for p1. ::None for p1 and p2 is still a good option.
   * We choose the arrow glyph, through it's not good looking for p1.
   */
  glyphSource2D->SetGlyphTypeToArrow();
  glyphSource2D->SetRotationAngle(glyphRotationAngle);
  glyphSource2D->SetScale(2.0);
}

//-----------------------------------------------------------------------------
void vtkSlicerLabelRepresentation2D::SetMarkupsNode(vtkMRMLMarkupsNode *markupsNode)
{
  if (this->MarkupsNode != markupsNode)
  {
    if (markupsNode)
    {
      this->SliceDistance->SetInputConnection(markupsNode->GetCurveWorldConnection());
    }
    /*else
    {
      this->SliceDistance->SetInputData(this->LabelSource->GetOutput());
    }*/
  }
  this->Superclass::SetMarkupsNode(markupsNode);
}

//----------------------------------------------------------------------
void vtkSlicerLabelRepresentation2D::GetActors(vtkPropCollection *pc)
{
  this->LineActor->GetActors(pc);
  this->TextActor->GetActors(pc);
  this->Superclass::GetActors(pc);
}

//----------------------------------------------------------------------
void vtkSlicerLabelRepresentation2D::ReleaseGraphicsResources(vtkWindow *win)
{
  this->LineActor->ReleaseGraphicsResources(win);
  this->TextActor->ReleaseGraphicsResources(win);
  this->Superclass::ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------
int vtkSlicerLabelRepresentation2D::RenderOverlay(vtkViewport *viewport)
{
  int count=0;
  if (this->LineActor->GetVisibility())
    {
    count +=  this->LineActor->RenderOverlay(viewport);
    }
  if (this->TextActor->GetVisibility())
    {
      count +=  this->TextActor->RenderOverlay(viewport);
    }
  count += this->Superclass::RenderOverlay(viewport);
  return count;
}

//-----------------------------------------------------------------------------
int vtkSlicerLabelRepresentation2D::RenderOpaqueGeometry(vtkViewport *viewport)
{
  int count=0;
  if (this->LineActor->GetVisibility())
    {
    count += this->LineActor->RenderOpaqueGeometry(viewport);
    }
  if (this->TextActor->GetVisibility())
    {
      count += this->TextActor->RenderOpaqueGeometry(viewport);
    }
  count = this->Superclass::RenderOpaqueGeometry(viewport);
  return count;
}

//-----------------------------------------------------------------------------
int vtkSlicerLabelRepresentation2D::RenderTranslucentPolygonalGeometry(vtkViewport *viewport)
{
  int count=0;
  if (this->LineActor->GetVisibility())
    {
    count += this->LineActor->RenderTranslucentPolygonalGeometry(viewport);
    }
  if (this->TextActor->GetVisibility())
    {
      count += this->TextActor->RenderTranslucentPolygonalGeometry(viewport);
    }
  count = this->Superclass::RenderTranslucentPolygonalGeometry(viewport);
  return count;
}

//-----------------------------------------------------------------------------
vtkTypeBool vtkSlicerLabelRepresentation2D::HasTranslucentPolygonalGeometry()
{
  if (this->Superclass::HasTranslucentPolygonalGeometry())
    {
    return true;
    }
  if (this->LineActor->GetVisibility() && this->LineActor->HasTranslucentPolygonalGeometry())
    {
    return true;
    }
    if (this->TextActor->GetVisibility() && this->TextActor->HasTranslucentPolygonalGeometry())
    {
      return true;
    }
  return false;
}
