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

#include "vtkSlicerLabelRepresentation3D.h"

#include "vtkMRMLMarkupsLabelNode.h"

// VTK includes
#include <vtkActor.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkGlyph3DMapper.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerLabelRepresentation3D);

//------------------------------------------------------------------------------
vtkSlicerLabelRepresentation3D::vtkSlicerLabelRepresentation3D()
{
  this->LineSource = vtkSmartPointer<vtkLineSource>::New();
  this->ConeSource = vtkSmartPointer<vtkConeSource>::New();
  this->AppendedArrow = vtkSmartPointer<vtkAppendPolyData>::New();
  
  this->AppendedArrow->AddInputConnection(this->LineSource->GetOutputPort());
  this->AppendedArrow->AddInputConnection(this->ConeSource->GetOutputPort());
  
  this->ArrowMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->ArrowMapper->SetInputConnection(this->AppendedArrow->GetOutputPort());
  this->ArrowMapper->SetScalarVisibility(true);
  
  this->ArrowActor = vtkSmartPointer<vtkActor>::New();
  this->ArrowActor->SetMapper(this->ArrowMapper);
  this->ArrowActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
  
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(Unselected)->TextProperty);
  this->ConeSource->SetAngle(30.0);
}

//------------------------------------------------------------------------------
vtkSlicerLabelRepresentation3D::~vtkSlicerLabelRepresentation3D()
{
}

//------------------------------------------------------------------------------
void vtkSlicerLabelRepresentation3D::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
  
  if (this->ArrowActor)
  {
    os << indent << "Arrow Visibility: " << this->ArrowActor->GetVisibility() << "\n";
  }
  else
  {
    os << indent << "Arrow Visibility: (none)\n";
  }
}

//------------------------------------------------------------------------------
void vtkSlicerLabelRepresentation3D::GetActors(vtkPropCollection* pc)
{
  this->Superclass::GetActors(pc);

  this->ArrowActor->GetActors(pc);
  this->TextActor->GetActors(pc);
}

//------------------------------------------------------------------------------
void vtkSlicerLabelRepresentation3D::ReleaseGraphicsResources(vtkWindow* win)
{
  this->Superclass::ReleaseGraphicsResources(win);

  this->ArrowActor->ReleaseGraphicsResources(win);
  this->TextActor->ReleaseGraphicsResources(win);
}

//------------------------------------------------------------------------------
int vtkSlicerLabelRepresentation3D::RenderOverlay(vtkViewport* viewport)
{
  int count = this->Superclass::RenderOverlay(viewport);

  if (this->ArrowActor->GetVisibility())
    {
    count += this->ArrowActor->RenderOverlay(viewport);
    }
  if (this->TextActor->GetVisibility())
    {
      count += this->TextActor->RenderOverlay(viewport);
    }
  return count;
}

//------------------------------------------------------------------------------
int vtkSlicerLabelRepresentation3D::RenderOpaqueGeometry(vtkViewport* viewport)
{
  int count = this->Superclass::RenderOpaqueGeometry(viewport);

  if (this->ArrowActor->GetVisibility())
    {
    count += this->ArrowActor->RenderOpaqueGeometry(viewport);
    }
  if (this->TextActor->GetVisibility())
    {
      count += this->TextActor->RenderOpaqueGeometry(viewport);
    }

  return count;
}

//------------------------------------------------------------------------------
int vtkSlicerLabelRepresentation3D::RenderTranslucentPolygonalGeometry(vtkViewport* viewport)
{
  int count = this->Superclass::RenderTranslucentPolygonalGeometry(viewport);

  if (this->ArrowActor->GetVisibility())
    {
    this->ArrowActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->ArrowActor->RenderTranslucentPolygonalGeometry(viewport);
    }
  if (this->TextActor->GetVisibility())
    {
      this->TextActor->SetPropertyKeys(this->GetPropertyKeys());
      count += this->TextActor->RenderTranslucentPolygonalGeometry(viewport);
    }

  return count;
}

//------------------------------------------------------------------------------
vtkTypeBool vtkSlicerLabelRepresentation3D::HasTranslucentPolygonalGeometry()
{
  if (this->Superclass::HasTranslucentPolygonalGeometry())
    {
    return true;
    }

  if (this->ArrowActor->GetVisibility() &&
      this->ArrowActor->HasTranslucentPolygonalGeometry())
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
void vtkSlicerLabelRepresentation3D::UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  this->Superclass::UpdateFromMRML(caller, event, callData);
  this->NeedToRenderOn();

  vtkMRMLMarkupsNode* markupsNode = this->GetMarkupsNode();
  if (!markupsNode || markupsNode->GetNumberOfDefinedControlPoints(true) == 0)
  {
    return;
  }
  
  this->ArrowActor->SetVisibility(false);
  this->TextActor->SetVisibility(false);
  switch (markupsNode->GetNumberOfDefinedControlPoints(true))
  {
    case 1:
      this->UpdateTagFromMRML(caller, event, callData);
      break;
    case 2:
      this->UpdatePointerFromMRML(caller, event, callData);
      break;
    default:
      vtkErrorMacro("Number of control points out of range.");
  }
}

