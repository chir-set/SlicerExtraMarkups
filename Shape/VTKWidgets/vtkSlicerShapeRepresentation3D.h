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

#ifndef __vtkslicerShaperepresentation3d_h_
#define __vtkslicerShaperepresentation3d_h_

#include "vtkSlicerShapeModuleVTKWidgetsExport.h"

// Markups VTKWidgets includes
#include "vtkSlicerMarkupsWidgetRepresentation3D.h"

// VTK includes
#include <vtkWeakPointer.h>
#include <vtkDiskSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkTubeFilter.h>
#include <vtkParametricSpline.h>
#include <vtkParametricSuperEllipsoid.h>
#include <vtkParametricSuperToroid.h>
#include <vtkParametricBohemianDome.h>
#include <vtkParametricBour.h>
#include <vtkParametricBoy.h>
#include <vtkParametricCrossCap.h>
#include <vtkParametricConicSpiral.h>
#include <vtkParametricKuen.h>
#include <vtkParametricMobius.h>
#include <vtkParametricPluckerConoid.h>
#include <vtkParametricRoman.h>
#include <vtkParametricFunctionSource.h>
#include <vtkConeSource.h>
#include <vtkArcSource.h>
#include <vtkMRMLScene.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

//------------------------------------------------------------------------------
class vtkCutter;
class vtkPlane;

/**
 * @class   vtkSlicerShapeRepresentation3D
 * @brief   Default representation for the line widget
 *
 * This class provides the default concrete representation for the
 * vtkMRMLAbstractWidget. See vtkMRMLAbstractWidget
 * for details.
 * @sa
 * vtkSlicerMarkupsWidgetRepresentation2D vtkMRMLAbstractWidget
*/

class VTK_SLICER_SHAPE_MODULE_VTKWIDGETS_EXPORT vtkSlicerShapeRepresentation3D
: public vtkSlicerMarkupsWidgetRepresentation3D
{
public:
  static vtkSlicerShapeRepresentation3D* New();
  vtkTypeMacro(vtkSlicerShapeRepresentation3D, vtkSlicerMarkupsWidgetRepresentation3D);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr) override;

  /// Methods to make this class behave as a vtkProp.
  void GetActors(vtkPropCollection*) override;
  void ReleaseGraphicsResources(vtkWindow*) override;
  int RenderOverlay(vtkViewport* viewport) override;
  int RenderOpaqueGeometry(vtkViewport* viewport) override;
  int RenderTranslucentPolygonalGeometry(vtkViewport* viewport) override;
  vtkTypeBool HasTranslucentPolygonalGeometry() override;

protected:
  vtkSlicerShapeRepresentation3D();
  ~vtkSlicerShapeRepresentation3D() override;

  vtkSmartPointer<vtkPolyDataMapper> MiddlePointMapper;
  vtkSmartPointer<vtkActor> MiddlePointActor;
  vtkSmartPointer<vtkSphereSource> MiddlePointSource;
  void BuildMiddlePoint();
  
  vtkSmartPointer<vtkPolyDataMapper> ParametricMiddlePointMapper;
  vtkSmartPointer<vtkActor> ParametricMiddlePointActor;
  vtkSmartPointer<vtkSphereSource> ParametricMiddlePointSource;
  
  vtkSmartPointer<vtkDiskSource> DiskSource;
  vtkSmartPointer<vtkDiskSource> RingSource;
  vtkSmartPointer<vtkSphereSource> SphereSource;
  vtkSmartPointer<vtkConeSource> ConeSource;
  vtkSmartPointer<vtkTubeFilter> CylinderSource; // Regular tube.
  vtkSmartPointer<vtkLineSource> CylinderAxis;
  vtkSmartPointer<vtkArcSource> ArcSource;
  
  vtkSmartPointer<vtkLineSource> RadiusSource;
  vtkSmartPointer<vtkPolyDataMapper> RadiusMapper;
  vtkSmartPointer<vtkActor> RadiusActor;
  
  vtkSmartPointer<vtkParametricSpline> Spline;
  vtkSmartPointer<vtkParametricFunctionSource> SplineFunctionSource;
  vtkSmartPointer<vtkPolyDataMapper> SplineMapper;
  vtkSmartPointer<vtkActor> SplineActor;
  vtkSmartPointer<vtkTubeFilter> Tube; // Variable radius tube.
  vtkSmartPointer<vtkTubeFilter> CappedTube;
  
  vtkSmartPointer<vtkParametricSuperEllipsoid> Ellipsoid;
  vtkSmartPointer<vtkParametricSuperToroid> Toroid;
  vtkSmartPointer<vtkParametricBohemianDome> BohemianDome;
  vtkSmartPointer<vtkParametricBour> Bour;
  vtkSmartPointer<vtkParametricBoy> Boy;
  vtkSmartPointer<vtkParametricCrossCap> CrossCap;
  vtkSmartPointer<vtkParametricConicSpiral> ConicSpiral;
  vtkSmartPointer<vtkParametricKuen> Kuen;
  vtkSmartPointer<vtkParametricMobius> Mobius;
  vtkSmartPointer<vtkParametricPluckerConoid> PluckerConoid;
  vtkSmartPointer<vtkParametricRoman> Roman;
  vtkSmartPointer<vtkParametricFunctionSource> ParametricFunctionSource;
  vtkSmartPointer<vtkTransform> ParametricTransform;
  vtkSmartPointer<vtkTransformPolyDataFilter> ParametricTransformer;
  
  vtkSmartPointer<vtkPolyDataMapper> ShapeMapper;
  vtkSmartPointer<vtkActor> ShapeActor;
  vtkSmartPointer<vtkProperty> ShapeProperty;
  
  void UpdateDiskFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);
  void UpdateRingFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);
  void UpdateSphereFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);
  void UpdateTubeFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);
  void UpdateConeFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);
  void UpdateCylinderFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);
  void UpdateArcFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);
  void UpdateParametricFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);
  
  // Set shape, spline and closed tube pointers in markups node from the first view only.
  vtkObject * GetFirstViewNode(vtkMRMLScene * scene) const;

private:
  vtkSlicerShapeRepresentation3D(const vtkSlicerShapeRepresentation3D&) = delete;
  void operator=(const vtkSlicerShapeRepresentation3D&) = delete;
  
  // Ring, Cone, Cylinder : 1 point is moved during UpdateFromMRML, block recursion.
  bool DoUpdateFromMRML = true;
};

#endif // __vtkslicerShaperepresentation3d_h_
