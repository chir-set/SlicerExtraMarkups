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

#ifndef __vtkslicerLabelrepresentation3d_h_
#define __vtkslicerLabelrepresentation3d_h_

#include "vtkSlicerLabelModuleVTKWidgetsExport.h"

// Markups VTKWidgets includes
#include "vtkSlicerMarkupsWidgetRepresentation3D.h"

// VTK includes
#include <vtkWeakPointer.h>
#include <vtkConeSource.h>
#include <vtkLineSource.h>

//------------------------------------------------------------------------------
/**
 * @class   vtkSlicerLabelRepresentation3D
 * @brief   Default representation for the line widget
 *
 * This class provides the default concrete representation for the
 * vtkMRMLAbstractWidget. See vtkMRMLAbstractWidget
 * for details.
 * @sa
 * vtkSlicerMarkupsWidgetRepresentation2D vtkMRMLAbstractWidget
*/

class VTK_SLICER_LABEL_MODULE_VTKWIDGETS_EXPORT vtkSlicerLabelRepresentation3D
: public vtkSlicerMarkupsWidgetRepresentation3D
{
public:
  static vtkSlicerLabelRepresentation3D* New();
  vtkTypeMacro(vtkSlicerLabelRepresentation3D, vtkSlicerMarkupsWidgetRepresentation3D);
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
  vtkSlicerLabelRepresentation3D();
  ~vtkSlicerLabelRepresentation3D() override;

  void UpdateTagFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);
  void UpdatePointerFromMRML(vtkMRMLNode* caller, unsigned long event, void* callData=nullptr);

  vtkSmartPointer<vtkLineSource> LineSource;
  vtkSmartPointer<vtkConeSource> ConeSource;
  vtkSmartPointer<vtkAppendPolyData> AppendedArrow;
  vtkSmartPointer<vtkPolyDataMapper> ArrowMapper;
  vtkSmartPointer<vtkActor> ArrowActor;
  
private:
  vtkSlicerLabelRepresentation3D(const vtkSlicerLabelRepresentation3D&) = delete;
  void operator=(const vtkSlicerLabelRepresentation3D&) = delete;
  
};

#endif // __vtkslicerLabelrepresentation3d_h_
