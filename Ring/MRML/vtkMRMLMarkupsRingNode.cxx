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

#include "vtkMRMLMarkupsRingNode.h"
#include "vtkMRMLMeasurementRingArea.h"

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkCollection.h>

//--------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsRingNode);

//--------------------------------------------------------------------------------
vtkMRMLMarkupsRingNode::vtkMRMLMarkupsRingNode()
{
  this->UpdateMeasurementLabel(Mode);
  
  vtkNew<vtkMRMLMeasurementRingArea> areaMeasurement;
  areaMeasurement->SetName("area");
  areaMeasurement->SetInputMRMLNode(this);
  areaMeasurement->SetEnabled(false);
  this->Measurements->AddItem(areaMeasurement);
}

//--------------------------------------------------------------------------------
vtkMRMLMarkupsRingNode::~vtkMRMLMarkupsRingNode()=default;

//----------------------------------------------------------------------------
void vtkMRMLMarkupsRingNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

void vtkMRMLMarkupsRingNode::SetMode(int mode)
{
  Mode = mode;
  this->UpdateMeasurementLabel(Mode);
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsRingNode::UpdateMeasurementLabel(int mode)
{
  vtkMRMLMeasurement * lengthMeasurement = this->GetNthMeasurement(0); // The first one is length, natively.
  if (!lengthMeasurement)
  {
    return; // Should not happen.
  }
  if (mode == Centered)
  {
    lengthMeasurement->SetName("radius");
  }
  else
  {
    lengthMeasurement->SetName("diameter");
  }
  /* The label of the checkbox in the widget is never changed to 'diameter',
   * and the checkbox does not work in 'Circumferential' mode.
   * The two other labels are updated.
   * Switching between nodes updates all labels.
  */
  this->UpdateAllMeasurements();
}
