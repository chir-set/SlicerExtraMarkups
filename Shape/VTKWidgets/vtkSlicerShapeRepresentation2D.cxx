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
#include <vtkTupleInterpolator.h>
#include <vtkPointData.h>
#include <vtkMatrix4x4.h>

// TODO: Fix opacity of shape and intersection actors in Projection mode.
//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerShapeRepresentation2D);

//------------------------------------------------------------------------------
vtkSlicerShapeRepresentation2D::vtkSlicerShapeRepresentation2D()
{
  this->SliceDistance = vtkSmartPointer<vtkSampleImplicitFunctionFilter>::New();
  this->SliceDistance->SetImplicitFunction(this->SlicePlane);
  
  this->ShapeWorldToSliceTransformer = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->ShapeWorldToSliceTransformer->SetTransform(this->WorldToSliceTransform);
  this->ShapeCutWorldToSliceTransformer = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->ShapeCutWorldToSliceTransformer->SetTransform(this->WorldToSliceTransform);

  this->SplineWorldToSliceTransformer = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->SplineWorldToSliceTransformer->SetTransform(this->WorldToSliceTransform);
  this->SplineCutWorldToSliceTransformer = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->SplineCutWorldToSliceTransformer->SetTransform(this->WorldToSliceTransform);

  this->MiddlePointSource = vtkSmartPointer<vtkGlyphSource2D>::New();
  this->MiddlePointSource->SetCenter(0.0, 0.0, 0.0);
  this->MiddlePointSource->SetScale(5);
  this->MiddlePointDataMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->MiddlePointDataMapper->SetInputConnection(this->MiddlePointSource->GetOutputPort());
  this->MiddlePointActor = vtkSmartPointer<vtkActor2D>::New();
  this->MiddlePointActor->SetMapper(this->MiddlePointDataMapper);
  
  this->DiskSource = vtkSmartPointer<vtkDiskSource>::New();
  this->RingSource = vtkSmartPointer<vtkDiskSource>::New();
  this->SphereSource = vtkSmartPointer<vtkSphereSource>::New();
  this->ConeSource = vtkSmartPointer<vtkConeSource>::New();
  this->ConeSource->CappingOn();
  
  this->RadiusSource = vtkSmartPointer<vtkLineSource>::New();
  this->RadiusMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->RadiusMapper->SetInputConnection(this->RadiusSource->GetOutputPort());
  this->RadiusActor = vtkSmartPointer<vtkActor2D>::New();
  this->RadiusActor->SetMapper(this->RadiusMapper);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(Unselected)->Property);
  
  this->ShapeMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->ShapeProperty = vtkSmartPointer<vtkProperty2D>::New();
  
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(Unselected)->Property);
  this->ShapeActor = vtkSmartPointer<vtkActor2D>::New();
  this->ShapeActor->SetMapper(this->ShapeMapper);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  
  this->Spline = vtkSmartPointer<vtkParametricSpline>::New();
  vtkNew<vtkPoints> points;
  const double point[3] = { 0.0 };
  points->InsertNextPoint(point);
  this->Spline->SetPoints(points);
  this->SplineFunctionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
  this->SplineFunctionSource->SetParametricFunction(this->Spline);
  this->Tube = vtkSmartPointer<vtkTubeFilter>::New();
  this->Tube->SetNumberOfSides(20);
  this->Tube->SetVaryRadiusToVaryRadiusByAbsoluteScalar();
  this->Tube->SetInputConnection(this->SplineFunctionSource->GetOutputPort());
  // To be consistent with 3D views. The cap is projected or intersected in slice views.
  this->CappedTube = vtkSmartPointer<vtkTubeFilter>::New();
  this->CappedTube->SetNumberOfSides(20);
  this->CappedTube->SetVaryRadiusToVaryRadiusByAbsoluteScalar();
  this->CappedTube->SetInputConnection(this->SplineFunctionSource->GetOutputPort());
  this->CappedTube->SetCapping(true);

  this->SplineMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->SplineActor = vtkSmartPointer<vtkActor2D>::New();
  this->SplineActor->SetMapper(this->SplineMapper);
  this->SplineActor->SetProperty(this->ShapeProperty);

  this->CylinderAxis = vtkSmartPointer<vtkLineSource>::New();
  this->CylinderSource = vtkSmartPointer<vtkTubeFilter>::New();
  this->CylinderSource->SetNumberOfSides(20);
  this->CylinderSource->SetInputConnection(this->CylinderAxis->GetOutputPort());
  this->CylinderSource->SetCapping(true);
  
  this->ArcSource = vtkSmartPointer<vtkArcSource>::New();
  this->ArcSource->UseNormalAndAngleOn();
  
  this->WorldPlane = vtkSmartPointer<vtkPlane>::New();
  this->WorldCutter = vtkSmartPointer<vtkCutter>::New();
  this->WorldCutter->SetCutFunction(this->WorldPlane);
  this->WorldCutMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->WorldCutActor = vtkSmartPointer<vtkActor2D>::New();
  this->WorldCutActor->SetMapper(this->WorldCutMapper);

  this->SplineWorldCutter = vtkSmartPointer<vtkCutter>::New();
  this->SplineWorldCutter->SetCutFunction(this->WorldPlane);
  this->SplineWorldCutMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->SplineWorldCutActor = vtkSmartPointer<vtkActor2D>::New();
  this->SplineWorldCutActor->SetMapper(this->SplineWorldCutMapper);
  
  this->Ellipsoid = vtkSmartPointer<vtkParametricSuperEllipsoid>::New();
  this->Toroid = vtkSmartPointer<vtkParametricSuperToroid>::New();
  this->BohemianDome = vtkSmartPointer<vtkParametricBohemianDome>::New();
  this->Bour = vtkSmartPointer<vtkParametricBour>::New();
  this->Boy = vtkSmartPointer<vtkParametricBoy>::New();
  this->CrossCap = vtkSmartPointer<vtkParametricCrossCap>::New();
  this->ConicSpiral = vtkSmartPointer<vtkParametricConicSpiral>::New();
  this->Kuen = vtkSmartPointer<vtkParametricKuen>::New();
  this->Mobius = vtkSmartPointer<vtkParametricMobius>::New();
  this->PluckerConoid = vtkSmartPointer<vtkParametricPluckerConoid>::New();
  this->Roman = vtkSmartPointer<vtkParametricRoman>::New();
  this->ParametricFunctionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
  
  this->ParametricMiddlePointSource = vtkSmartPointer<vtkGlyphSource2D>::New();
  this->ParametricMiddlePointSource->SetCenter(0.0, 0.0, 0.0);
  this->ParametricMiddlePointSource->SetScale(5);
  this->ParametricMiddlePointMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
  this->ParametricMiddlePointMapper->SetInputConnection(this->ParametricMiddlePointSource->GetOutputPort());
  this->ParametricMiddlePointActor = vtkSmartPointer<vtkActor2D>::New();
  this->ParametricMiddlePointActor->SetMapper(this->ParametricMiddlePointMapper);
  this->ParametricTransform = vtkSmartPointer<vtkTransform>::New();
  this->ParametricTransformer = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->ParametricTransformer->SetTransform(this->ParametricTransform);
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
  // NOTE: the WorldCutter is a determinant in many functions.
  Superclass::UpdateFromMRML(caller, event, callData);

  this->NeedToRenderOn();

  vtkMRMLMarkupsShapeNode* shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  if (!shapeNode)
  {
    return;
  }

  this->RadiusMapper->SetScalarVisibility(shapeNode->GetScalarVisibility());
  this->ShapeMapper->SetScalarVisibility(shapeNode->GetScalarVisibility());
  this->WorldCutMapper->SetScalarVisibility(shapeNode->GetScalarVisibility());
  this->SplineMapper->SetScalarVisibility(shapeNode->GetScalarVisibility());
  this->SplineWorldCutMapper->SetScalarVisibility(shapeNode->GetScalarVisibility());

  this->MiddlePointActor->SetVisibility(false);
  this->RadiusActor->SetVisibility(false);
  this->ShapeActor->SetVisibility(false);
  this->TextActor->SetVisibility(false);
  this->WorldCutActor->SetVisibility(false);
  this->SplineActor->SetVisibility(false);
  this->SplineWorldCutActor->SetVisibility(false);

  if (!shapeNode->IsParametric())
  {
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
      case vtkMRMLMarkupsShapeNode::Tube :
        this->UpdateTubeFromMRML(caller, event, callData);
        break;
      case vtkMRMLMarkupsShapeNode::Cone :
        this->UpdateConeFromMRML(caller, event, callData);
        break;
      case vtkMRMLMarkupsShapeNode::Cylinder :
        this->UpdateCylinderFromMRML(caller, event, callData);
        break;
      case vtkMRMLMarkupsShapeNode::Arc :
        this->UpdateArcFromMRML(caller, event, callData);
        break;
      case vtkMRMLMarkupsShapeNode::ShapeName_Last :
        break;
      default:
        vtkErrorMacro("Unknown shape.");
        return;
    }
  }
  else
  {
    this->UpdateParametricFromMRML(caller, event, callData);
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
      // ??
    }
  }
  this->Superclass::SetMarkupsNode(markupsNode);
}

