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

#ifndef __vtkSlicerSphereMarkupslogic_h_
#define __vtkSlicerSphereMarkupslogic_h_

#include <vtkSlicerMarkupsLogic.h>

#include "vtkSlicerSphereModuleLogicExport.h"

class VTK_SLICER_SPHERE_MODULE_LOGIC_EXPORT vtkSlicerSphereLogic:
  public vtkSlicerMarkupsLogic
{
public:
  static vtkSlicerSphereLogic* New();
  vtkTypeMacro(vtkSlicerSphereLogic, vtkSlicerMarkupsLogic);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkSlicerSphereLogic();
  ~vtkSlicerSphereLogic() override;

  void RegisterNodes() override;

private:
  vtkSlicerSphereLogic(const vtkSlicerSphereLogic&) = delete;
  void operator=(const vtkSlicerSphereLogic&) = delete;
};

#endif // __vtkSlicerSphereMarkupslogic_h_
