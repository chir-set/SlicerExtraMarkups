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

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerShapeRepresentation2D);

//------------------------------------------------------------------------------
vtkSlicerShapeRepresentation2D::vtkSlicerShapeRepresentation2D()
{
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
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::SetMarkupsNode(vtkMRMLMarkupsNode *markupsNode)
{
  if (this->MarkupsNode != markupsNode)
  {
    if (markupsNode)
    {
      //this->SliceDistance->SetInputConnection(markupsNode->GetCurveWorldConnection());
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
  // Final visibility handled by superclass in vtkSlicerMarkupsWidgetRepresentation
  Superclass::UpdateInteractionPipeline();
}

//----------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::GetActors(vtkPropCollection *pc)
{
  this->Superclass::GetActors(pc);
}

//----------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::ReleaseGraphicsResources(vtkWindow *win)
{
  this->Superclass::ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------
int vtkSlicerShapeRepresentation2D::RenderOverlay(vtkViewport *viewport)
{
  int count=0;
  count += this->Superclass::RenderOverlay(viewport);
  return count;
}

//-----------------------------------------------------------------------------
int vtkSlicerShapeRepresentation2D::RenderOpaqueGeometry(vtkViewport *viewport)
{
  int count=0;
  count = this->Superclass::RenderOpaqueGeometry(viewport);
  return count;
}

//-----------------------------------------------------------------------------
int vtkSlicerShapeRepresentation2D::RenderTranslucentPolygonalGeometry(vtkViewport *viewport)
{
  int count=0;
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
  return false;
}