//----------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::GetActors(vtkPropCollection *pc)
{
  this->MiddlePointActor->GetActors(pc);
  this->ParametricMiddlePointActor->GetActors(pc);
  this->ShapeActor->GetActors(pc);
  this->SplineActor->GetActors(pc);
  this->RadiusActor->GetActors(pc);
  this->TextActor->GetActors(pc);
  this->WorldCutActor->GetActors(pc);
  this->SplineWorldCutActor->GetActors(pc);
  this->Superclass::GetActors(pc);
}

//----------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::ReleaseGraphicsResources(vtkWindow *win)
{
  this->MiddlePointActor->ReleaseGraphicsResources(win);
  this->ParametricMiddlePointActor->ReleaseGraphicsResources(win);
  this->ShapeActor->ReleaseGraphicsResources(win);
  this->SplineActor->ReleaseGraphicsResources(win);
  this->RadiusActor->ReleaseGraphicsResources(win);
  this->TextActor->ReleaseGraphicsResources(win);
  this->WorldCutActor->ReleaseGraphicsResources(win);
  this->SplineWorldCutActor->ReleaseGraphicsResources(win);
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
  if (this->ParametricMiddlePointActor->GetVisibility())
  {
    count +=  this->ParametricMiddlePointActor->RenderOverlay(viewport);
  }
  if (this->ShapeActor->GetVisibility())
  {
    count +=  this->ShapeActor->RenderOverlay(viewport);
  }
  if (this->SplineActor->GetVisibility())
  {
    count +=  this->SplineActor->RenderOverlay(viewport);
  }
  if (this->RadiusActor->GetVisibility())
  {
    count +=  this->RadiusActor->RenderOverlay(viewport);
  }
  if (this->TextActor->GetVisibility())
  {
    count +=  this->TextActor->RenderOverlay(viewport);
  }
  if (this->WorldCutActor->GetVisibility())
  {
    count +=  this->WorldCutActor->RenderOverlay(viewport);
  }
  if (this->SplineWorldCutActor->GetVisibility())
  {
    count +=  this->SplineWorldCutActor->RenderOverlay(viewport);
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
  if (this->ParametricMiddlePointActor->GetVisibility())
  {
    count += this->ParametricMiddlePointActor->RenderOpaqueGeometry(viewport);
  }
  if (this->ShapeActor->GetVisibility())
  {
    count +=  this->ShapeActor->RenderOverlay(viewport);
  }
  if (this->SplineActor->GetVisibility())
  {
    count +=  this->SplineActor->RenderOverlay(viewport);
  }
  if (this->RadiusActor->GetVisibility())
  {
    count +=  this->RadiusActor->RenderOverlay(viewport);
  }
  if (this->TextActor->GetVisibility())
  {
    count +=  this->TextActor->RenderOverlay(viewport);
  }
  if (this->WorldCutActor->GetVisibility())
  {
    count +=  this->WorldCutActor->RenderOverlay(viewport);
  }
  if (this->SplineWorldCutActor->GetVisibility())
  {
    count +=  this->SplineWorldCutActor->RenderOverlay(viewport);
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
  if (this->ParametricMiddlePointActor->GetVisibility())
  {
    count += this->ParametricMiddlePointActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->ShapeActor->GetVisibility())
  {
    count += this->ShapeActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->SplineActor->GetVisibility())
  {
    count += this->SplineActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->RadiusActor->GetVisibility())
  {
    count += this->RadiusActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->TextActor->GetVisibility())
  {
    count += this->TextActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->WorldCutActor->GetVisibility())
  {
    count += this->WorldCutActor->RenderTranslucentPolygonalGeometry(viewport);
  }
  if (this->SplineWorldCutActor->GetVisibility())
  {
    count += this->SplineWorldCutActor->RenderTranslucentPolygonalGeometry(viewport);
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
  if (this->ParametricMiddlePointActor->GetVisibility() && this->ParametricMiddlePointActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }
  if (this->ShapeActor->GetVisibility() && this->ShapeActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }
  if (this->SplineActor->GetVisibility() && this->SplineActor->HasTranslucentPolygonalGeometry())
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
  if (this->WorldCutActor->GetVisibility() && this->WorldCutActor->HasTranslucentPolygonalGeometry())
  {
    return true;
  }
  if (this->SplineWorldCutActor->GetVisibility() && this->SplineWorldCutActor->HasTranslucentPolygonalGeometry())
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
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  
  this->ShapeActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
  this->WorldCutActor->SetVisibility(true);
  
  double closestPoint[3] = { 0.0 };
  double farthestPoint[3] = { 0.0 };
  double farthestDisplayPoint[3] = { 0.0 };
  double innerDisplayRadius = 0.0, outerDisplayRadius = 0.0;
  
  if (!shapeNode->DescribeDiskPointSpacing(closestPoint, farthestPoint,
                                            innerDisplayRadius, outerDisplayRadius))
  {
    return;
  }
  this->MiddlePointActor->SetVisibility(false);
  
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
        
  // Calculate normal relative to p1World
  double normalWorld[3] = { 0.0 };
  double rp2World[3] = { 0.0 };
  double rp3World[3] = { 0.0 };
  vtkMath::Subtract(p2World, p1World, rp2World);
  vtkMath::Subtract(p3World, p1World, rp3World);
  vtkMath::Cross(rp2World, rp3World, normalWorld);
        
  // Calculate inner and outer radii. Account for point proximities to center.
  double innerRadius = 0.0;
  double outerRadius = 0.0;
  if (closestPoint[0] == p2World[0] && closestPoint[1] == p2World[1] && closestPoint[2] == p2World[2])
  {
    innerRadius = std::sqrt(vtkMath::Distance2BetweenPoints(p1World, p2World));
    outerRadius = std::sqrt(vtkMath::Distance2BetweenPoints(p1World, p3World));
    farthestDisplayPoint[0] = p3[0];
    farthestDisplayPoint[1] = p3[1];
    farthestDisplayPoint[2] = p3[2];
  }
  else
  {
    innerRadius = std::sqrt(vtkMath::Distance2BetweenPoints(p1World, p3World));
    outerRadius = std::sqrt(vtkMath::Distance2BetweenPoints(p1World, p2World));
    farthestDisplayPoint[0] = p2[0];
    farthestDisplayPoint[1] = p2[1];
    farthestDisplayPoint[2] = p2[2];
  }
          
  this->DiskSource->SetCenter(p1World);
  this->DiskSource->SetNormal(normalWorld);
  this->DiskSource->SetOuterRadius(outerRadius);
  this->DiskSource->SetInnerRadius(innerRadius);
  // Show projections on demand.
  this->WorldCutActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Intersection);
  this->ShapeActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);

  this->DiskSource->SetCircumferentialResolution((int) shapeNode->GetResolution());
  this->DiskSource->Update();
  
  // Update shape and map from world to slice.
  this->ShapeWorldToSliceTransformer->SetInputConnection(this->DiskSource->GetOutputPort());
  this->ShapeWorldToSliceTransformer->Update();
  this->ShapeMapper->SetInputConnection(this->ShapeWorldToSliceTransformer->GetOutputPort());
  this->ShapeMapper->Update();
  
  // Update intersection and map from world to slice.
  double origin[3] = { 0.0 };
  double normal[3] = { 0.0 };
  vtkMatrix4x4 * sliceToRAS = this->GetSliceNode()->GetSliceToRAS();
  for (int i = 0; i < 3; i++)
  {
    origin[i] = sliceToRAS->GetElement(i, 3);
    normal[i] = sliceToRAS->GetElement(i, 2);
  }
  // Cut the invisible 3D representation.
  this->WorldPlane->SetOrigin(origin);
  this->WorldPlane->SetNormal(normal);
  this->WorldCutter->SetInputConnection(this->DiskSource->GetOutputPort());
  this->WorldCutter->Update();
  // Transform to slice representation and show.
  this->ShapeCutWorldToSliceTransformer->SetInputConnection(this->WorldCutter->GetOutputPort());
  this->ShapeCutWorldToSliceTransformer->Update();
  this->WorldCutMapper->SetInputConnection(this->ShapeCutWorldToSliceTransformer->GetOutputPort());
  this->WorldCutMapper->Update();
  
  // Hide the disk actor if it doesn't intersect the current slice
  this->SliceDistance->Update();
  if (!Superclass::IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->ShapeActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->WorldCutActor->SetVisibility(false);
  }
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->WorldCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  this->TextActor->SetDisplayPosition(farthestDisplayPoint[0], farthestDisplayPoint[1]);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->ShapeProperty->SetOpacity(fillOpacity);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  this->WorldCutActor->SetProperty(this->ShapeProperty);
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::UpdateRingFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  
  this->MiddlePointActor->SetVisibility(true);
  this->ShapeActor->SetVisibility(true);
  this->RadiusActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
  this->WorldCutActor->SetVisibility(true);
  
  this->ShapeMapper->SetInputConnection(this->RingSource->GetOutputPort());
  
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
  
  // Normal relative to p1
  double normalWorld[3] = { 0.0 };
  double rp2World[3] = { 0.0 };
  double rp3World[3] = { 0.0 };
  vtkMath::Subtract(p2World, p1World, rp2World);
  vtkMath::Subtract(p3World, p1World, rp3World);
  vtkMath::Cross(rp2World, rp3World, normalWorld);
  
  double lineLengthWorld = std::sqrt(vtkMath::Distance2BetweenPoints(p1World, p2World)); 
        
  // Centered mode.
  if (shapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
  {       
    this->RingSource->SetCenter(p1World);
    this->RingSource->SetNormal(normalWorld);
    this->RingSource->SetOuterRadius(lineLengthWorld);
    this->RingSource->SetInnerRadius((lineLengthWorld - this->ViewScaleFactorMmPerPixel) );
    
    this->MiddlePointSource->SetCenter(p1[0], p1[1], 0.0);
    this->MiddlePointSource->Update();
    // The middle point's properties are distinct.
    this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
    
    this->RadiusSource->SetPoint1(p1);
  }
  // Circumferential mode : center is half way between p1 and p2.
  else
  {
    double radiusWorld = lineLengthWorld / 2.0;
    double centerWorld[3] = { (p1World[0] + p2World[0]) / 2.0,
                        (p1World[1] + p2World[1]) / 2.0,
                        (p1World[2] + p2World[2]) / 2.0 };
      
    this->RingSource->SetCenter(centerWorld);
    this->RingSource->SetNormal(normalWorld);
    this->RingSource->SetOuterRadius(radiusWorld);
    this->RingSource->SetInnerRadius(radiusWorld - this->ViewScaleFactorMmPerPixel);
    
    double middlePointPos[2] = { (p1[0] + p2[0]) / 2.0, (p1[1] + p2[1]) / 2.0 };
    this->MiddlePointSource->SetCenter(middlePointPos[0], middlePointPos[1], 0.0);
    this->MiddlePointSource->Update();
    this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
    
    this->RadiusSource->SetPoint1(middlePointPos);
  }
  // Showing the radius here is confusing in UI.
  this->RadiusActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
  // Show the projection. SliceViewCutActor is also visible, but will blend with the projection. 
  this->ShapeActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
  
  this->RingSource->SetCircumferentialResolution((int) shapeNode->GetResolution());
  this->RingSource->Update();
  
  // Update shape and map from world to slice.
  this->ShapeWorldToSliceTransformer->SetInputConnection(this->RingSource->GetOutputPort());
  this->ShapeWorldToSliceTransformer->Update();
  this->ShapeMapper->SetInputConnection(this->ShapeWorldToSliceTransformer->GetOutputPort());
  this->ShapeMapper->Update();
  
  // Update intersection and map from world to slice.
  double origin[3] = { 0.0 };
  double normal[3] = { 0.0 };
  vtkMatrix4x4 * sliceToRAS = this->GetSliceNode()->GetSliceToRAS();
  for (int i = 0; i < 3; i++)
  {
    origin[i] = sliceToRAS->GetElement(i, 3);
    normal[i] = sliceToRAS->GetElement(i, 2);
  }
  this->WorldPlane->SetOrigin(origin);
  this->WorldPlane->SetNormal(normal);
  this->WorldCutter->SetInputConnection(this->RingSource->GetOutputPort());
  this->WorldCutter->Update();
  this->ShapeCutWorldToSliceTransformer->SetInputConnection(this->WorldCutter->GetOutputPort());
  this->ShapeCutWorldToSliceTransformer->Update();
  this->WorldCutMapper->SetInputConnection(this->ShapeCutWorldToSliceTransformer->GetOutputPort());
  this->WorldCutMapper->Update();
  
  this->RadiusSource->SetPoint2(p2);
  this->RadiusSource->Update();
  this->TextActor->SetDisplayPosition(p3[0], p3[1]);

  // Hide actors if they don't intersect the current slice
  this->SliceDistance->Update();
  if (!Superclass::IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->MiddlePointActor->SetVisibility(false);
    this->ShapeActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->WorldCutActor->SetVisibility(false);
  }
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  this->WorldCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::UpdateSphereFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  vtkMRMLMarkupsShapeNode * shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  
  this->ShapeActor->SetVisibility(true);
  this->MiddlePointActor->SetVisibility(true);
  this->WorldCutActor->SetVisibility(true);
  this->RadiusActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
  
  this->ShapeMapper->SetInputConnection(this->SphereSource->GetOutputPort());
  this->WorldCutter->SetInputConnection(this->SphereSource->GetOutputPort());
  
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
    
  double lineLengthWorld = std::sqrt(vtkMath::Distance2BetweenPoints(p1World, p2World));
  
  // Centered mode.
  if (shapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
  { 
    this->SphereSource->SetCenter(p1World);
    this->SphereSource->SetRadius(lineLengthWorld);
    
    this->MiddlePointSource->SetCenter(p1[0], p1[1], 0.0);
    this->MiddlePointSource->Update();
    // The middle point's properties are distinct.
    this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
    
    this->RadiusSource->SetPoint1(p1);
  }
  // Circumferential mode : center is half way between p1 and p2.
  else
  {
    double radiusWorld = lineLengthWorld / 2.0;
    double centerWorld[3] = { (p1World[0] + p2World[0]) / 2.0,
                        (p1World[1] + p2World[1]) / 2.0,
                        (p1World[2] + p2World[2]) / 2.0 };
      
    this->SphereSource->SetCenter(centerWorld);
    this->SphereSource->SetRadius(radiusWorld);
    
    double middlePointPos[2] = { (p1[0] + p2[0]) / 2.0, (p1[1] + p2[1]) / 2.0 };
    this->MiddlePointSource->SetCenter(middlePointPos[0], middlePointPos[1], 0.0);
    this->MiddlePointSource->Update();
    this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
    
    this->RadiusSource->SetPoint1(middlePointPos);
  }
  this->ShapeActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
  this->RadiusActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Intersection);
  this->WorldCutActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Intersection);
  
  this->SphereSource->SetPhiResolution(shapeNode->GetResolution());
  this->SphereSource->SetThetaResolution(shapeNode->GetResolution());
  this->SphereSource->Update();
  
  // Update shape and map from world to slice.
  this->ShapeWorldToSliceTransformer->SetInputConnection(this->SphereSource->GetOutputPort());
  this->ShapeWorldToSliceTransformer->Update();
  this->ShapeMapper->SetInputConnection(this->ShapeWorldToSliceTransformer->GetOutputPort());
  this->ShapeMapper->Update();
  
  // Update intersection and map from world to slice.
  double origin[3] = { 0.0 };
  double normal[3] = { 0.0 };
  vtkMatrix4x4 * sliceToRAS = this->GetSliceNode()->GetSliceToRAS();
  for (int i = 0; i < 3; i++)
  {
    origin[i] = sliceToRAS->GetElement(i, 3);
    normal[i] = sliceToRAS->GetElement(i, 2);
  }
  this->WorldPlane->SetOrigin(origin);
  this->WorldPlane->SetNormal(normal);
  this->WorldCutter->SetInputConnection(this->SphereSource->GetOutputPort());
  this->WorldCutter->Update();
  this->ShapeCutWorldToSliceTransformer->SetInputConnection(this->WorldCutter->GetOutputPort());
  this->ShapeCutWorldToSliceTransformer->Update();
  this->WorldCutMapper->SetInputConnection(this->ShapeCutWorldToSliceTransformer->GetOutputPort());
  this->WorldCutMapper->Update();
  
  this->RadiusSource->SetPoint2(p2);
  this->RadiusSource->Update();
  this->TextActor->SetDisplayPosition(p2[0], p2[1]);

  // Hide actors if they don't intersect the current slice
  this->SliceDistance->Update();
  if (!this->IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->ShapeActor->SetVisibility(false);
    this->MiddlePointActor->SetVisibility(false);
    this->WorldCutActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
  }
  
  this->MiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->WorldCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->RadiusActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->ShapeProperty->SetOpacity(fillOpacity);
  this->ShapeActor->SetProperty(this->ShapeProperty); // Doesnt't work.
  this->WorldCutActor->SetProperty(this->ShapeProperty);
  this->RadiusActor->SetProperty(this->ShapeProperty);
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::UpdateTubeFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  this->MiddlePointActor->SetVisibility(false);
  this->RadiusActor->SetVisibility(false);

  vtkMRMLMarkupsShapeNode* shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  
  if (!shapeNode || shapeNode->GetNumberOfControlPoints() < 4
    || shapeNode->GetNumberOfUndefinedControlPoints() > 0)
  {
    return;
  }

  if (!shapeNode->GetDisplayCappedTube())
  {
    this->ShapeMapper->SetInputConnection(this->Tube->GetOutputPort());
    this->WorldCutter->SetInputConnection(this->Tube->GetOutputPort());
  }
  else
  {
    this->ShapeMapper->SetInputConnection(this->CappedTube->GetOutputPort());
    this->WorldCutter->SetInputConnection(this->CappedTube->GetOutputPort());
  }
  
  int numberOfPairedControlPoints = (shapeNode->GetNumberOfControlPoints() % 2)
                            ? shapeNode->GetNumberOfControlPoints() - 1
                            : shapeNode->GetNumberOfControlPoints();
  
  vtkNew<vtkPoints> splinePoints;
  vtkNew<vtkTupleInterpolator> interpolatedRadius;
  interpolatedRadius->SetInterpolationTypeToLinear();
  interpolatedRadius->SetNumberOfComponents(1);
  int interpolatorIndex = 0;
  for (int i = 0; i < numberOfPairedControlPoints; i = i + 2)
  {
    double middlePoint[3] = { 0.0 };
    double p1[3] = { 0.0 };
    double p2[3] = { 0.0 };
    shapeNode->GetNthControlPointPositionWorld(i, p1);
    shapeNode->GetNthControlPointPositionWorld(i + 1, p2);
    middlePoint[0] = (p1[0] + p2[0]) / 2.0;
    middlePoint[1] = (p1[1] + p2[1]) / 2.0;
    middlePoint[2] = (p1[2] + p2[2]) / 2.0;
    splinePoints->InsertNextPoint(middlePoint);
    
    double radius = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2)) / 2.0;
    interpolatedRadius->AddTuple(interpolatorIndex, &radius);
    //interpolatedRadius->AddTuple(i + 1, &radius);
    interpolatorIndex++;
  }
  int numberOfPoints = splinePoints->GetNumberOfPoints();
  
  this->Spline->SetPoints(splinePoints);
  this->SplineFunctionSource->SetUResolution(100 * numberOfPoints);
  this->SplineFunctionSource->SetVResolution(100 * numberOfPoints);
  this->SplineFunctionSource->SetWResolution(100 * numberOfPoints);
  this->SplineFunctionSource->Update();
  vtkPolyData * splinePolyData = this->SplineFunctionSource->GetOutput();
  numberOfPoints = splinePolyData->GetNumberOfPoints();
  
  // https://kitware.github.io/vtk-examples/site/Cxx/VisualizationAlgorithms/TubesFromSplines/
  vtkSmartPointer<vtkDoubleArray> tubeRadius = vtkSmartPointer<vtkDoubleArray>::New();
  tubeRadius->SetNumberOfTuples(numberOfPoints);
  tubeRadius->SetName("TubeRadius");
  double tMin = interpolatedRadius->GetMinimumT();
  double tMax = interpolatedRadius->GetMaximumT();
  double r;
  for (unsigned int i = 0; i < numberOfPoints; ++i)
  {
    double t = (tMax - tMin) / (numberOfPoints - 1) * i + tMin;
    interpolatedRadius->InterpolateTuple(t, &r);
    tubeRadius->SetTuple1(i, r);
  }
  
  splinePolyData->GetPointData()->AddArray(tubeRadius);
  splinePolyData->GetPointData()->SetActiveScalars("TubeRadius");
  
  if (!shapeNode->GetDisplayCappedTube())
  {
    this->Tube->SetNumberOfSides(shapeNode->GetResolution());
    this->Tube->Update();
  }
  else
  {
    this->CappedTube->SetNumberOfSides(shapeNode->GetResolution());
    this->CappedTube->Update();
  }
  
  this->ShapeActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
  this->WorldCutActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Intersection);
  this->SplineActor->SetVisibility(shapeNode->GetSplineVisibility()
                            && shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
  this->SplineWorldCutActor->SetVisibility(shapeNode->GetSplineVisibility()
                            && shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Intersection);
  
  // Update shape and map from world to slice.
  if (!shapeNode->GetDisplayCappedTube())
  {
    this->ShapeWorldToSliceTransformer->SetInputConnection(this->Tube->GetOutputPort());
  }
  else
  {
    this->ShapeWorldToSliceTransformer->SetInputConnection(this->CappedTube->GetOutputPort());
  }
  this->ShapeWorldToSliceTransformer->Update();
  this->ShapeMapper->SetInputConnection(this->ShapeWorldToSliceTransformer->GetOutputPort());
  this->ShapeMapper->Update();

  this->SplineWorldToSliceTransformer->SetInputConnection(this->SplineFunctionSource->GetOutputPort());
  this->SplineWorldToSliceTransformer->Update();
  this->SplineMapper->SetInputConnection(this->SplineWorldToSliceTransformer->GetOutputPort());
  this->SplineMapper->Update();

  // Update intersection and map from world to slice.
  double origin[3] = { 0.0 };
  double normal[3] = { 0.0 };
  vtkMatrix4x4 * sliceToRAS = this->GetSliceNode()->GetSliceToRAS();
  for (int i = 0; i < 3; i++)
  {
    origin[i] = sliceToRAS->GetElement(i, 3);
    normal[i] = sliceToRAS->GetElement(i, 2);
  }
  this->WorldPlane->SetOrigin(origin);
  this->WorldPlane->SetNormal(normal);
  this->WorldCutter->Update();
  this->ShapeCutWorldToSliceTransformer->SetInputConnection(this->WorldCutter->GetOutputPort());
  this->ShapeCutWorldToSliceTransformer->Update();
  this->WorldCutMapper->SetInputConnection(this->ShapeCutWorldToSliceTransformer->GetOutputPort());
  this->WorldCutMapper->Update();

  this->SplineWorldCutter->SetInputConnection(this->SplineFunctionSource->GetOutputPort());
  this->SplineWorldCutter->Update();
  this->SplineCutWorldToSliceTransformer->SetInputConnection(this->SplineWorldCutter->GetOutputPort());
  this->SplineCutWorldToSliceTransformer->Update();
  this->SplineWorldCutMapper->SetInputConnection(this->SplineCutWorldToSliceTransformer->GetOutputPort());
  this->SplineWorldCutMapper->Update();
  
  double p1[3] = { 0.0 };
  this->GetNthControlPointDisplayPosition(0, p1);
  this->TextActor->SetDisplayPosition(p1[0], p1[1]);
  this->TextActor->SetVisibility(true);
  
  // Hide actors if they don't intersect the current slice
  this->SliceDistance->Update();
  if (!this->IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->ShapeActor->SetVisibility(false);
    this->WorldCutActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->SplineActor->SetVisibility(false);
    this->SplineWorldCutActor->SetVisibility(false);
  }
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->WorldCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->SplineActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->SplineWorldCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  
  double opacity = this->MarkupsDisplayNode->GetOpacity();
  double fillOpacity = opacity * this->MarkupsDisplayNode->GetFillOpacity();
  this->ShapeProperty->DeepCopy(this->GetControlPointsPipeline(controlPointType)->Property);
  this->ShapeProperty->SetOpacity(fillOpacity);
  this->ShapeActor->SetProperty(this->ShapeProperty);
  this->WorldCutActor->SetProperty(this->ShapeProperty);
  this->SplineActor->SetProperty(this->ShapeProperty);
  this->SplineWorldCutActor->SetProperty(this->ShapeProperty);
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::UpdateConeFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  vtkMRMLMarkupsShapeNode* shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }

  this->ShapeActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
  this->WorldCutActor->SetVisibility(true);
  
  this->ShapeMapper->SetInputConnection(this->ConeSource->GetOutputPort());
  
  double p1World[3] = { 0.0 };
  double p2World[3] = { 0.0 };
  double p3World[3] = { 0.0 };
  shapeNode->GetNthControlPointPositionWorld(0, p1World);
  shapeNode->GetNthControlPointPositionWorld(1, p2World);
  shapeNode->GetNthControlPointPositionWorld(2, p3World);
  
  this->ShapeMapper->SetInputConnection(this->ConeSource->GetOutputPort());
  
  double height = std::sqrt(vtkMath::Distance2BetweenPoints(p1World, p3World));
  double directionWorld[3] = { 0.0 };
  // Points towards p3
  vtkMath::Subtract(p3World, p1World, directionWorld);
  
  double centerWorld[3] = {0.0};
  centerWorld[0] = (p1World[0] + p3World[0]) / 2.0;
  centerWorld[1] = (p1World[1] + p3World[1]) / 2.0;
  centerWorld[2] = (p1World[2] + p3World[2]) / 2.0;
  
  double radius = std::sqrt(vtkMath::Distance2BetweenPoints(p1World, p2World));
  
  this->ConeSource->SetCenter(centerWorld);
  this->ConeSource->SetRadius(radius);
  this->ConeSource->SetHeight(height);
  this->ConeSource->SetDirection(directionWorld);
  this->ConeSource->SetResolution(shapeNode->GetResolution());
  this->ConeSource->Update();
  
  // Update shape and map from world to slice.
  this->ShapeWorldToSliceTransformer->SetInputConnection(this->ConeSource->GetOutputPort());
  this->ShapeWorldToSliceTransformer->Update();
  this->ShapeMapper->SetInputConnection(this->ShapeWorldToSliceTransformer->GetOutputPort());
  this->ShapeMapper->Update();
  
  // Update intersection and map from world to slice.
  double origin[3] = { 0.0 };
  double normal[3] = { 0.0 };
  vtkMatrix4x4 * sliceToRAS = this->GetSliceNode()->GetSliceToRAS();
  for (int i = 0; i < 3; i++)
  {
    origin[i] = sliceToRAS->GetElement(i, 3);
    normal[i] = sliceToRAS->GetElement(i, 2);
  }
  this->WorldPlane->SetOrigin(origin);
  this->WorldPlane->SetNormal(normal);
  this->WorldCutter->SetInputConnection(this->ConeSource->GetOutputPort());
  this->WorldCutter->Update();
  this->ShapeCutWorldToSliceTransformer->SetInputConnection(this->WorldCutter->GetOutputPort());
  this->ShapeCutWorldToSliceTransformer->Update();
  this->WorldCutMapper->SetInputConnection(this->ShapeCutWorldToSliceTransformer->GetOutputPort());
  this->WorldCutMapper->Update();
  
  this->ShapeActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
  this->WorldCutActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Intersection);
  
  double p3Display[3] = { 0.0 };
  this->GetNthControlPointDisplayPosition(2, p3Display);
  this->TextActor->SetDisplayPosition(p3Display[0], p3Display[1]);

  // Hide actors if they don't intersect the current slice
  this->SliceDistance->Update();
  if (!Superclass::IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->MiddlePointActor->SetVisibility(false);
    this->ShapeActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->WorldCutActor->SetVisibility(false);
  }
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  this->WorldCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::UpdateCylinderFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  vtkMRMLMarkupsShapeNode* shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  
  this->ShapeActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
  this->WorldCutActor->SetVisibility(true);
  
  this->ShapeMapper->SetInputConnection(this->CylinderSource->GetOutputPort());
  
  double p1World[3] = { 0.0 };
  double p2World[3] = { 0.0 };
  double p3World[3] = { 0.0 };
  shapeNode->GetNthControlPointPositionWorld(0, p1World);
  shapeNode->GetNthControlPointPositionWorld(1, p2World);
  shapeNode->GetNthControlPointPositionWorld(2, p3World);
  
  this->CylinderAxis->SetPoint1(p1World);
  this->CylinderAxis->SetPoint2(p3World);
  this->CylinderAxis->Update();
  double radius = std::sqrt(vtkMath::Distance2BetweenPoints(p1World, p2World));
  
  this->CylinderSource->SetRadius(radius);
  this->CylinderSource->SetNumberOfSides(shapeNode->GetResolution());
  this->CylinderSource->Update();
  
  // Update shape and map from world to slice.
  this->ShapeWorldToSliceTransformer->SetInputConnection(this->CylinderSource->GetOutputPort());
  this->ShapeWorldToSliceTransformer->Update();
  this->ShapeMapper->SetInputConnection(this->ShapeWorldToSliceTransformer->GetOutputPort());
  this->ShapeMapper->Update();
  
  // Update intersection and map from world to slice.
  double origin[3] = { 0.0 };
  double normal[3] = { 0.0 };
  vtkMatrix4x4 * sliceToRAS = this->GetSliceNode()->GetSliceToRAS();
  for (int i = 0; i < 3; i++)
  {
    origin[i] = sliceToRAS->GetElement(i, 3);
    normal[i] = sliceToRAS->GetElement(i, 2);
  }
  this->WorldPlane->SetOrigin(origin);
  this->WorldPlane->SetNormal(normal);
  this->WorldCutter->SetInputConnection(this->CylinderSource->GetOutputPort());
  this->WorldCutter->Update();
  this->ShapeCutWorldToSliceTransformer->SetInputConnection(this->WorldCutter->GetOutputPort());
  this->ShapeCutWorldToSliceTransformer->Update();
  this->WorldCutMapper->SetInputConnection(this->ShapeCutWorldToSliceTransformer->GetOutputPort());
  this->WorldCutMapper->Update();
  
  this->ShapeActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
  this->WorldCutActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Intersection);
  
  double p3Display[3] = { 0.0 };
  this->GetNthControlPointDisplayPosition(2, p3Display);
  this->TextActor->SetDisplayPosition(p3Display[0], p3Display[1]);

  // Hide actors if they don't intersect the current slice
  this->SliceDistance->Update();
  if (!Superclass::IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->MiddlePointActor->SetVisibility(false);
    this->ShapeActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->WorldCutActor->SetVisibility(false);
  }
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  this->WorldCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::UpdateArcFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  /*è
   * If all 3 points are coplanar in a slice view, the arc is visible in
   * ::Projection mode only.
   */
  vtkMRMLMarkupsShapeNode* shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  
  this->ShapeActor->SetVisibility(true);
  this->TextActor->SetVisibility(true);
  this->WorldCutActor->SetVisibility(true);
  
  this->ShapeMapper->SetInputConnection(this->ArcSource->GetOutputPort());
  
  double p1World[3] = { 0.0 };
  double p2World[3] = { 0.0 };
  double p3World[3] = { 0.0 };
  shapeNode->GetNthControlPointPositionWorld(0, p1World);
  shapeNode->GetNthControlPointPositionWorld(1, p2World);
  shapeNode->GetNthControlPointPositionWorld(2, p3World);
  
  this->ShapeMapper->SetInputConnection(this->ArcSource->GetOutputPort());
  
  double polarVector1World[3] = { 0.0 };
  double polarVector2World[3] = { 0.0 }; // Not really, but will be when repositioned.
  double normal[3] = { 0.0 }; // Normal to the plane.
  vtkMath::Subtract(p2World, p1World, polarVector1World);
  vtkMath::Subtract(p3World, p1World, polarVector2World);
  vtkMath::Cross(polarVector1World, polarVector2World, normal);
  const double angle = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(polarVector1World, polarVector2World));
  
  this->ArcSource->SetCenter(p1World);
  this->ArcSource->SetPolarVector(polarVector1World);
  this->ArcSource->SetNormal(normal);
  this->ArcSource->SetAngle(angle);
  this->ArcSource->SetResolution(shapeNode->GetResolution());
  this->ArcSource->Update();
  
  // Update shape and map from world to slice.
  this->ShapeWorldToSliceTransformer->SetInputConnection(this->ArcSource->GetOutputPort());
  this->ShapeWorldToSliceTransformer->Update();
  this->ShapeMapper->SetInputConnection(this->ShapeWorldToSliceTransformer->GetOutputPort());
  this->ShapeMapper->Update();
  
  // Update intersection and map from world to slice.
  double origin[3] = { 0.0 };
  vtkMatrix4x4 * sliceToRAS = this->GetSliceNode()->GetSliceToRAS();
  for (int i = 0; i < 3; i++)
  {
    origin[i] = sliceToRAS->GetElement(i, 3);
    normal[i] = sliceToRAS->GetElement(i, 2);
  }
  this->WorldPlane->SetOrigin(origin);
  this->WorldPlane->SetNormal(normal);
  this->WorldCutter->SetInputConnection(this->ArcSource->GetOutputPort());
  this->WorldCutter->Update();
  this->ShapeCutWorldToSliceTransformer->SetInputConnection(this->WorldCutter->GetOutputPort());
  this->ShapeCutWorldToSliceTransformer->Update();
  this->WorldCutMapper->SetInputConnection(this->ShapeCutWorldToSliceTransformer->GetOutputPort());
  this->WorldCutMapper->Update();
  
  this->ShapeActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
  this->WorldCutActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Intersection);
  
  double p1[3] = { 0.0 };
  this->GetNthControlPointDisplayPosition(0, p1);
  this->TextActor->SetDisplayPosition(p1[0], p1[1]);

  // Hide actors if they don't intersect the current slice
  this->SliceDistance->Update();
  if (!Superclass::IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->MiddlePointActor->SetVisibility(false);
    this->ShapeActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->WorldCutActor->SetVisibility(false);
  }
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  this->WorldCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
}

