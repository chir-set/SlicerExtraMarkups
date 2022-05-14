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

#ifndef __vtkSlicerRingMarkupslogic_h_
#define __vtkSlicerRingMarkupslogic_h_

#include <vtkSlicerMarkupsLogic.h>

#include "vtkSlicerRingModuleLogicExport.h"

class VTK_SLICER_RING_MODULE_LOGIC_EXPORT vtkSlicerRingLogic:
  public vtkSlicerMarkupsLogic
{
public:
  static vtkSlicerRingLogic* New();
  vtkTypeMacro(vtkSlicerRingLogic, vtkSlicerMarkupsLogic);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkSlicerRingLogic();
  ~vtkSlicerRingLogic() override;

  void RegisterNodes() override;

private:
  vtkSlicerRingLogic(const vtkSlicerRingLogic&) = delete;
  void operator=(const vtkSlicerRingLogic&) = delete;
};

#endif // __vtkSlicerRingMarkupslogic_h_
