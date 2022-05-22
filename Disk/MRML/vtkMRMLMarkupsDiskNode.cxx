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

#include "vtkMRMLMarkupsDiskNode.h"
#include "vtkMRMLMeasurementDisk.h"

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkCollection.h>

//--------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMarkupsDiskNode);

//--------------------------------------------------------------------------------
vtkMRMLMarkupsDiskNode::vtkMRMLMarkupsDiskNode()
{
  /*
   * Point 0 : always the center.
   */
  this->MaximumNumberOfControlPoints = 3;
  this->RequiredNumberOfControlPoints = 3;
  
  auto addLengthMeasurement = [&] (const char * name, bool enabled)
  {
    vtkNew<vtkMRMLMeasurementDisk> measurement;
    measurement->SetUnits("mm");
    measurement->SetPrintFormat("%-#4.4g%s");
    measurement->SetName(name);
    measurement->SetInputMRMLNode(this);
    measurement->SetEnabled(enabled);
    this->Measurements->AddItem(measurement);
  };
  
  addLengthMeasurement("innerRadius", true);
  addLengthMeasurement("outerRadius", true);
  addLengthMeasurement("width", false);
  
  auto addAreaMeasurement = [&](const char * name) {
    vtkNew<vtkMRMLMeasurementDisk> measurement;
    measurement->SetUnits("cm2");
    measurement->SetDisplayCoefficient(0.01);
    measurement->SetPrintFormat("%-#4.4g%s");
    measurement->SetName(name);
    measurement->SetInputMRMLNode(this);
    measurement->SetEnabled(false);
    this->Measurements->AddItem(measurement);
  };
  
  addAreaMeasurement("area");
  addAreaMeasurement("innerArea");
  addAreaMeasurement("outerArea");
}

//--------------------------------------------------------------------------------
vtkMRMLMarkupsDiskNode::~vtkMRMLMarkupsDiskNode()=default;

//----------------------------------------------------------------------------
void vtkMRMLMarkupsDiskNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}