//-----------------------------------------------------------------------------
void vtkSlicerShapeRepresentation2D::UpdateParametricFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData)
{
  // See notes in vtkSlicerShapeRepresentation3D.cxx.
  vtkMRMLMarkupsShapeNode* shapeNode = vtkMRMLMarkupsShapeNode::SafeDownCast(this->GetMarkupsNode());
  if (!shapeNode || shapeNode->GetNumberOfDefinedControlPoints(true) != shapeNode->GetRequiredNumberOfControlPoints())
  {
    return;
  }
  
  switch (shapeNode->GetShapeName())
  {
    case vtkMRMLMarkupsShapeNode::Ellipsoid:
      this->ParametricFunctionSource->SetParametricFunction(this->Ellipsoid);
      break;
    case vtkMRMLMarkupsShapeNode::Toroid:
      this->ParametricFunctionSource->SetParametricFunction(this->Toroid);
      break;
    case vtkMRMLMarkupsShapeNode::BohemianDome:
      this->ParametricFunctionSource->SetParametricFunction(this->BohemianDome);
      break;
    case vtkMRMLMarkupsShapeNode::Bour:
      this->ParametricFunctionSource->SetParametricFunction(this->Bour);
      break;
    case vtkMRMLMarkupsShapeNode::Boy:
      this->ParametricFunctionSource->SetParametricFunction(this->Boy);
      break;
    case vtkMRMLMarkupsShapeNode::CrossCap:
      this->ParametricFunctionSource->SetParametricFunction(this->CrossCap);
      break;
    case vtkMRMLMarkupsShapeNode::ConicSpiral:
      this->ParametricFunctionSource->SetParametricFunction(this->ConicSpiral);
      break;
    case vtkMRMLMarkupsShapeNode::Kuen:
      this->ParametricFunctionSource->SetParametricFunction(this->Kuen);
      break;
    case vtkMRMLMarkupsShapeNode::Mobius:
      this->ParametricFunctionSource->SetParametricFunction(this->Mobius);
      break;
    case vtkMRMLMarkupsShapeNode::PluckerConoid:
      this->ParametricFunctionSource->SetParametricFunction(this->PluckerConoid);
      break;
    case vtkMRMLMarkupsShapeNode::Roman:
      this->ParametricFunctionSource->SetParametricFunction(this->Roman);
      break;
    default:
      vtkErrorMacro("Unfit shape.");
      return;
  }
  
  double p1World[3] = { 0.0 };
  double p2World[3] = { 0.0 };
  double p3World[3] = { 0.0 };
  double p4World[3] = { 0.0 };
  double directionWorld[3] = { 0.0 };
  double centerWorld[3] = { 0.0 };
  shapeNode->GetNthControlPointPositionWorld(0, p1World);
  shapeNode->GetNthControlPointPositionWorld(1, p2World);
  shapeNode->GetNthControlPointPositionWorld(2, p3World);
  shapeNode->GetNthControlPointPositionWorld(3, p4World);
  
  double center[3] = { 0.0 };
  double p1[3] = { 0.0 };
  double p4[3] = { 0.0 };
  this->GetNthControlPointDisplayPosition(0, p1);
  this->GetNthControlPointDisplayPosition(3, p4);
  
  if (shapeNode->GetRadiusMode() == vtkMRMLMarkupsShapeNode::Centered)
  {
    vtkMath::Assign(p1World, centerWorld);
    vtkMath::Assign(p1, center);
    this->ParametricMiddlePointActor->SetVisibility(false);
  }
  else{
    centerWorld[0] = (p1World[0] + p4World[0]) / 2.0;
    centerWorld[1] = (p1World[1] + p4World[1]) / 2.0;
    centerWorld[2] = (p1World[2] + p4World[2]) / 2.0;
    
    center[0] = (p1[0] + p4[0]) / 2.0;
    center[1] = (p1[1] + p4[1]) / 2.0;
    center[2] = 0.0;
    
    this->ParametricMiddlePointSource->SetCenter(center);
    this->ParametricMiddlePointSource->Update();
    this->ParametricMiddlePointActor->SetVisibility(true);
    this->ParametricMiddlePointActor->SetProperty(this->GetControlPointsPipeline(Active)->Property);
  }
  vtkMath::Subtract(p4World, centerWorld, directionWorld);
  vtkMath::Normalize(directionWorld);
  
  double transformReferenceAxis[3] = {0.0, 0.0, 1.0};
  double transformRotationAxis[3] = { 0.0 };
  vtkMath::Cross(transformReferenceAxis, directionWorld, transformRotationAxis);
  double angleToTransformReferenceAxis = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(directionWorld, transformReferenceAxis));
  this->ParametricTransform->Identity();
  this->ParametricTransform->RotateWXYZ(angleToTransformReferenceAxis, transformRotationAxis);
  this->ParametricTransform->PostMultiply();
  this->ParametricTransform->Translate(centerWorld);
  this->ParametricTransform->PreMultiply();
  
  double xRadius = std::sqrt(vtkMath::Distance2BetweenPoints(centerWorld, p2World));
  double yRadius = std::sqrt(vtkMath::Distance2BetweenPoints(centerWorld, p3World));
  double zRadius = std::sqrt(vtkMath::Distance2BetweenPoints(centerWorld, p4World));
  
  this->ParametricTransformer->SetInputConnection(this->ParametricFunctionSource->GetOutputPort());
  this->ParametricFunctionSource->SetScalarMode(shapeNode->GetParametricScalarMode());
  this->ParametricTransformer->Update();
  
  switch (shapeNode->GetShapeName())
  {
    case vtkMRMLMarkupsShapeNode::Ellipsoid:
      this->Ellipsoid->SetZRadius(zRadius);
      this->Ellipsoid->SetYRadius(yRadius);
      this->Ellipsoid->SetXRadius(xRadius);
      this->Ellipsoid->SetN1(shapeNode->GetParametricN1());
      this->Ellipsoid->SetN2(shapeNode->GetParametricN2());
      break;
    case vtkMRMLMarkupsShapeNode::Toroid:
      this->Toroid->SetZRadius(zRadius);
      this->Toroid->SetYRadius(yRadius);
      this->Toroid->SetXRadius(xRadius);
      this->Toroid->SetN1(shapeNode->GetParametricN1());
      this->Toroid->SetN2(shapeNode->GetParametricN2());
      this->Toroid->SetRingRadius(shapeNode->GetParametricRingRadius());
      this->Toroid->SetCrossSectionRadius(shapeNode->GetParametricCrossSectionRadius());
      break;
    case vtkMRMLMarkupsShapeNode::BohemianDome:
      this->BohemianDome->SetA(xRadius);
      this->BohemianDome->SetB(yRadius);
      this->BohemianDome->SetC(zRadius);
      break;
    case vtkMRMLMarkupsShapeNode::ConicSpiral:
      this->ConicSpiral->SetA(xRadius);
      this->ConicSpiral->SetB(zRadius); // yes, to be visually consistent.
      this->ConicSpiral->SetC(yRadius);
      this->ConicSpiral->SetN(shapeNode->GetParametricN());
      break;
    case vtkMRMLMarkupsShapeNode::PluckerConoid:
      this->PluckerConoid->SetN((int) shapeNode->GetParametricN());
      this->ParametricTransform->Scale(xRadius, yRadius, zRadius);
      break;
    case vtkMRMLMarkupsShapeNode::Roman:
      this->Roman->SetRadius(shapeNode->GetParametricRadius());
      this->ParametricTransform->Scale(xRadius, yRadius, zRadius);
      break;
    case vtkMRMLMarkupsShapeNode::Mobius:
      this->Mobius->SetRadius(shapeNode->GetParametricRadius());
      this->ParametricTransform->Scale(xRadius, yRadius, zRadius);
      break;
    case vtkMRMLMarkupsShapeNode::Kuen:
    case vtkMRMLMarkupsShapeNode::CrossCap:
    case vtkMRMLMarkupsShapeNode::Boy:
    case vtkMRMLMarkupsShapeNode::Bour:
      // This geometry and many others do not have their own resizing parameters.
      this->ParametricTransform->Scale(xRadius, yRadius, zRadius);
      break;
    default:
      vtkErrorMacro("Unfit shape.");
      return;
  }
  this->ParametricFunctionSource->SetUResolution(shapeNode->GetResolution());
  this->ParametricFunctionSource->SetVResolution(shapeNode->GetResolution());
  this->ParametricFunctionSource->SetWResolution(shapeNode->GetResolution());
  
  vtkParametricFunction * function = this->ParametricFunctionSource->GetParametricFunction();
  function->SetMinimumU(shapeNode->GetParametricMinimumU());
  function->SetMaximumU(shapeNode->GetParametricMaximumU());
  function->SetMinimumV(shapeNode->GetParametricMinimumV());
  function->SetMaximumV(shapeNode->GetParametricMaximumV());
  function->SetMinimumW(shapeNode->GetParametricMinimumW());
  function->SetMaximumW(shapeNode->GetParametricMaximumW());
  function->SetJoinU(shapeNode->GetParametricJoinU());
  function->SetJoinV(shapeNode->GetParametricJoinV());
  function->SetJoinW(shapeNode->GetParametricJoinW());
  function->SetTwistU(shapeNode->GetParametricTwistU());
  function->SetTwistV(shapeNode->GetParametricTwistV());
  function->SetTwistW(shapeNode->GetParametricTwistW());
  function->SetClockwiseOrdering(shapeNode->GetParametricClockwiseOrdering());
  this->ParametricFunctionSource->Update();
  
  // Update shape and map from world to slice.
  this->ShapeWorldToSliceTransformer->SetInputConnection(this->ParametricTransformer->GetOutputPort());
  this->ShapeWorldToSliceTransformer->Update();
  this->ShapeMapper->SetInputConnection(this->ShapeWorldToSliceTransformer->GetOutputPort());
  this->ShapeMapper->Update();
  
  // Update intersection and map from world to slice.
  double origin[3] = { 0.0 };
  double normal[3] = { 0.0 };
  vtkMatrix4x4 * sliceToRAS = this->GetSliceNode()->GetSliceToRAS();
  for (int i = 0; i < 3; i++)
  {
    origin[i] = sliceToRAS->GetElement(i, 3);
    normal[i] = sliceToRAS->GetElement(i, 2);
  }
  this->WorldPlane->SetOrigin(origin);
  this->WorldPlane->SetNormal(normal);
  this->WorldCutter->SetInputConnection(this->ParametricTransformer->GetOutputPort());
  this->WorldCutter->Update();
  this->ShapeCutWorldToSliceTransformer->SetInputConnection(this->WorldCutter->GetOutputPort());
  this->ShapeCutWorldToSliceTransformer->Update();
  this->WorldCutMapper->SetInputConnection(this->ShapeCutWorldToSliceTransformer->GetOutputPort());
  this->WorldCutMapper->Update();
  
  this->ShapeActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Projection);
  this->WorldCutActor->SetVisibility(shapeNode->GetDrawMode2D() == vtkMRMLMarkupsShapeNode::Intersection);
  
  this->TextActor->SetDisplayPosition(center[0], center[1]);
  this->TextActor->SetVisibility(true);
  
  // Hide actors if they don't intersect the current slice
  this->SliceDistance->Update();
  if (!Superclass::IsRepresentationIntersectingSlice(vtkPolyData::SafeDownCast(this->SliceDistance->GetOutput()), this->SliceDistance->GetScalarArrayName()))
  {
    this->MiddlePointActor->SetVisibility(false);
    this->ParametricMiddlePointActor->SetVisibility(false);
    this->ShapeActor->SetVisibility(false);
    this->RadiusActor->SetVisibility(false);
    this->TextActor->SetVisibility(false);
    this->WorldCutActor->SetVisibility(false);
  }
  
  int controlPointType = this->GetAllControlPointsSelected() ? Selected : Unselected;
  this->ShapeActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
  this->TextActor->SetTextProperty(this->GetControlPointsPipeline(controlPointType)->TextProperty);
  this->WorldCutActor->SetProperty(this->GetControlPointsPipeline(controlPointType)->Property);
}