//----------------------------------------------------------------------
void vtkSlicerLabelRepresentation3D::UpdateTagFromMRML(vtkMRMLNode* caller,
                                                    unsigned long event,
                                                    void *callData /*=nullptr*/)
{
  vtkMRMLMarkupsNode* markupsNode = this->GetMarkupsNode();
  if (!markupsNode || markupsNode->GetNumberOfDefinedControlPoints() != 1)
  {
    return;
  }
  vtkMRMLMarkupsLabelNode * labelNode = vtkMRMLMarkupsLabelNode::SafeDownCast(markupsNode);
  
  double p1[3] = { 0.0 };
  markupsNode->GetNthControlPointPositionWorld(0, p1);

  this->TextActor->SetInput(labelNode->GetLabel());
  this->TextActorPositionWorld[0] = p1[0];
  this->TextActorPositionWorld[1] = p1[1];
  this->TextActorPositionWorld[2] = p1[2];
  
  this->TextActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 1);
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  //Hide a control point by decreasing its size.
  this->GetControlPointsPipeline(controlPointType)->GlyphMapper->SetScaleFactor(0.01);
  // Shrink the control points on markups creation.
  this->UpdateViewScaleFactor();
}

//----------------------------------------------------------------------
void vtkSlicerLabelRepresentation3D::UpdatePointerFromMRML(vtkMRMLNode* caller,
                                                           unsigned long event,
                                                           void *callData /*=nullptr*/)
{
  vtkMRMLMarkupsNode* markupsNode = this->GetMarkupsNode();
  if (!markupsNode || markupsNode->GetNumberOfDefinedControlPoints(true) != 2)
  {
    return;
  }
  vtkMRMLMarkupsLabelNode * labelNode = vtkMRMLMarkupsLabelNode::SafeDownCast(markupsNode);

  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  markupsNode->GetNthControlPointPositionWorld(0, p1);
  markupsNode->GetNthControlPointPositionWorld(1, p2);
  double direction[3] = { 0.0 };
  vtkMath::Subtract(p2, p1, direction);
  
  const double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  double radius = 6.0 * this->ViewScaleFactorMmPerPixel;

  switch (labelNode->GetTipDimensionMode3D())
  {
    case vtkMRMLMarkupsLabelNode::LineLength:
      radius = 0.03 * lineLength;
      break;
    case vtkMRMLMarkupsLabelNode::ViewScaleFactor:
      break;
    case vtkMRMLMarkupsLabelNode::Fixed:
      radius = 1.0;
      break;
    default:
      vtkErrorMacro("Unknown 3D tip dimension mode, using default.");
      break;
  }
  const double height = 3.0 * radius;
  
  this->LineSource->SetPoint1(p1);
  this->LineSource->SetPoint2(p2);
  this->LineSource->Update();
  this->ConeSource->SetCenter(p2);
  this->ConeSource->SetRadius(radius);
  this->ConeSource->SetHeight(height);
  this->ConeSource->SetResolution(45);
  this->ConeSource->SetDirection(direction);
  this->ConeSource->Update();
  
  // p1, the center (tip) of the cone, has always id 0.
  vtkIdType closestIdToP2 = this->ConeSource->GetOutput()->FindPoint(p1);
  // If the tip is closest, it's inversely oriented.
  if (closestIdToP2 == 0)
  {
    // Invert direction.
    vtkMath::Subtract(p1, p2, direction);
    this->ConeSource->SetDirection(direction);
    this->ConeSource->Update();
  }
  
  this->AppendedArrow->Update();
  this->TextActor->SetInput(labelNode->GetLabel());
  this->TextActorPositionWorld[0] = p1[0];
  this->TextActorPositionWorld[1] = p1[1];
  this->TextActorPositionWorld[2] = p1[2];
  
  this->ArrowActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ArrowActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);

  //Hide a control point by decreasing its size.
  this->GetControlPointsPipeline(controlPointType)->GlyphMapper->SetScaleFactor(0.01);
  // Shrink the control points on markups creation.
  this->UpdateViewScaleFactor();
}

//----------------------------------------------------------------------
double * vtkSlicerLabelRepresentation3D::GetBounds()
{
  vtkBoundingBox boundingBox;
  const std::vector<vtkProp*> actors({ this->ArrowActor });
  this->AddActorsBounds(boundingBox, actors, Superclass::GetBounds());
  boundingBox.GetBounds(this->Bounds);
  return this->Bounds;
}
