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

#ifndef __vtkmrmlmarkupsdisknode_h_
#define __vtkmrmlmarkupsdisknode_h_

#include <vtkMRMLMarkupsNode.h>

#include "vtkSlicerDiskModuleMRMLExport.h"

//-----------------------------------------------------------------------------
class VTK_SLICER_DISK_MODULE_MRML_EXPORT vtkMRMLMarkupsDiskNode
: public vtkMRMLMarkupsNode
{
public:
  enum
  {
    WorldProjection = 0,
    WorldIntersection,
    SliceProjection
  };
  static vtkMRMLMarkupsDiskNode* New();
  vtkTypeMacro(vtkMRMLMarkupsDiskNode, vtkMRMLMarkupsNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //--------------------------------------------------------------------------------
  // MRMLNode methods
  //--------------------------------------------------------------------------------
  const char* GetIcon() override {return ":/Icons/MarkupsGeneric.png";}
  const char* GetAddIcon() override {return ":/Icons/MarkupsGenericMouseModePlace.png";}
  const char* GetPlaceAddIcon() override {return ":/Icons/MarkupsGenericMouseModePlaceAdd.png";}

  vtkMRMLNode* CreateNodeInstance() override;
  /// Get node XML tag name (like Volume, Model)
  ///
  const char* GetNodeTagName() override {return "MarkupsDisk";}

  /// Get markup type internal name
  const char* GetMarkupType() override {return "Disk";}

  // Get markup type GUI display name
  const char* GetTypeDisplayName() override {return "Disk";};

  /// Get markup short name
  const char* GetDefaultNodeNamePrefix() override {return "D";}

  /// \sa vtkMRMLNode::CopyContent
  vtkMRMLCopyContentDefaultMacro(vtkMRMLMarkupsDiskNode);
  
  vtkSetMacro(DrawMode2D, int);
  vtkGetMacro(DrawMode2D, int);
  vtkSetMacro(Resolution, double);
  vtkGetMacro(Resolution, double);
  vtkSetMacro(InnerRadius, double);
  vtkGetMacro(InnerRadius, double);
  vtkSetMacro(OuterRadius, double);
  vtkGetMacro(OuterRadius, double);
  
  vtkPolyData * GetDiskWorld() const {return this->DiskWorld;}
  // Used by 3D representation.
  void SetDiskWorld(vtkPolyData * polydata) {this->DiskWorld = polydata;}
  
  vtkSetObjectMacro(ResliceNode, vtkMRMLNode);
  vtkGetObjectMacro(ResliceNode, vtkMRMLNode);
  
  void ResliceToDiskPlane();
  bool DescribePointsProximity(double * closestPoint, double * farthestPoint,
                               double& innerRadius, double& outerRadius);

protected:
  vtkMRMLMarkupsDiskNode();
  ~vtkMRMLMarkupsDiskNode() override;
  vtkMRMLMarkupsDiskNode(const vtkMRMLMarkupsDiskNode&);
  void operator=(const vtkMRMLMarkupsDiskNode&);

private:
  int DrawMode2D { WorldProjection };
  double Resolution { 45.0 };
  double InnerRadius { 0.0 };
  double OuterRadius { 0.0 };
  
  vtkPolyData * DiskWorld = nullptr;
  vtkMRMLNode * ResliceNode = nullptr;
};

#endif //vtkmrmlmarkupsdisknode_h_
