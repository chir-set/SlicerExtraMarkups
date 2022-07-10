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

#ifndef __vtkslicerslicingcontourwidget_h_
#define __vtkslicerslicingcontourwidget_h_

#include "vtkSlicerShapeModuleVTKWidgetsExport.h"

#include <vtkSlicerMarkupsWidget.h>

class VTK_SLICER_SHAPE_MODULE_VTKWIDGETS_EXPORT vtkSlicerShapeWidget
: public vtkSlicerMarkupsWidget
{
public:
  static vtkSlicerShapeWidget *New();
  vtkTypeMacro(vtkSlicerShapeWidget, vtkSlicerMarkupsWidget);

  void CreateDefaultRepresentation(vtkMRMLMarkupsDisplayNode* markupsDisplayNode,
                                  vtkMRMLAbstractViewNode* viewNode,
                                  vtkRenderer* renderer) override;

  /// Create instance of the markups widget
  VTK_NEWINSTANCE
  virtual vtkSlicerMarkupsWidget* CreateInstance() const override;

protected:
  vtkSlicerShapeWidget();
  ~vtkSlicerShapeWidget() override;

private:
  vtkSlicerShapeWidget(const vtkSlicerShapeWidget&) = delete;
  void operator=(const vtkSlicerShapeWidget) = delete;
};

#endif // __vtkslicerslicingcontourwidget_h_
