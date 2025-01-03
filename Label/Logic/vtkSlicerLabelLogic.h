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

#ifndef __vtkSlicerLabelMarkupslogic_h_
#define __vtkSlicerLabelMarkupslogic_h_

#include <vtkSlicerMarkupsLogic.h>

#include "vtkSlicerLabelModuleLogicExport.h"

class VTK_SLICER_LABEL_MODULE_LOGIC_EXPORT vtkSlicerLabelLogic:
  public vtkSlicerMarkupsLogic
{
public:
  static vtkSlicerLabelLogic* New();
  vtkTypeMacro(vtkSlicerLabelLogic, vtkSlicerMarkupsLogic);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkSlicerLabelLogic();
  ~vtkSlicerLabelLogic() override;

  void RegisterNodes() override;
  void OnMRMLSceneNodeAdded(vtkMRMLNode * node) override;
  void GenerateUniqueColor(double color[3], const std::string& colorNodeID);

private:
  vtkSlicerLabelLogic(const vtkSlicerLabelLogic&) = delete;
  void operator=(const vtkSlicerLabelLogic&) = delete;
  int NextColorIndex = 1;
};

#endif // __vtkSlicerLabelMarkupslogic_h_
