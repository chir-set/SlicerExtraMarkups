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
#include "vtkMRMLMeasurementRing.h"

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkCollection.h>

//--------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsRingNode);

//--------------------------------------------------------------------------------
vtkMRMLMarkupsRingNode::vtkMRMLMarkupsRingNode()
{
  // Third point is used to calculate normal relative to the center in 3D view.
  this->MaximumNumberOfControlPoints = 3;
  this->RequiredNumberOfControlPoints = 3;
  
  vtkNew<vtkMRMLMeasurementRing> radiusMeasurement;
  radiusMeasurement->SetName("radius");
  radiusMeasurement->SetUnits("mm");
  radiusMeasurement->SetPrintFormat("%-#4.4g%s");
  radiusMeasurement->SetInputMRMLNode(this);
  radiusMeasurement->SetEnabled(true);
  this->Measurements->AddItem(radiusMeasurement);
  
  vtkNew<vtkMRMLMeasurementRing> areaMeasurement;
  areaMeasurement->SetName("area");
  areaMeasurement->SetUnits("cm2");
  areaMeasurement->SetDisplayCoefficient(0.01);
  areaMeasurement->SetPrintFormat("%-#4.4g%s");
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
