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
//#include <vtkMRMLScene.h>

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

  this->CameraModifiedCallbackCommand = vtkSmartPointer<vtkCallbackCommand>::New();
  this->CameraModifiedCallbackCommand->SetClientData( reinterpret_cast<void *>(this) );
  this->CameraModifiedCallbackCommand->SetCallback( vtkSlicerLabelRepresentation3D::OnCameraModified );
}

//------------------------------------------------------------------------------
vtkSlicerLabelRepresentation3D::~vtkSlicerLabelRepresentation3D()
{
  if (this->CameraIsBeingObserved)
  {
    this->GetRenderer()->GetActiveCamera()->RemoveObserver(this->CameraModifiedCallbackCommand);
    //this->SetCameraObservationStatus(false);
  }
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
void vtkSlicerLabelRepresentation3D::UpdateFromMRML(vtkMRMLNode* caller,
                                                           unsigned long event,
                                                           void *callData /*=nullptr*/)
{
  this->Superclass::UpdateFromMRML(caller, event, callData);

  this->NeedToRenderOn();

  vtkMRMLMarkupsNode* markupsNode = this->GetMarkupsNode();
  if (!markupsNode || markupsNode->GetNumberOfDefinedControlPoints() != 2)
  {
    return;
  }
  vtkMRMLMarkupsLabelNode * labelNode = vtkMRMLMarkupsLabelNode::SafeDownCast(markupsNode);
  if (!this->CameraIsBeingObserved)
  {
    this->GetRenderer()->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, this->CameraModifiedCallbackCommand);
    this->CameraIsBeingObserved = true; // remove if using vtkMRMLCameraNode
    //this->SetCameraObservationStatus(true);
  }
  double p1[3] = { 0.0 };
  double p2[3] = { 0.0 };
  markupsNode->GetNthControlPointPositionWorld(0, p1);
  markupsNode->GetNthControlPointPositionWorld(1, p2);
  double direction[3] = { 0.0 };
  vtkMath::Subtract(p2, p1, direction);
  
  this->LineSource->SetPoint1(p1);
  this->LineSource->SetPoint2(p2);
  this->LineSource->Update();
  this->ConeSource->SetCenter(p2);
  this->ConeSource->SetRadius(1.0);
  this->ConeSource->SetHeight(2.5);
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
  this->TextActor->SetInput(labelNode->GetLabel().toStdString().c_str());
  this->TextActorPositionWorld[0] = p1[0];
  this->TextActorPositionWorld[1] = p1[1];
  this->TextActorPositionWorld[2] = p1[2];
  
  this->ArrowActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 2);
  this->TextActor->SetVisibility(markupsNode->GetNumberOfDefinedControlPoints(true) == 2);
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ArrowActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);

  //Hide a control point by decreasing its size.
  this->GetControlPointsPipeline(controlPointType)->GlyphMapper->SetScaleFactor(0.01);
  // Shrink the control points on markups creation.
  this->UpdateViewScaleFactor();
}

//---------------------------------------------------------------------------
/*
 * Try to hide a control point :
 *  - success : on rotation, translation
 *  - failure : on zooming
 * Same results with :
 * - vtkCamera observed via vtkCommand::ModifiedEvent
 * - vtkMRMLCameraNode observed via vtkMRMLCameraNode::CameraInteractionEvent
 * Got better, but insufficient.
 */
void vtkSlicerLabelRepresentation3D::OnCameraModified(vtkObject *caller,
                 unsigned long event, void *clientData, void *callData)
{
  vtkSlicerLabelRepresentation3D * client = reinterpret_cast<vtkSlicerLabelRepresentation3D*>(clientData);
  if (!client)
  {
    return;
  }
  int controlPointType = client->GetAllControlPointsSelected() ? Selected : Unselected;
  client->GetControlPointsPipeline(controlPointType)->GlyphMapper->SetScaleFactor(0.01);
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


//---------------------------------------------------------------------------
/*bool vtkSlicerLabelRepresentation3D::SetCameraObservationStatus(bool observe)
{
  vtkWeakPointer<vtkCollection> mrmlCameraNodes = this->GetViewNode()->GetScene()->GetNodesByClass("vtkMRMLCameraNode");
  if (!mrmlCameraNodes.GetPointer())
  {
    return false;
  }
  for (uint i = 0; i < mrmlCameraNodes->GetNumberOfItems(); i++)
  {
    this->MRMLCamera = vtkMRMLCameraNode::SafeDownCast(mrmlCameraNodes->GetItemAsObject(i));
    if (!this->MRMLCamera.GetPointer())
    {
      return false;
    }
    if (this->MRMLCamera->GetCamera() == this->GetRenderer()->GetActiveCamera())
    {
      if (observe)
      {
        this->MRMLCamera->AddObserver(vtkMRMLCameraNode::CameraInteractionEvent, this->CameraModifiedCallbackCommand);
      }
      else
      {
        this->MRMLCamera->RemoveObserver(this->CameraModifiedCallbackCommand);
      }
      this->CameraIsBeingObserved = observe;
      return true;
    }
  }
  return false;
}*/
