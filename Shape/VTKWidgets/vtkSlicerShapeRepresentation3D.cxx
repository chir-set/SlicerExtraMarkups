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

#include "vtkSlicerShapeRepresentation3D.h"

#include "vtkMRMLMarkupsShapeNode.h"

// VTK includes
#include <vtkActor.h>
#include <vtkCutter.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>


//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerShapeRepresentation3D);

//------------------------------------------------------------------------------
vtkSlicerShapeRepresentation3D::vtkSlicerShapeRepresentation3D()
{
  this->MiddlePointSource = vtkSmartPointer<vtkSphereSource>::New();

  this->MiddlePointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->MiddlePointMapper->SetInputConnection(this->MiddlePointSource->GetOutputPort());

  this->MiddlePointActor = vtkSmartPointer<vtkActor>::New();
  this->MiddlePointActor->SetMapper(this->MiddlePointMapper);
}

//------------------------------------------------------------------------------
vtkSlicerShapeRepresentation3D::~vtkSlicerShapeRepresentation3D() = default;

//------------------------------------------------------------------------------
void vtkSlicerShapeRepresentation3D::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
void vtkSlicerShapeRepresentation3D::GetActors(vtkPropCollection* pc)
{
  this->Superclass::GetActors(pc);

  /*if (this->TargetOrgan)
  {
    this->ContourActor->GetActors(pc);
  }*/

  this->MiddlePointActor->GetActors(pc);
}

//------------------------------------------------------------------------------
void vtkSlicerShapeRepresentation3D::ReleaseGraphicsResources(vtkWindow* win)
{
  this->Superclass::ReleaseGraphicsResources(win);

  /*if (this->TargetOrgan)
  {
    this->ContourActor->ReleaseGraphicsResources(win);
   }*/

  this->MiddlePointActor->ReleaseGraphicsResources(win);
}

//------------------------------------------------------------------------------
int vtkSlicerShapeRepresentation3D::RenderOverlay(vtkViewport* viewport)
{
  int count = this->Superclass::RenderOverlay(viewport);
  /*if (this->TargetOrgan && this->ContourActor->GetVisibility())
  {
    count += this->ContourActor->RenderOverlay(viewport);
   }*/

  if (this->MiddlePointActor->GetVisibility())
    {
    count += this->MiddlePointActor->RenderOverlay(viewport);
    }
  return count;
}

//------------------------------------------------------------------------------
int vtkSlicerShapeRepresentation3D::RenderOpaqueGeometry(vtkViewport* viewport)
{
  int count = this->Superclass::RenderOpaqueGeometry(viewport);
  /*if (this->TargetOrgan && this->ContourActor->GetVisibility())
  {
    count += this->ContourActor->RenderOpaqueGeometry(viewport);
  }*/

  if (this->MiddlePointActor->GetVisibility())
    {
    count += this->MiddlePointActor->RenderOpaqueGeometry(viewport);
    }

  return count;
}

//------------------------------------------------------------------------------
int vtkSlicerShapeRepresentation3D::RenderTranslucentPolygonalGeometry(vtkViewport* viewport)
{
  int count = this->Superclass::RenderTranslucentPolygonalGeometry(viewport);
  /*if (this->TargetOrgan && this->ContourActor->GetVisibility())
  {
    this->ContourActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->ContourActor->RenderTranslucentPolygonalGeometry(viewport);
  }*/

  if (this->MiddlePointActor->GetVisibility())
    {
    this->MiddlePointActor->SetPropertyKeys(this->GetPropertyKeys());
    count += this->MiddlePointActor->RenderTranslucentPolygonalGeometry(viewport);
    }

  return count;
}

//------------------------------------------------------------------------------
vtkTypeBool vtkSlicerShapeRepresentation3D::HasTranslucentPolygonalGeometry()
{
  if (this->Superclass::HasTranslucentPolygonalGeometry())
    {
    return true;
    }

  /*if (this->TargetOrgan && this->ContourActor->GetVisibility() &&
      this->ContourActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }*/

  if (this->MiddlePointActor->GetVisibility() &&
      this->MiddlePointActor->HasTranslucentPolygonalGeometry())
    {
    return true;
    }

  return false;
}

//----------------------------------------------------------------------
void vtkSlicerShapeRepresentation3D::UpdateFromMRML(vtkMRMLNode* caller,
                                                           unsigned long event,
                                                           void *callData /*=nullptr*/)
{
 this->Superclass::UpdateFromMRML(caller, event, callData);

 this->NeedToRenderOn();

 this->BuildMiddlePoint();

 vtkMRMLMarkupsShapeNode* shapeNode=
   vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());

 if (!shapeNode)
   {
   return;
   }
}

//------------------------------------------------------------------------------
void vtkSlicerShapeRepresentation3D::BuildMiddlePoint()
{
  vtkMRMLMarkupsNode* markupsNode = this->GetMarkupsNode();
  if (!markupsNode)
    {
    return;
    }

  if (markupsNode->GetNumberOfControlPoints() != 2)
    {
    return;
    }

  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  double center[3] = {0.0};
  markupsNode->GetNthControlPointPositionWorld(0, p1);
  markupsNode->GetNthControlPointPositionWorld(1, p2);
  center[0] = (p1[0] + p2[0]) / 2.0;
  center[1] = (p1[1] + p2[1]) / 2.0;
  center[2] = (p1[2] + p2[2]) / 2.0;

  this->MiddlePointSource->SetCenter(center);
  this->MiddlePointSource->SetRadius(this->ControlPointSize);
}
