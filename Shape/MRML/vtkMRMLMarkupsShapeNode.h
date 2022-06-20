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

#ifndef __vtkmrmlmarkupsShape_LOWERnode_h_
#define __vtkmrmlmarkupsShape_LOWERnode_h_

#include <vtkMRMLMarkupsNode.h>

#include "vtkSlicerShapeModuleMRMLExport.h"

//-----------------------------------------------------------------------------
class VTK_SLICER_SHAPE_MODULE_MRML_EXPORT vtkMRMLMarkupsShapeNode
: public vtkMRMLMarkupsNode
{
public:
  static vtkMRMLMarkupsShapeNode* New();
  vtkTypeMacro(vtkMRMLMarkupsShapeNode, vtkMRMLMarkupsNode);
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
  const char* GetNodeTagName() override {return "MarkupsShape";}

  /// Get markup type internal name
  const char* GetMarkupType() override {return "Shape";}

  // Get markup type GUI display name
  const char* GetTypeDisplayName() override {return "Shape";};

  /// Get markup short name
  const char* GetDefaultNodeNamePrefix() override {return "SH";}

  /// \sa vtkMRMLNode::CopyContent
  vtkMRMLCopyContentDefaultMacro(vtkMRMLMarkupsShapeNode);

protected:
  vtkMRMLMarkupsShapeNode();
  ~vtkMRMLMarkupsShapeNode() override;
  vtkMRMLMarkupsShapeNode(const vtkMRMLMarkupsShapeNode&);
  void operator=(const vtkMRMLMarkupsShapeNode&);

private:
  vtkPolyData *TargetOrgan = nullptr;
};

#endif //vtkmrmlmarkupsShape_LOWERnode_h_
