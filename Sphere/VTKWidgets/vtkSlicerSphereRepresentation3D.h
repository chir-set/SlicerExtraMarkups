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

#ifndef __vtkslicerSphere_LOWERrepresentation3d_h_
#define __vtkslicerSphere_LOWERrepresentation3d_h_

#include "vtkSlicerSphereModuleVTKWidgetsExport.h"

// Markups VTKWidgets includes
#include "vtkSlicerMarkupsWidgetRepresentation3D.h"

// VTK includes
#include <vtkWeakPointer.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>

/**
 * @class   vtkSlicerSphereRepresentation3D
 * @brief   Default representation for the line widget
 *
 * This class provides the default concrete representation for the
 * vtkMRMLAbstractWidget. See vtkMRMLAbstractWidget
 * for details.
 * @sa
 * vtkSlicerMarkupsWidgetRepresentation2D vtkMRMLAbstractWidget
*/

class VTK_SLICER_SPHERE_MODULE_VTKWIDGETS_EXPORT vtkSlicerSphereRepresentation3D
: public vtkSlicerMarkupsWidgetRepresentation3D
{
public:
  static vtkSlicerSphereRepresentation3D* New();
  vtkTypeMacro(vtkSlicerSphereRepresentation3D, vtkSlicerMarkupsWidgetRepresentation3D);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr) override;

  /// Methods to make this class behave as a vtkProp.
  void GetActors(vtkPropCollection*) override;
  void ReleaseGraphicsResources(vtkWindow*) override;
  int RenderOverlay(vtkViewport* viewport) override;
  int RenderOpaqueGeometry(vtkViewport* viewport) override;
  int RenderTranslucentPolygonalGeometry(vtkViewport* viewport) override;
  vtkTypeBool HasTranslucentPolygonalGeometry() override;
  
  double * GetBounds() override;

protected:
  vtkSlicerSphereRepresentation3D();
  ~vtkSlicerSphereRepresentation3D() override;

  vtkSmartPointer<vtkPolyData> MiddlePoint;
  vtkSmartPointer<vtkPolyDataMapper> MiddlePointMapper;
  vtkSmartPointer<vtkActor> MiddlePointActor;
  vtkSmartPointer<vtkSphereSource> MiddlePointSource;
  vtkSmartPointer<vtkPlane> SlicingPlane;
  
  vtkSmartPointer<vtkSphereSource> SphereSource;
  vtkSmartPointer<vtkPolyDataMapper> SphereMapper;
  vtkSmartPointer<vtkActor> SphereActor;
  vtkSmartPointer<vtkProperty> SphereProperty;
  
  vtkSmartPointer<vtkLineSource> RadiusSource;
  vtkSmartPointer<vtkPolyDataMapper> RadiusMapper;
  vtkSmartPointer<vtkActor> RadiusActor;

private:
  vtkSlicerSphereRepresentation3D(const vtkSlicerSphereRepresentation3D&) = delete;
  void operator=(const vtkSlicerSphereRepresentation3D&) = delete;
};

#endif // __vtkslicerSphere_LOWERrepresentation3d_h_
