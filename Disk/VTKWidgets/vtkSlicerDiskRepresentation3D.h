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

#ifndef __vtkslicerdiskrepresentation3d_h_
#define __vtkslicerdiskrepresentation3d_h_

#include "vtkSlicerDiskModuleVTKWidgetsExport.h"

// Markups VTKWidgets includes
#include "vtkSlicerMarkupsWidgetRepresentation3D.h"

// VTK includes
#include <vtkWeakPointer.h>
#include <vtkDiskSource.h>

//------------------------------------------------------------------------------
class vtkCutter;
class vtkPlane;

/**
 * @class   vtkSlicerDiskRepresentation3D
 * @brief   Default representation for the line widget
 *
 * This class provides the default concrete representation for the
 * vtkMRMLAbstractWidget. See vtkMRMLAbstractWidget
 * for details.
 * @sa
 * vtkSlicerMarkupsWidgetRepresentation2D vtkMRMLAbstractWidget
*/

class VTK_SLICER_DISK_MODULE_VTKWIDGETS_EXPORT vtkSlicerDiskRepresentation3D
: public vtkSlicerMarkupsWidgetRepresentation3D
{
public:
  static vtkSlicerDiskRepresentation3D* New();
  vtkTypeMacro(vtkSlicerDiskRepresentation3D, vtkSlicerMarkupsWidgetRepresentation3D);
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
  vtkSlicerDiskRepresentation3D();
  ~vtkSlicerDiskRepresentation3D() override;

private:
  vtkSlicerDiskRepresentation3D(const vtkSlicerDiskRepresentation3D&) = delete;
  void operator=(const vtkSlicerDiskRepresentation3D&) = delete;
  
  vtkSmartPointer<vtkDiskSource> DiskSource;
  vtkSmartPointer<vtkPolyDataMapper> DiskMapper;
  vtkSmartPointer<vtkActor> DiskActor;
  
  vtkSmartPointer<vtkProperty> DiskProperty;
  
  bool DescribePointsProximity(double * closestPoint, double * farthestPoint,
                              double& innerRadius, double& outerRadius);
};

#endif // __vtkslicerdiskrepresentation3d_h_
