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

#include <vtkMRMLSliceNode.h>

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

//----------------------------------------------------------------------------
void vtkMRMLMarkupsRingNode::ResliceToRingPlane()
{
  if (!this->ResliceNode)
  {
    return;
  }
  vtkMRMLSliceNode * resliceNode = vtkMRMLSliceNode::SafeDownCast(this->ResliceNode);
  if (!resliceNode)
  {
    return;
  }
  double rasP1[3] = { 0.0 };
  double rasP2[3] = { 0.0 };
  double rasP3[3] = { 0.0 };
  double rasNormal[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, rasP1);
  this->GetNthControlPointPositionWorld(1, rasP2);
  this->GetNthControlPointPositionWorld(2, rasP3);
  
  // Relative to rasP1 (center)
  double rRasP2[3] = { rasP2[0] - rasP1[0], rasP2[1] - rasP1[1], rasP2[2] - rasP1[2] };
  double rRasP3[3] = { rasP3[0] - rasP1[0], rasP3[1] - rasP1[1], rasP3[2] - rasP1[2] };

  vtkMath::Cross(rRasP2, rRasP3, rasNormal);
  if (rasNormal[0] == 0.0 && rasNormal[1] == 0.0 && rasNormal[2] == 0.0)
  {
    return;
  }
  resliceNode->SetSliceToRASByNTP(
    rasNormal[0], rasNormal[1], rasNormal[2],
    rasP2[0], rasP2[1], rasP2[2],
    rasP1[0], rasP1[1], rasP1[2],
    0);
  resliceNode->UpdateMatrices();
}
