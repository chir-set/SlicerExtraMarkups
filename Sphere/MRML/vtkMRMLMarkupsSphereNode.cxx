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

#include "vtkMRMLMarkupsSphereNode.h"
#include "vtkMRMLMeasurementSphere.h"

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkCollection.h>

#include <vtkMRMLSliceNode.h>

//--------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsSphereNode);

//--------------------------------------------------------------------------------
vtkMRMLMarkupsSphereNode::vtkMRMLMarkupsSphereNode()
{
  this->RequiredNumberOfControlPoints = 2;
  this->MaximumNumberOfControlPoints = 2;
  
  vtkNew<vtkMRMLMeasurementSphere> radiusMeasurement;
  radiusMeasurement->SetName("radius");
  radiusMeasurement->SetUnits("mm");
  radiusMeasurement->SetPrintFormat("%-#4.4g%s");
  radiusMeasurement->SetInputMRMLNode(this);
  radiusMeasurement->SetEnabled(true);
  this->Measurements->AddItem(radiusMeasurement);
  
  vtkNew<vtkMRMLMeasurementSphere> areaMeasurement;
  areaMeasurement->SetName("area");
  areaMeasurement->SetUnits("cm2");
  areaMeasurement->SetDisplayCoefficient(0.01);
  areaMeasurement->SetPrintFormat("%-#4.4g%s");
  areaMeasurement->SetInputMRMLNode(this);
  areaMeasurement->SetEnabled(false);
  this->Measurements->AddItem(areaMeasurement);
  
  vtkNew<vtkMRMLMeasurementSphere> volumeMeasurement;
  volumeMeasurement->SetName("volume");
  volumeMeasurement->SetUnits("cm3");
  volumeMeasurement->SetDisplayCoefficient(0.01);
  volumeMeasurement->SetPrintFormat("%-#4.4g%s");
  volumeMeasurement->SetInputMRMLNode(this);
  volumeMeasurement->SetEnabled(false);
  this->Measurements->AddItem(volumeMeasurement);
}

//--------------------------------------------------------------------------------
vtkMRMLMarkupsSphereNode::~vtkMRMLMarkupsSphereNode()=default;

//----------------------------------------------------------------------------
void vtkMRMLMarkupsSphereNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

//----------------------------API only----------------------------------------
void vtkMRMLMarkupsSphereNode::SetRadius(double radius)
{
  if (radius <= 0)
  {
    vtkErrorMacro("Radius must be greater than zero.");
    return;
  }
  double rasP1[3] = { 0.0 };
  double rasP2[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, rasP1);
  this->GetNthControlPointPositionWorld(1, rasP2);
  const double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(rasP1, rasP2));
  const double currentRadius = (this->RadiusMode == Centered)
  ? lineLength : lineLength / 2;
  const double difference = radius - currentRadius;
  double rasP2Shifted[3] = { 0.0 };
  this->FindLinearCoordinateByDistance(rasP1, rasP2, rasP2Shifted, difference);
  
  this->SetNthControlPointPositionWorld(1, rasP2Shifted);
  // Don't move center, move p1.
  if (this->RadiusMode == Circumferential)
  {
    double rasP1Shifted[3] = { 0.0 };
    this->FindLinearCoordinateByDistance(rasP2, rasP1, rasP1Shifted, difference);
    this->SetNthControlPointPositionWorld(0, rasP1Shifted);
  }
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsSphereNode::ResliceToControlPoints()
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
  double rasNormal[3] = { 0.0 };
  this->GetNthControlPointPositionWorld(0, rasP1);
  this->GetNthControlPointPositionWorld(1, rasP2);
  
  vtkMath::Cross(rasP1, rasP2, rasNormal);
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

//----------------------------------------------------------------------------
void vtkMRMLMarkupsSphereNode::FindLinearCoordinateByDistance(const double * p1, const double * p2,
                                                            double * result, const double difference)
{
  // Relative to p1, itself placed at origin. Simplifies reasoning.
  const double rp2[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
  const double lineLength = std::sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  const double factor = (1.0 + (difference / lineLength));
  result[0] = p1[0] + (rp2[0] * factor);
  result[1] = p1[1] + (rp2[1] * factor);
  result[2] = p1[2] + (rp2[2] * factor);
}
